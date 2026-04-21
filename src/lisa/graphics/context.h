//
// Created by kinami on 3/24/26.
//

#ifndef LISA_GRAPHICS_H
#define LISA_GRAPHICS_H

#include "descriptors/DescriptorContainer.h"
#include "device/Instance.h"
#include "device/LogicalDevice.h"
#include "memory/MemoryAllocator.h"
#include "swapchain/Surface.h"
#include "swapchain/Swapchain.h"

namespace lisa::graphics {
  class Swapchain;
}

namespace lisa::graphics::context {
  void init();
  void destroy();

  const Instance& instance();
  const PhysicalDevice& physical_device();
  const LogicalDevice& device();
  const MemoryAllocator& allocator();
  const Surface& surface();
  Swapchain& swapchain();
  DescriptorContainer& descriptor_container();

  void recreate_swapchain();
}

#endif
