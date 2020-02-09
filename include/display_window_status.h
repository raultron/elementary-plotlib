#pragma once

#include <mutex>

#include "display_window.h"
#include "notify_request.h"
#include "status_notifier.h"
#include "window_flags.h"

namespace graphics {

/* Extend the interface 'display_window'
 * by providing a mutex for lock/unlock methods and a status variable
 * with the related methods to query/set status and issue a request. */
class display_window_status : public display_window {
public:
    int status() {
        return m_window_status.value();
    }

    // Important: this method should be called only from
    // the window's thread.
    void set_status(window_status_e new_status) {
        m_window_status.set(new_status);
    }

    request_error_e wait_until_notification(window_status_e notify_status) {
        notify_request<window_status_e> req{notify_status};
        request_error_e retval = m_window_status.set_notify_request(req);
        if (retval == request_success) {
            req.wait();
        }
        return retval;
    }
protected:
    // Lock methods should be called only from window's thread.
    void lock() {
        m_mutex.lock();
    }
    void unlock() {
        m_mutex.unlock();
    }
private:
    std::mutex m_mutex;
    status_notifier<window_status_e> m_window_status;
};

}
