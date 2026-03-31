//
// Created by kinami on 3/31/26.
//

#ifndef LISA_VULKAN_LOGICALDEVICE_H
#define LISA_VULKAN_LOGICALDEVICE_H
#include <vulkan/vulkan_raii.hpp>

namespace lisa::graphics {
  class LogicalDevice {
  public:
    explicit LogicalDevice(const vk::raii::PhysicalDevice& physical_device);
    ~LogicalDevice();

    [[nodiscard]]
    const vk::raii::Device& vk_device() {
      return device_;
    }

  private:
    vk::raii::Device device_ = nullptr;
  };
}

#endif // LISA_VULKAN_LOGICALDEVICE_H
