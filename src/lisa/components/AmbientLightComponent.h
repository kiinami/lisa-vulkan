//
// Created by kinami on 4/24/26.
//

#ifndef LISA_VULKAN_AMBIENTLIGHTCOMPONENT_H
#define LISA_VULKAN_AMBIENTLIGHTCOMPONENT_H
#include "systems/ecs/Component.h"
#include "utils/common.h"

namespace lisa::components {

  struct AmbientLightComponent : systems::ecs::Component {
    rgb color;
    float intensity;
  };

}

#endif //LISA_VULKAN_AMBIENTLIGHTCOMPONENT_H
