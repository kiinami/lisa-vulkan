//
// Created by kinami on 4/2/26.
//

#ifndef LISA_VULKAN_MESH_H
#define LISA_VULKAN_MESH_H
#include "graphics/buffer/Buffer.h"
#include "systems/resources/Resource.h"
#include "utils/common.h"

namespace lisa::resources {

  class Mesh : public systems::resources::Resource {
  public:
    explicit Mesh(const str& id) : Resource(id) {}

  protected:
    bool load_function() override;
    bool unload_function() override;

    str type_name() override { return "Mesh"; }

  private:
    graphics::Buffer vertex_buffer_;
    uint32 vertex_count_ = 0;
  };

}

#endif // LISA_VULKAN_MESH_H
