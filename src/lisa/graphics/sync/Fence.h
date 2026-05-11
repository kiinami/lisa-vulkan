//
// Created by kinami on 4/3/26.
//

#ifndef LISA_VULKAN_FENCE_H
#define LISA_VULKAN_FENCE_H

#include "graphics/vk/VkObject.h"

#include <vulkan/vulkan_raii.hpp>

namespace lisa::graphics {

  class Fence : public VkObject<vk::raii::Fence> {
  public:
    Fence();

    void wait() const;
    void reset() const;
  };

}

#endif // LISA_VULKAN_FENCE_H
