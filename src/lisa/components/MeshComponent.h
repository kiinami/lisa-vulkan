//
// Created by kinami on 4/14/26.
//

#ifndef LISA_VULKAN_MESHCOMPONENT_H
#define LISA_VULKAN_MESHCOMPONENT_H
#pragma once
#include "resources/Mesh.h"
#include "systems/ecs/ResourceComponent.h"

namespace lisa::components {

  struct MeshComponent : systems::ecs::ResourceComponent<resources::Mesh> {};

}

#endif // LISA_VULKAN_MESHCOMPONENT_H
