//
// Created by kinami on 4/14/26.
//

#ifndef LISA_VULKAN_DIRECTIONALLIGHTCOMPONENT_H
#define LISA_VULKAN_DIRECTIONALLIGHTCOMPONENT_H
#include "systems/ecs/Component.h"
#include "utils/common.h"

namespace lisa::components {

  struct DirectionalLightComponent : systems::ecs::Component {
    rgb color;
    float intensity;
  };

}

#endif // LISA_VULKAN_DIRECTIONALLIGHTCOMPONENT_H
