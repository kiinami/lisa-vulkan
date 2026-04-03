//
// Created by kinami on 4/1/26.
//

#include "Window.h"

#include "utils/chk.h"

namespace lisa::window {
  Window::WindowSize Window::size() const {
    if (size_dirty_) {
      SDL::GetWindowSize(window_, &size_.x, &size_.y);
      size_dirty_ = false;
    }
    return size_;
  }

  Window::Window(WindowSize size) {
    window_ = SDL::CreateWindow(
      "lisa",
      SDL::Point{size.x, size.y},
      SDL::WINDOW_VULKAN | SDL::WINDOW_RESIZABLE
    );
    assert(window_);
    Window::size();

    logging::debug("Window with size {}x{} created", size_.x, size_.y);
  }
}
