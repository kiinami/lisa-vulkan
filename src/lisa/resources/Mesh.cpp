//
// Created by kinami on 4/2/26.
//

#include "Mesh.h"

#include "Vertex.h"
#include "graphics/context.h"
#include "tiny_obj_loader.h"
#include "utils/chk.h"

#include <cstring>
#include <vulkan/vulkan_raii.hpp>

namespace lisa::resources {
  bool Mesh::load_function() {
    tinyobj::attrib_t attrib;
    vector<tinyobj::shape_t> shapes;
    vector<tinyobj::material_t> materials;

    utils::chk(
      tinyobj::LoadObj(
        &attrib, &shapes, &materials, nullptr, nullptr, path_.c_str()
      )
    );

    vertex_count_ = shapes[0].mesh.indices.size();

    vector<Vertex> vertices{};
    vector<uint16_t> indices{};
    for (const auto& [vertex_index, normal_index, texcoord_index] :
         shapes[0].mesh.indices) {
      Vertex v{
        .pos =
          {attrib.vertices[vertex_index * 3],
           attrib.vertices[vertex_index * 3 + 1],
           attrib.vertices[vertex_index * 3 + 2]},
        .normal =
          {attrib.normals[normal_index * 3],
           attrib.normals[normal_index * 3 + 1],
           attrib.normals[normal_index * 3 + 2]},
        .uv = {
          attrib.texcoords[texcoord_index * 2],
          attrib.texcoords[texcoord_index * 2 + 1]
        }
      };
      vertices.push_back(v);
      indices.push_back(indices.size());
    }

    vertex_count_ = vertices.size();
    index_count_ = indices.size();

    const auto v_size = sizeof(Vertex) * vertices.size();
    const auto i_size = sizeof(uint16_t) * indices.size();
    const auto size = v_size + i_size;

    auto transfer_buffer = graphics::Buffer(
      size,
      vk::BufferUsageFlagBits::eTransferSrc,
      {.flags = vma::AllocationCreateFlagBits::eMapped |
                vma::AllocationCreateFlagBits::eHostAccessSequentialWrite,
       .usage = vma::MemoryUsage::eAuto}
    );
    std::memcpy(transfer_buffer.mapped_data(), vertices.data(), v_size);
    std::memcpy(
      static_cast<char*>(transfer_buffer.mapped_data()) + v_size,
      indices.data(),
      i_size
    );

    vertex_buffer_ = graphics::Buffer(
      size,
      vk::BufferUsageFlagBits::eVertexBuffer |
        vk::BufferUsageFlagBits::eIndexBuffer |
        vk::BufferUsageFlagBits::eTransferDst,
      {.usage = vma::MemoryUsage::eAuto}
    );

    auto cmd_buffer = graphics::context::device().cmd_buffer();
    cmd_buffer.begin_onetime();
    const vk::BufferCopy copy_region{.size = size};
    cmd_buffer->copyBuffer(transfer_buffer, vertex_buffer_, copy_region);
    cmd_buffer->end();
    graphics::context::device().submit_cmd_buffer_with_fence(cmd_buffer);

    return true;
  }

  bool Mesh::unload_function() { return true; }
}
