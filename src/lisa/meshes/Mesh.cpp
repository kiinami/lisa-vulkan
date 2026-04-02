//
// Created by kinami on 4/2/26.
//

#include "Mesh.h"

#include "Vertex.h"
#include "tiny_obj_loader.h"
#include "utils/chk.h"
#include "utils/common.h"

namespace lisa::meshes {
  Mesh::Mesh(const std::filesystem::path& filename) {
    if (filename.extension() == ".obj") load_obj(filename);
  }

  void Mesh::load_obj(const std::filesystem::path& filename) {
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;

    utils::chk(
      tinyobj::LoadObj(
        &attrib, &shapes, &materials, nullptr, nullptr, filename.c_str()
      )
    );

    count_ = shapes[0].mesh.indices.size();

    std::vector<Vertex> vertices{};
    std::vector<uint16_t> indices{};
    for (const auto& [vertex_index, normal_index, texcoord_index] :
         shapes[0].mesh.indices) {
      Vertex v{ .pos = { attrib.vertices[vertex_index * 3],
                         -attrib.vertices[vertex_index * 3 + 1],
                         attrib.vertices[vertex_index * 3 + 2] },
                .normal = { attrib.normals[normal_index * 3],
                            -attrib.normals[normal_index * 3 + 1],
                            attrib.normals[normal_index * 3 + 2] },
                .uv = { attrib.texcoords[texcoord_index * 2],
                        1.0 - attrib.texcoords[texcoord_index * 2 + 1] } };
      vertices.push_back(v);
      indices.push_back(indices.size());
    }

    const auto v_size = sizeof(Vertex) * vertices.size();
    const auto i_size = sizeof(uint16_t) * indices.size();
    const auto size = v_size + i_size;

    vertex_buffer_ = graphics::Buffer(
      size,
      vk::BufferUsageFlagBits::eVertexBuffer |
        vk::BufferUsageFlagBits::eIndexBuffer
    );

    vertex_buffer_.allocation().copyFromMemory(vertices.data(), 0, v_size);
    vertex_buffer_.allocation().copyFromMemory(indices.data(), v_size, i_size);
  }
}
