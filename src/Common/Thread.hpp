//
// Copyright (C) 2015 Red Hat, Inc.
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//
// Authors: Daniel Kopecek <dkopecek@redhat.com>
//
#ifndef USBFW_THREAD_HPP
#define USBFW_THREAD_HPP

#include "Logging.hpp"
#include <thread>
#include <atomic>

namespace usbguard
{
  template<class C>
  class Thread
  {
  public:
    Thread(C* method_class_ptr, void(C::*method)())
    {
      _method_class_ptr = method_class_ptr;
      _method = method;
      _stop_request = false;
    }

    Thread(Thread& thread)
    {
      _method_class_ptr = thread._method_class_ptr;
      _method = thread._method;
      std::swap(_thread, thread._thread);
      _stop_request = thread._stop_request;
    }

    Thread& operator=(Thread& thread)
    {
      _method_class_ptr = thread._method_class_ptr;
      _method = thread._method;
      std::swap(_thread, thread._thread);
      _stop_request = thread._stop_request;
      return *this;
    }

    ~Thread()
    {
    }

    void start()
    {
      _thread = std::thread(_method, _method_class_ptr);
    }

    /**
     * Set the termination flag and wait for the thread
     * to exit.
     */
    void stop(bool do_wait = true)
    {
      _stop_request = true;
      if (do_wait) {
	wait();
      }
    }

    void wait()
    {
      if (_thread.joinable()) {
	try {
	  _thread.join();
	} catch(const std::system_error& ex) {
	  throw;
	}
      }
      _stop_request = false;
    }

    bool running() const
    {
      return (_thread.get_id() != std::thread::id());
    }

    bool stopRequested() const
    {
      return _stop_request;
    }

  private:
    C* _method_class_ptr;
    void(C::*_method)();
    std::thread _thread;
    std::atomic_bool _stop_request;
  };

} /* namespace usbguard */

#endif /* USBFW_THREAD_HPP */