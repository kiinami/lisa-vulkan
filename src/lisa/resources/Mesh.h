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
    Mesh(
      const str& id,
      const vector<Vertex>& vertices,
      const vector<uint32>& indices,
      const graphics::CommandBuffer& cmdb
    );

    void unload() override {}

    const graphics::Buffer& vertex_buffer() const { return vertex_buffer_; }

    const graphics::Buffer& index_buffer() const { return index_buffer_; }

    uint32 index_count() const { return index_count_; }

  private:
    graphics::Buffer vertex_buffer_;
    graphics::Buffer index_buffer_;
    uint32 vertex_count_ = 0;
    uint32 index_count_ = 0;
  };

  struct MeshSpec : systems::resources::ResourceSpec<Mesh> {
    vector<Vertex> vertices;
    vector<uint32> indices;
    uint32 vertex_count = 0;
    uint32 index_count = 0;

    explicit MeshSpec(const str& id, const path& filepath);

    MeshSpec(
      const str& id, const fastgltf::Primitive& p, const fastgltf::Asset& asset
    );

    Mesh load_resource(const graphics::CommandBuffer& cmdb) override;

  private:
    void load_obj(const path& filepath);

    void calculate_tangents();
  };

}

#endif // LISA_VULKAN_MESH_H
