//
// Created by kinami on 6/18/26.
//

#ifndef LISA_VULKAN_ACCELERATION_STRUCTURE_H
#define LISA_VULKAN_ACCELERATION_STRUCTURE_H

#ifdef VK_KHR_acceleration_structure

#include "graphics/buffer/Buffer.h"
#include "graphics/commands/CommandBuffer.h"
#include "utils/common.h"

#include <vulkan/vulkan_raii.hpp>

namespace lisa::graphics {

  class AccelerationStructure {
  public:
    static AccelerationStructure build_blas(
      const str& id,
      const Buffer& vertex_buffer,
      uint32 vertex_count,
      const Buffer& index_buffer,
      uint32 index_count,
      const CommandBuffer& cmdb
    );

    vk::AccelerationStructureKHR handle() const { return *handle_; }
    vk::DeviceAddress address() const { return address_; }

  private:
    AccelerationStructure() = default;

    vk::raii::AccelerationStructureKHR handle_{nullptr};
    Buffer buffer_;
    vk::DeviceAddress address_ = 0;
  };

}

#endif // VK_KHR_acceleration_structure
#endif // LISA_VULKAN_ACCELERATION_STRUCTURE_H
