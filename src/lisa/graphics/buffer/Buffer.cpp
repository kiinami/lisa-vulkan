//
// Created by kinami on 4/2/26.
//

#include "Buffer.h"

#include "graphics/context.h"

namespace lisa::graphics {
  Buffer::Buffer(
    const vk::DeviceSize size,
    const vk::BufferUsageFlags usage,
    const vma::AllocationCreateInfo& allocation_ci
  ) {
    const vk::BufferCreateInfo buffer_ci{.size = size, .usage = usage};
    buffer_ = context::allocator().create_buffer(buffer_ci, allocation_ci);
  }

  Buffer Buffer::from_data(
    const void* data,
    const size size,
    const vk::BufferUsageFlags usage,
    const vma::AllocationCreateInfo& allocation_ci
  ) {
    const auto staging_buffer = Buffer(
      size,
      vk::BufferUsageFlagBits::eTransferSrc,
      {.flags = vma::AllocationCreateFlagBits::eMapped |
                vma::AllocationCreateFlagBits::eHostAccessSequentialWrite,
       .usage = vma::MemoryUsage::eAuto}
    );
    std::memcpy(staging_buffer.mapped_data(), data, size);

    auto buffer = Buffer(size, usage, allocation_ci);

    auto cmdb = context::device().cmd_buffer();
    cmdb.begin_onetime();
    const vk::BufferCopy copy_region{.size = size};
    cmdb->copyBuffer(staging_buffer, buffer, copy_region);
    cmdb->end();
    context::device().submit_cmd_buffer_with_fence(cmdb);

    return buffer;
  }

  vk::DeviceAddress Buffer::address() const {
    const vk::BufferDeviceAddressInfo buffer_device_ai{.buffer = buffer_};
    return context::device()->getBufferAddress(buffer_device_ai);
  }

  void* Buffer::mapped_data() const {
    return allocation().getInfo().pMappedData;
  }
}
