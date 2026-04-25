//
// Created by kinami on 4/25/26.
//

#include "events.h"

#include "context.h"
#include "utils/logging.h"

namespace lisa::window::events {
  void on_window_close(const WindowClose& event) {
    logging::debug("Closing window");
  }

  REGISTER_EVENT(WindowClose, on_window_close);

  void on_mouse_button_down(const MouseButtonDown& event) {
    if (event.button == SDL::BUTTON_LEFT)
      SDL_SetWindowRelativeMouseMode(SDL::WindowRef(context::window()), true);
  }

  REGISTER_EVENT(MouseButtonDown, on_mouse_button_down);

  void on_mouse_button_up(const MouseButtonUp& event) {
    if (event.button == SDL::BUTTON_LEFT)
      SDL_SetWindowRelativeMouseMode(SDL::WindowRef(context::window()), false);
  }

  REGISTER_EVENT(MouseButtonUp, on_mouse_button_up);

}
