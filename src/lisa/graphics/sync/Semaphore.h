//
// Created by kinami on 4/4/26.
//

#ifndef LISA_VULKAN_SEMAPHORE_H
#define LISA_VULKAN_SEMAPHORE_H

#include "graphics/vk/VkObject.h"

#include <vulkan/vulkan_raii.hpp>

namespace lisa::graphics {

  class Semaphore : public VkObject<vk::raii::Semaphore> {
  public:
    explicit Semaphore(vk::SemaphoreCreateFlags flags = {});
  };

}

#endif // LISA_VULKAN_SEMAPHORE_H
