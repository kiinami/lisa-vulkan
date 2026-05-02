//
// Created by kinami on 4/14/26.
//

#ifndef LISA_VULKAN_MESHCOMPONENT_H
#define LISA_VULKAN_MESHCOMPONENT_H
#pragma once
#include "resources/Mesh.h"

namespace lisa::components {

  struct MeshComponent : systems::ecs::Component {
    const resources::Mesh* mesh = nullptr;
  };

}

#endif // LISA_VULKAN_MESHCOMPONENT_H
