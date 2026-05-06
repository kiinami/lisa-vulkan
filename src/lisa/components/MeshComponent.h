//
// Created by kinami on 4/14/26.
//

#ifndef LISA_VULKAN_MESHCOMPONENT_H
#define LISA_VULKAN_MESHCOMPONENT_H
#pragma once
#include "resources/Mesh.h"
#include "resources/context.h"

namespace lisa::components {

  struct MeshComponent : systems::ecs::Component {
    explicit MeshComponent(const str& id) : id_(id) {}

    resources::Mesh* resource() const {
      auto* mesh = resources::context::manager().get<resources::Mesh>(id_);
      if (!mesh) {
        const auto error = "Mesh with token '" + id_ + "' not found";
        throw std::runtime_error(error);
      }
      return mesh;
    }

  private:
    const str id_;
  };

}

#endif // LISA_VULKAN_MESHCOMPONENT_H
