//
// Created by kinami on 3/24/26.
//

#ifndef LISA_GRAPHICS_H
#define LISA_GRAPHICS_H
#include "lisa/utils/defines.h"
#include "device/Instance.h"

namespace lisa::graphics {
  void init_device(Instance& instance, int index);
  void destroy_device();

  vk::Instance instance();
  vk::Device device();
  vk::PhysicalDevice physical_device();
}

#endif
