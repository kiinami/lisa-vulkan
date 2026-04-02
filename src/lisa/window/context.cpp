//
// Created by kinami on 4/1/26.
//

#include "context.h"

#include "SDL3pp/SDL3pp_vulkan.h"

#include <memory>

namespace lisa::window::context {
  namespace {
    std::unique_ptr<Window> window_;
  }

  void init(const int width, const int height) {
    SDL::Init(SDL::INIT_VIDEO);
    SDL::Vulkan_LoadLibrary(nullptr);

    Window::WindowSize size{width, height};
    window_ = std::make_unique<Window>(size);
  }

  void destroy() {
    SDL::QuitSubSystem(SDL::INIT_VIDEO);
    SDL::Quit();
  }

  const Window& window() { return *window_; }
}
