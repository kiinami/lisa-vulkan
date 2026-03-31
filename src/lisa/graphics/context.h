//
// Created by kinami on 3/24/26.
//

#ifndef LISA_GRAPHICS_H
#define LISA_GRAPHICS_H

#include "utils/common.h"
#include "device/Instance.h"

namespace lisa::graphics::context {
  vk::raii::Context& context();
  Instance& instance();
  const vk::raii::Instance& vk_instance();
  const vk::raii::PhysicalDevice& vk_physical_device();

  void init();
  void destroy();
}

#endif
