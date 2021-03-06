#pragma once

#include <thread>

#include "window_close_callback.h"

/* The functions below are meant to be used with a Window type that:
   - inherits from display_window_status for the methods lock, unlock
     and wait_until_notification
   - implements the "start_blocking" method.

   The purpose of the start_window_new_thread function is to start a
   detached thread to run the window without blocking the calling
   thread.
*/

template <typename Window>
void run_window(Window *window, unsigned width, unsigned height, unsigned flags, window_close_callback *callback) {
    window->start_blocking(width, height, flags);
    if (callback) {
        callback->execute();
        delete callback;
    }
}

template <typename Window>
int start_window_new_thread(Window *window, unsigned width, unsigned height, unsigned flags, window_close_callback *callback) {
    std::thread window_thread(run_window<Window>, window, width, height, flags, callback);
    window_thread.detach();
    window->wait_for_status(graphics::window_running);
    return 0;
}
