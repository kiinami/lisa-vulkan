//
// Created by kinami on 4/1/26.
//

#include "context.h"

#include "SDL3pp/SDL3pp_vulkan.h"
#include "utils/logging.h"

#include <SDL3/SDL_events.h>
#include <memory>

namespace lisa::window::context {
  namespace {
    uptr<Window> window_;
    bool should_close_ = false;
  }

  void init(const int width, const int height) {
    SDL::Init(SDL::INIT_VIDEO);
    SDL::Vulkan_LoadLibrary(nullptr);

    Window::WindowSize size{width, height};
    window_ = std::make_unique<Window>(size);
    should_close_ = false;

    logging::debug("Window context initiated");
  }

  void destroy() {
    window_.reset();
    SDL::QuitSubSystem(SDL::INIT_VIDEO);
    SDL::Quit();
    logging::debug("Window context destroyed");
  }

  const Window& window() { return *window_; }

  vec2 window_size() {
    auto [x, y] = window_.get()->size();
    return {x, y};
  }

  uint32 window_width() { return window_size().x; }

  uint32 window_height() { return window_size().y; }

  bool should_close() { return should_close_; }

  void poll_events() {
    SDL_Event event;
    while (SDL_PollEvent(&event))
      if (event.type == SDL_EVENT_QUIT) should_close_ = true;
  }
}
