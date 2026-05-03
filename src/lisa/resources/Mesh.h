//
// Created by kinami on 4/2/26.
//

#ifndef LISA_VULKAN_MESH_H
#define LISA_VULKAN_MESH_H
#include "Vertex.h"
#include "fastgltf/types.hpp"
#include "graphics/buffer/Buffer.h"
#include "systems/resources/Resource.h"
#include "utils/common.h"

namespace lisa::resources {

  class Mesh : public systems::resources::Resource {
  public:
    explicit Mesh(const path& filepath);

    explicit Mesh(const fastgltf::Primitive& p, const fastgltf::Asset& asset);

    void unload() override {}

    const graphics::Buffer& vertex_buffer() const { return vertex_buffer_; }

    const graphics::Buffer& index_buffer() const { return index_buffer_; }

    uint32 index_offset() const { return vertex_count_ * sizeof(Vertex); }

    uint32 index_count() const { return index_count_; }

  private:
    graphics::Buffer vertex_buffer_;
    graphics::Buffer index_buffer_;
    uint32 vertex_count_ = 0;
    uint32 index_count_ = 0;

    void load_obj(const path& filepath);

    static void calculate_tangents(
      vector<Vertex>& vertices, const vector<uint16>& indices
    );
    void load(const vector<Vertex>& vertices, const vector<uint16>& indices);
  };

}

#endif // LISA_VULKAN_MESH_H
