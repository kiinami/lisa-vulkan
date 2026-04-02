//
// Created by kinami on 3/24/26.
//

#ifndef LISA_GRAPHICS_H
#define LISA_GRAPHICS_H

#include "utils/common.h"
#include "device/Instance.h"
#include "memory/MemoryAllocator.h"

namespace lisa::graphics::context {
  void init();
  void destroy();

  const Instance& instance();
  const PhysicalDevice& physical_device();
  const LogicalDevice& device();
  const MemoryAllocator& allocator();
}

#endif
