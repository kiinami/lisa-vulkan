//
// Created by kinami on 6/18/26.
//

#include "AccelerationStructure.h"

#ifdef VK_KHR_acceleration_structure

  #include "graphics/context.h"
  #include "resources/Vertex.h"
  #include "utils/logging.h"

namespace lisa::graphics {

  namespace {
    uint64 scratch_alignment() {
      static const uint64 align = context::physical_device()
        .handle()
        .getProperties2<
          vk::PhysicalDeviceProperties2,
          vk::PhysicalDeviceAccelerationStructurePropertiesKHR>()
        .get<vk::PhysicalDeviceAccelerationStructurePropertiesKHR>()
        .minAccelerationStructureScratchOffsetAlignment;
      return align;
    }
  }

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

    const uint64 align = scratch_alignment();
    auto scratch = Buffer{
      logging::genid(id, "blas_scratch"),
      sizes.buildScratchSize + align - 1,
      vk::BufferUsageFlagBits::eStorageBuffer |
        vk::BufferUsageFlagBits::eShaderDeviceAddress,
      {.usage = vma::MemoryUsage::eAuto},
    };

    build_info.dstAccelerationStructure = result.handle();
    build_info.scratchData.deviceAddress =
      (scratch.address() + align - 1) & ~(align - 1);

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

  AccelerationStructure AccelerationStructure::build_tlas(
    const str& id,
    const vector<TlasInstance>& instances,
    const CommandBuffer& cmdb
  ) {
    const uint32 instance_count = static_cast<uint32>(instances.size());

    vector<vk::AccelerationStructureInstanceKHR> vk_instances;
    vk_instances.reserve(instance_count);
    for (const auto& inst : instances) {
      vk::TransformMatrixKHR t{};
      for (int r = 0; r < 3; r++)
        for (int c = 0; c < 4; c++)
          t.matrix[r][c] = inst.transform[c][r];

      vk_instances.push_back({
        .transform = t,
        .instanceCustomIndex = inst.instance_index,
        .mask = 0xFF,
        .instanceShaderBindingTableRecordOffset = 0,
        .flags = static_cast<VkGeometryInstanceFlagsKHR>(
          vk::GeometryInstanceFlagBitsKHR::eTriangleFacingCullDisable),
        .accelerationStructureReference = inst.blas_address,
      });
    }

    auto instances_buf = Buffer{
      logging::genid(id, "tlas_instances"),
      sizeof(vk::AccelerationStructureInstanceKHR) * instance_count,
      vk::BufferUsageFlagBits::eAccelerationStructureBuildInputReadOnlyKHR |
        vk::BufferUsageFlagBits::eShaderDeviceAddress,
      {
        .flags = vma::AllocationCreateFlagBits::eHostAccessSequentialWrite |
                 vma::AllocationCreateFlagBits::eMapped,
        .usage = vma::MemoryUsage::eAuto,
      },
    };
    std::memcpy(
      instances_buf.mapped_data(),
      vk_instances.data(),
      sizeof(vk::AccelerationStructureInstanceKHR) * instance_count
    );

    const vk::AccelerationStructureGeometryInstancesDataKHR inst_data{
      .data = {.deviceAddress = instances_buf.address()},
    };

    const vk::AccelerationStructureGeometryKHR geometry{
      .geometryType = vk::GeometryTypeKHR::eInstances,
      .geometry = {.instances = inst_data},
    };

    vk::AccelerationStructureBuildGeometryInfoKHR build_info{
      .type = vk::AccelerationStructureTypeKHR::eTopLevel,
      .flags = vk::BuildAccelerationStructureFlagBitsKHR::ePreferFastTrace,
      .mode = vk::BuildAccelerationStructureModeKHR::eBuild,
      .geometryCount = 1,
      .pGeometries = &geometry,
    };

    const auto sizes = context::device()->getAccelerationStructureBuildSizesKHR(
      vk::AccelerationStructureBuildTypeKHR::eDevice,
      build_info,
      {instance_count}
    );

    AccelerationStructure result(id);

    result.buffer_ = Buffer{
      logging::genid(id, "tlas"),
      sizes.accelerationStructureSize,
      vk::BufferUsageFlagBits::eAccelerationStructureStorageKHR |
        vk::BufferUsageFlagBits::eShaderDeviceAddress,
      {.usage = vma::MemoryUsage::eAuto},
    };

    result.set(context::device()->createAccelerationStructureKHR({
      .buffer = result.buffer_.handle(),
      .size = sizes.accelerationStructureSize,
      .type = vk::AccelerationStructureTypeKHR::eTopLevel,
    }));

    const uint64 align = scratch_alignment();
    auto scratch = Buffer{
      logging::genid(id, "tlas_scratch"),
      sizes.buildScratchSize + align - 1,
      vk::BufferUsageFlagBits::eStorageBuffer |
        vk::BufferUsageFlagBits::eShaderDeviceAddress,
      {.usage = vma::MemoryUsage::eAuto},
    };

    build_info.dstAccelerationStructure = result.handle();
    build_info.scratchData.deviceAddress =
      (scratch.address() + align - 1) & ~(align - 1);

    const vk::AccelerationStructureBuildRangeInfoKHR range_info{
      .primitiveCount = instance_count,
    };
    const vk::AccelerationStructureBuildRangeInfoKHR* p_range_info = &range_info;

    cmdb->buildAccelerationStructuresKHR(build_info, p_range_info);

    cmdb.keep_alive(std::move(instances_buf));
    cmdb.keep_alive(std::move(scratch));

    const vk::MemoryBarrier2 barrier{
      .srcStageMask =
        vk::PipelineStageFlagBits2::eAccelerationStructureBuildKHR,
      .srcAccessMask = vk::AccessFlagBits2::eAccelerationStructureWriteKHR,
      .dstStageMask = vk::PipelineStageFlagBits2::eFragmentShader,
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

    logging::trace("Built TLAS for '{}'", id);

    return result;
  }

}

#endif // VK_KHR_acceleration_structure
