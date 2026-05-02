//
// Created by kinami on 4/2/26.
//

#include "Mesh.h"

#include "Vertex.h"
#include "graphics/context.h"
#include "utils/chk.h"
#include "utils/path.h"

#include <cstring>
#include <tiny_obj_loader.h>

namespace lisa::resources {
  Mesh::Mesh(const path& filepath) {
    const auto ext = filepath.extension().string();

    if (ext == ".obj") {
      load_obj(filepath);
    } else {
      logging::abort(
        "Mesh format of file '{}' not supported", filepath.c_str()
      );
    }
  }

  void Mesh::load_obj(const path& filepath) {
    tinyobj::attrib_t attrib;
    vector<tinyobj::shape_t> shapes;
    vector<tinyobj::material_t> materials;
    str warn, error;

    const auto fp = utils::pstr(filepath);
    auto result =
      tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &error, fp.c_str());

    if (!warn.empty()) logging::warning("{}", warn);
    if (!error.empty()) logging::error("{}", error);
    if (!result || shapes.empty())
      throw std::runtime_error("Failed to load OBJ: " + fp);

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

    for (size i = 0; i < vertices.size(); i += 3) {
      auto& v0 = vertices[i].pos;
      auto& v1 = vertices[i + 1].pos;
      auto& v2 = vertices[i + 2].pos;

      auto& uv0 = vertices[i].uv;
      auto& uv1 = vertices[i + 1].uv;
      auto& uv2 = vertices[i + 2].uv;

      auto edge1 = v1 - v0;
      auto edge2 = v2 - v0;
      auto deltaUV1 = uv1 - uv0;
      auto deltaUV2 = uv2 - uv0;

      auto f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);
      auto tangent =
        glm::normalize(f * (deltaUV2.y * edge1 - deltaUV1.y * edge2));

      vertices[i].tangent = tangent;
      vertices[i + 1].tangent = tangent;
      vertices[i + 2].tangent = tangent;
    }

    vertex_count_ = vertices.size();
    index_count_ = indices.size();

    const auto v_size = sizeof(Vertex) * vertices.size();
    const auto i_size = sizeof(uint16_t) * indices.size();
    const auto size = v_size + i_size;

    vector<uint8> buffer_data(size);
    std::memcpy(buffer_data.data(), vertices.data(), v_size);
    std::memcpy(buffer_data.data() + v_size, indices.data(), i_size);

    vertex_buffer_ = graphics::Buffer::from_data(
      buffer_data.data(),
      size,
      vk::BufferUsageFlagBits::eVertexBuffer |
        vk::BufferUsageFlagBits::eIndexBuffer |
        vk::BufferUsageFlagBits::eTransferDst,
      {.usage = vma::MemoryUsage::eAuto}
    );
  }
}
