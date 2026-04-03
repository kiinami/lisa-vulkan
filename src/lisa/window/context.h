//
// Created by kinami on 4/1/26.
//

#ifndef LISA_VULKAN_CONTEXT_H
#define LISA_VULKAN_CONTEXT_H
#include "Window.h"

namespace lisa::window::context {
  void init(int width, int height);
  void destroy();

  const Window& window();
}

#endif // LISA_VULKAN_CONTEXT_H
