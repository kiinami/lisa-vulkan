//
// Created by kinami on 4/14/26.
//

#ifndef LISA_VULKAN_POINTLIGHTCOMPONENT_H
#define LISA_VULKAN_POINTLIGHTCOMPONENT_H
#include "TransformComponent.h"
#include "systems/ecs/Component.h"
#include "utils/common.h"

namespace lisa::components {

  struct PointLightComponent : systems::ecs::Component {
    rgb color;
    float intensity;
    float attenuation;
    float radius;
    float source_radius = 0.0f;
    bool cast_shadows = false;

    const array<mat4, 6>& views(const TransformComponent& transform) const {
      if (views_dirty_ || transform.position() != last_position_) {
        const vec3 pos = transform.position();
        last_position_ = pos;
        views_dirty_ = false;
        cached_views_ = {
          lookAt(pos, pos + vec3{1.0f, 0.0f, 0.0f}, vec3{0.0f, -1.0f, 0.0f}),
          lookAt(pos, pos + vec3{-1.0f, 0.0f, 0.0f}, vec3{0.0f, -1.0f, 0.0f}),
          lookAt(pos, pos + vec3{0.0f, 1.0f, 0.0f}, vec3{0.0f, 0.0f, 1.0f}),
          lookAt(pos, pos + vec3{0.0f, -1.0f, 0.0f}, vec3{0.0f, 0.0f, -1.0f}),
          lookAt(pos, pos + vec3{0.0f, 0.0f, 1.0f}, vec3{0.0f, -1.0f, 0.0f}),
          lookAt(pos, pos + vec3{0.0f, 0.0f, -1.0f}, vec3{0.0f, -1.0f, 0.0f}),
        };
      }
      return cached_views_;
    }

    static float near() { return 0.1f; }

    float far() const { return radius * 3.0f; }

    mat4 projection() const {
      auto proj = glm::perspective(glm::radians(90.0f), 1.0f, near(), far());
      proj[1][1] *= -1.0f;
      return proj;
    }

  private:
    mutable array<mat4, 6> cached_views_;
    mutable vec3 last_position_{std::numeric_limits<float>::quiet_NaN()};
    mutable bool views_dirty_ = true;
  };

}

#endif
