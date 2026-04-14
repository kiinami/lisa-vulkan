//
// Created by kinami on 4/5/26.
//

#ifndef LISA_VULKAN_TRANSFORMCOMPONENT_H
#define LISA_VULKAN_TRANSFORMCOMPONENT_H
#pragma once
#include "systems/ecs/Component.h"
#include "utils/common.h"

#include <pugixml.hpp>

namespace lisa::components {

  struct TransformComponent : systems::ecs::Component {
    void parse(const pugi::xml_node& node);

    vec3 position() const { return position_; }

    quat rotation() const { return rotation_; }

    vec3 scale() const { return scale_; }

    void set_position(const vec3& position) {
      position_ = position;
      dirty_ = true;
    }

    void set_rotation(const quat& rotation) {
      rotation_ = rotation;
      dirty_ = true;
    }

    void set_scale(const vec3& scale) {
      scale_ = scale;
      dirty_ = true;
    }

    mat4 matrix() const;

  private:
    vec3 position_;
    quat rotation_;
    vec3 scale_;

    mutable mat4 matrix_{1.0f};
    mutable bool dirty_ = true;
  };
}

#endif
