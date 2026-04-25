//
// Created by kinami on 4/1/26.
//

#ifndef LISA_VULKAN_WINDOW_CONTEXT_H
#define LISA_VULKAN_WINDOW_CONTEXT_H
#include "Window.h"
#include "utils/common.h"

#include <entt/signal/dispatcher.hpp>

namespace lisa::window::context {
  void init(int width, int height);
  void destroy();

  const Window& window();
  vec2 window_size();
  uint32 window_width();
  uint32 window_height();
  vec2 texel_size();
  vec2 from_scale(float scale);
  bool should_close();
  bool was_resized();
  entt::dispatcher& dispatcher();
  void poll_events();
}

#endif // LISA_VULKAN_WINDOW_CONTEXT_H
