//
// Created by kinami on 4/14/26.
//

#ifndef LISA_VULKAN_MATERIALCOMPONENT_H
#define LISA_VULKAN_MATERIALCOMPONENT_H
#include "systems/ecs/Component.h"

namespace lisa::components {

  struct BrdfComponent : systems::ecs::Component {
    rgb color;
    float roughness;
    float metallic;
    float specular;
  };

}

#endif //LISA_VULKAN_MATERIALCOMPONENT_H
