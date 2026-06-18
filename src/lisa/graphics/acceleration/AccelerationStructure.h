//
// Created by kinami on 6/18/26.
//

#ifndef LISA_VULKAN_ACCELERATION_STRUCTURE_H
#define LISA_VULKAN_ACCELERATION_STRUCTURE_H

#ifdef VK_KHR_acceleration_structure

#include "graphics/buffer/Buffer.h"
#include "graphics/commands/CommandBuffer.h"
#include "graphics/vk/NamedVkObject.h"
#include "utils/common.h"

#include <vulkan/vulkan_raii.hpp>

namespace lisa::graphics {

  class AccelerationStructure :
    public NamedVkObject<vk::raii::AccelerationStructureKHR> {
  public:
    static AccelerationStructure build_blas(
      const str& id,
      const Buffer& vertex_buffer,
      uint32 vertex_count,
      const Buffer& index_buffer,
      uint32 index_count,
      const CommandBuffer& cmdb
    );

    vk::DeviceAddress address() const { return address_; }

  private:
    explicit AccelerationStructure(const str& id) :
      NamedVkObject(id) {}

    Buffer buffer_;
    vk::DeviceAddress address_ = 0;
  };

}

#endif // VK_KHR_acceleration_structure
#endif // LISA_VULKAN_ACCELERATION_STRUCTURE_H
