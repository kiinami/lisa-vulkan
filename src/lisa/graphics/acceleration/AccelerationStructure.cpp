//
// Created by kinami on 6/18/26.
//

#include "AccelerationStructure.h"

#ifdef VK_KHR_acceleration_structure

  #include "graphics/context.h"
  #include "resources/Vertex.h"
  #include "utils/logging.h"

namespace lisa::graphics {

  AccelerationStructure AccelerationStructure::build_blas(
    const str& id,
    const Buffer& vertex_buffer,
    const uint32 vertex_count,
    const Buffer& index_buffer,
    const uint32 index_count,
    const CommandBuffer& cmdb
  ) {
    const uint32 primitive_count = index_count / 3;

    const vk::AccelerationStructureGeometryTrianglesDataKHR triangles{
      .vertexFormat = vk::Format::eR32G32B32Sfloat,
      .vertexData = {.deviceAddress = vertex_buffer.address()},
      .vertexStride = sizeof(resources::Vertex),
      .maxVertex = vertex_count - 1,
      .indexType = vk::IndexType::eUint32,
      .indexData = {.deviceAddress = index_buffer.address()},
    };

    const vk::AccelerationStructureGeometryKHR geometry{
      .geometryType = vk::GeometryTypeKHR::eTriangles,
      .geometry = {.triangles = triangles},
      .flags = vk::GeometryFlagBitsKHR::eOpaque,
    };

    vk::AccelerationStructureBuildGeometryInfoKHR build_info{
      .type = vk::AccelerationStructureTypeKHR::eBottomLevel,
      .flags = vk::BuildAccelerationStructureFlagBitsKHR::ePreferFastTrace,
      .mode = vk::BuildAccelerationStructureModeKHR::eBuild,
      .geometryCount = 1,
      .pGeometries = &geometry,
    };

    const auto sizes = context::device()->getAccelerationStructureBuildSizesKHR(
      vk::AccelerationStructureBuildTypeKHR::eDevice,
      build_info,
      {primitive_count}
    );

    AccelerationStructure result(id);

    result.buffer_ = Buffer{
      logging::genid(id, "blas"),
      sizes.accelerationStructureSize,
      vk::BufferUsageFlagBits::eAccelerationStructureStorageKHR |
        vk::BufferUsageFlagBits::eShaderDeviceAddress,
      {.usage = vma::MemoryUsage::eAuto},
    };

    result.set(context::device()->createAccelerationStructureKHR({
      .buffer = result.buffer_.handle(),
      .size = sizes.accelerationStructureSize,
      .type = vk::AccelerationStructureTypeKHR::eBottomLevel,
    }));

    auto scratch = Buffer{
      logging::genid(id, "blas_scratch"),
      sizes.buildScratchSize,
      vk::BufferUsageFlagBits::eStorageBuffer |
        vk::BufferUsageFlagBits::eShaderDeviceAddress,
      {.usage = vma::MemoryUsage::eAuto},
    };

    build_info.dstAccelerationStructure = result.handle();
    build_info.scratchData.deviceAddress = scratch.address();

    const vk::AccelerationStructureBuildRangeInfoKHR range_info{
      .primitiveCount = primitive_count,
    };
    const vk::AccelerationStructureBuildRangeInfoKHR* p_range_info =
      &range_info;

    cmdb->buildAccelerationStructuresKHR(build_info, p_range_info);

    cmdb.keep_alive(std::move(scratch));

    const vk::MemoryBarrier2 barrier{
      .srcStageMask =
        vk::PipelineStageFlagBits2::eAccelerationStructureBuildKHR,
      .srcAccessMask = vk::AccessFlagBits2::eAccelerationStructureWriteKHR,
      .dstStageMask =
        vk::PipelineStageFlagBits2::eAccelerationStructureBuildKHR,
      .dstAccessMask = vk::AccessFlagBits2::eAccelerationStructureReadKHR,
    };
    cmdb->pipelineBarrier2(
      vk::DependencyInfo{
        .memoryBarrierCount = 1,
        .pMemoryBarriers = &barrier,
      }
    );

    result.address_ = context::device()->getAccelerationStructureAddressKHR({
      .accelerationStructure = result.handle(),
    });

    logging::trace("Built BLAS for '{}'", id);

    return result;
  }

}

#endif // VK_KHR_acceleration_structure
