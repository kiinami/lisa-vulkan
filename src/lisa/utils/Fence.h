//
// Created by kinami on 4/2/26.
//

#ifndef LISA_VULKAN_FENCE_H
#define LISA_VULKAN_FENCE_H
#include "graphics/device/LogicalDevice.h"

#include <vulkan/vulkan_raii.hpp>

namespace lisa::graphics::utils {

  class Fence {
  public:
    explicit Fence(const vk::raii::Device& device) :
      fence_(device.createFence({})) {}

    ~Fence() = default;

  private:
    vk::raii::Fence fence_ = nullptr;
  };

}

#endif // LISA_VULKAN_FENCE_H
