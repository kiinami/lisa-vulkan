//
// Created by kinami on 3/31/26.
//

#ifndef LISA_VULKAN_LOGICALDEVICE_H
#define LISA_VULKAN_LOGICALDEVICE_H
#include "graphics/commands/Queue.h"

#include <vulkan/vulkan_raii.hpp>

namespace lisa::graphics {
  class LogicalDevice {
  public:
    explicit LogicalDevice(const vk::raii::PhysicalDevice& physical_device);
    ~LogicalDevice();

    operator const vk::raii::Device&() const { return device_; }

    const Queue& queue() { return queue_.value(); }

  private:
    vk::raii::Device device_ = nullptr;
    std::optional<Queue> queue_;
  };
}

#endif // LISA_VULKAN_LOGICALDEVICE_H
