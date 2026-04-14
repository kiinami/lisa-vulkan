//
// Created by kinami on 4/14/26.
//

#ifndef LISA_VULKAN_POINTLIGHTCOMPONENT_H
#define LISA_VULKAN_POINTLIGHTCOMPONENT_H
#include "systems/ecs/Component.h"
#include "utils/common.h"

namespace lisa::components {

  struct PointLightComponent : systems::ecs::Component {
    rgb color;
    float intensity;
    float attenuation;
    float radius;
  };

}

#endif // LISA_VULKAN_POINTLIGHTCOMPONENT_H
