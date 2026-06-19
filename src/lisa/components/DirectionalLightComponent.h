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
    float angular_radius = 0.0f;
    bool cast_shadows = false;

    mat4 shadow_view_projection(const vec3& direction) const {
      const mat4 light_view =
        glm::lookAt(vec3(0.0f), -direction, vec3(0.0f, 1.0f, 0.0f));
      mat4 light_projection =
        glm::ortho(-20.0f, 20.0f, -20.0f, 20.0f, 1.0f, 100.0f);
      light_projection[1][1] *= -1.0f;
      return light_projection * light_view;
    }
  };

}

#endif // LISA_VULKAN_DIRECTIONALLIGHTCOMPONENT_H
