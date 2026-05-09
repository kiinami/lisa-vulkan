//
// Created by kinami on 4/2/26.
//

#ifndef LISA_VULKAN_BUFFER_H
#define LISA_VULKAN_BUFFER_H
#include "graphics/commands/CommandBuffer.h"
#include "graphics/vk/NamedVkObject.h"
#include "utils/common.h"

#include <vk_mem_alloc_raii.hpp>

namespace lisa::graphics {

  class Buffer : public NamedVkObject<vma::raii::Buffer> {
  public:
    Buffer() = default;
    Buffer(
      const str& id,
      vk::DeviceSize size,
      vk::BufferUsageFlags usage,
      const vma::AllocationCreateInfo& allocation_ci = {}
    );

    const vma::raii::Allocation& allocation() const {
      return object_.getAllocation();
    }

    static Buffer from_data(
      const str& id,
      const CommandBuffer& cmdb,
      const void* data,
      size size,
      vk::BufferUsageFlags usage,
      const vma::AllocationCreateInfo& allocation_ci = {}
    );
    static Buffer from_data(
      const str& id,
      const void* data,
      size size,
      vk::BufferUsageFlags usage,
      const vma::AllocationCreateInfo& allocation_ci = {}
    );
    vk::DeviceAddress address() const;
    void* mapped_data() const;
  };

}

#endif // LISA_VULKAN_BUFFER_H
