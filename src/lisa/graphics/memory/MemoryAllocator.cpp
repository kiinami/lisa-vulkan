//
// Created by kinami on 3/31/26.
//

#include "MemoryAllocator.h"

#include "graphics/device/Instance.h"
#include "graphics/device/LogicalDevice.h"
#include "utils/logging.h"

#include <vk_mem_alloc_raii.hpp>

namespace lisa::graphics {
  MemoryAllocator::MemoryAllocator(
    const Instance& instance,
    const PhysicalDevice& physical_device,
    const LogicalDevice& device
  ) :
    allocator_(
      vma::raii::Allocator{
        instance, device, vma::AllocatorCreateInfo{{}, physical_device}
      }
    ) {
    logging::debug("Memory allocator created");
  }

  vma::raii::Image MemoryAllocator::create_image(
    const vk::ImageCreateInfo& image_ci,
    const vma::AllocationCreateInfo& allocation_ci
  ) const {
    return allocator_.createImage(image_ci, allocation_ci);
  }

  vma::raii::Buffer MemoryAllocator::create_buffer(
    const vk::BufferCreateInfo& buffer_ci,
    const vma::AllocationCreateInfo& allocation_ci
  ) const {
    return allocator_.createBuffer(buffer_ci, allocation_ci);
  }
}
