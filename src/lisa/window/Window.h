//
// Created by kinami on 4/1/26.
//

#ifndef LISA_VULKAN_WINDOW_H
#define LISA_VULKAN_WINDOW_H
#include <SDL3pp/SDL3pp.h>

namespace lisa::window {
  class Window {
  public:
    struct WindowSize {
      int x;
      int y;

      operator SDL::Point() const { return {x, y}; }
    };

    explicit Window(WindowSize size);
    ~Window() = default;

    operator const SDL::Window&() const { return window_; }

    operator SDL::WindowRef() const { return SDL::WindowRef(window_); }

    WindowSize size() const;

  private:
    SDL::Window window_ = nullptr;
    mutable WindowSize size_;
    mutable bool size_dirty_ = true;
  };
}

#endif // LISA_VULKAN_WINDOW_H
