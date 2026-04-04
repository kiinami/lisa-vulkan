//
// Created by kinami on 3/31/26.
//

#ifndef LISA_VULKAN_MEMORYALLOCATOR_H
#define LISA_VULKAN_MEMORYALLOCATOR_H
#include "graphics/device/Instance.h"
#include "graphics/device/LogicalDevice.h"
#include "graphics/device/PhysicalDevice.h"

#include <vk_mem_alloc_raii.hpp>

namespace lisa::graphics {
  class MemoryAllocator {
  public:
    MemoryAllocator(
      const Instance& instance,
      const PhysicalDevice& physical_device,
      const LogicalDevice& device
    );
    ~MemoryAllocator() = default;

    operator const vma::raii::Allocator&() const { return allocator_; }

    vma::raii::Image create_image(
      const vk::ImageCreateInfo& image_ci,
      const vma::AllocationCreateInfo& allocation_ci = {
        .flags = vma::AllocationCreateFlagBits::eDedicatedMemory,
        .usage = vma::MemoryUsage::eAuto
      }
    ) const;

    vma::raii::Buffer create_buffer(
      const vk::BufferCreateInfo& buffer_ci,
      const vma::AllocationCreateInfo& allocation_ci = {
        .flags =
          vma::AllocationCreateFlagBits::eHostAccessSequentialWrite |
          vma::AllocationCreateFlagBits::eHostAccessAllowTransferInstead |
          vma::AllocationCreateFlagBits::eMapped,
        .usage = vma::MemoryUsage::eAuto,
      }
    ) const;

  private:
    vma::raii::Allocator allocator_;
  };
}

#endif // LISA_VULKAN_MEMORYALLOCATOR_H
