//
// Created by kinami on 4/1/26.
//

#ifndef LISA_VULKAN_WINDOW_CONTEXT_H
#define LISA_VULKAN_WINDOW_CONTEXT_H
#include "Window.h"
#include "utils/common.h"

namespace lisa::window::context {
  void init(int width, int height);
  void destroy();

  const Window& window();
  vec2 window_size();
  uint32 window_width();
  uint32 window_height();
  bool should_close();
  void poll_events();
}

#endif // LISA_VULKAN_WINDOW_CONTEXT_H
