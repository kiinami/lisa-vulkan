//
// Created by kinami on 4/2/26.
//

#include "Mesh.h"

#include "Vertex.h"
#include "graphics/context.h"
#include "utils/chk.h"
#include "utils/path.h"

#include <fastgltf/glm_element_traits.hpp>
#include <numeric>
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

  Mesh::Mesh(const fastgltf::Primitive& p, const fastgltf::Asset& asset) {
    auto* pos_attr = p.findAttribute("POSITION");
    auto* norm_attr = p.findAttribute("NORMAL");
    auto* uv_attr = p.findAttribute("TEXCOORD_0");

    if (!pos_attr)
      throw std::runtime_error("Primitive missing POSITION attribute");

    auto& pos_accessor = asset.accessors[pos_attr->accessorIndex];
    auto& norm_accessor = asset.accessors[norm_attr->accessorIndex];
    auto& uv_accessor = asset.accessors[uv_attr->accessorIndex];

    vector<Vertex> vertices(pos_accessor.count);

    fastgltf::iterateAccessorWithIndex<vec3>(
      asset, pos_accessor, [&](const vec3 pos, const size i) {
        vertices[i].pos = pos;
      }
    );
    fastgltf::iterateAccessorWithIndex<vec3>(
      asset, norm_accessor, [&](const vec3 n, const size i) {
        vertices[i].normal = n;
      }
    );
    fastgltf::iterateAccessorWithIndex<vec2>(
      asset, uv_accessor, [&](const vec2 uv, const size i) {
        vertices[i].uv = uv;
      }
    );

    vector<uint16> indices;
    if (p.indicesAccessor.has_value()) {
      auto& idx_accessor = asset.accessors[p.indicesAccessor.value()];
      indices.resize(idx_accessor.count);
      fastgltf::iterateAccessorWithIndex<uint16>(
        asset, idx_accessor, [&](const uint16 idx, const size i) {
          indices[i] = idx;
        }
      );
    } else {
      indices.resize(pos_accessor.count);
      std::iota(indices.begin(), indices.end(), 0);
    }

    calculate_tangents(vertices, indices);
    load(vertices, indices);
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

    vector<Vertex> vertices;
    vector<uint16> indices;
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

    calculate_tangents(vertices, indices);
    load(vertices, indices);
  }

  void Mesh::calculate_tangents(
    vector<Vertex>& vertices, const vector<uint16>& indices
  ) {
    for (size i = 0; i < indices.size(); i += 3) {
      const uint16 i0 = indices[i];
      const uint16 i1 = indices[i + 1];
      const uint16 i2 = indices[i + 2];

      auto& v0 = vertices[i0].pos;
      auto& v1 = vertices[i1].pos;
      auto& v2 = vertices[i2].pos;

      auto& uv0 = vertices[i0].uv;
      auto& uv1 = vertices[i1].uv;
      auto& uv2 = vertices[i2].uv;

      auto edge1 = v1 - v0;
      auto edge2 = v2 - v0;
      const auto deltaUV1 = uv1 - uv0;
      const auto deltaUV2 = uv2 - uv0;

      const float denom = (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);
      const auto f = (std::abs(denom) < 1e-6f) ? 1.0f : 1.0f / denom;

      const auto tangent =
        glm::normalize(f * (deltaUV2.y * edge1 - deltaUV1.y * edge2));

      vertices[i0].tangent += tangent;
      vertices[i1].tangent += tangent;
      vertices[i2].tangent += tangent;
    }

    for (auto& v : vertices)
      v.tangent = glm::normalize(v.tangent);
  }

  void
    Mesh::load(const vector<Vertex>& vertices, const vector<uint16>& indices) {
    vertex_count_ = vertices.size();
    index_count_ = indices.size();

    vertex_buffer_ = graphics::Buffer::from_data(
      vertices.data(),
      sizeof(Vertex) * vertices.size(),
      vk::BufferUsageFlagBits::eVertexBuffer |
        vk::BufferUsageFlagBits::eTransferDst,
      {.usage = vma::MemoryUsage::eAuto}
    );

    index_buffer_ = graphics::Buffer::from_data(
      indices.data(),
      sizeof(uint16_t) * indices.size(),
      vk::BufferUsageFlagBits::eIndexBuffer |
        vk::BufferUsageFlagBits::eTransferDst,
      {.usage = vma::MemoryUsage::eAuto}
    );
  }
}
