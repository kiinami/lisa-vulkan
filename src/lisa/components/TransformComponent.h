//
// Created by kinami on 4/5/26.
//

#ifndef LISA_VULKAN_TRANSFORMCOMPONENT_H
#define LISA_VULKAN_TRANSFORMCOMPONENT_H
#pragma once
#include "systems/ecs/Component.h"
#include "utils/common.h"

#include <glm/gtx/matrix_decompose.hpp>
#include <pugixml.hpp>

namespace lisa::components {

  struct TransformComponent : systems::ecs::Component {
    void parse(const pugi::xml_node& node, const path& base_path);

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

    void set_matrix(const mat4& matrix) {
      vec3 skew;
      vec4 perspective;
      glm::decompose(matrix, scale_, rotation_, position_, skew, perspective);
      dirty_ = true;
    }

    void translate(const vec3& delta) {
      position_ += delta;
      dirty_ = true;
    }

    void translate_local(const vec3& delta) {
      position_ += rotation_ * delta;
      dirty_ = true;
    }

    void rotate(const quat& delta) {
      rotation_ = glm::normalize(delta * rotation_);
      dirty_ = true;
    }

    void rotate(const vec3& axis, const float angle) {
      rotate(glm::angleAxis(angle, glm::normalize(axis)));
    }

    mat4 matrix() const {
      if (dirty_) {
        matrix_ = glm::translate(mat4(1.0f), position_) *
                  glm::mat4_cast(rotation_) *
                  glm::scale(mat4(1.0f), scale_);
        dirty_ = false;
      }
      return matrix_;
    }

    vec3 direction() const {
      return glm::normalize(rotation_ * vec3(0.0f, 0.0f, -1.0f));
    }

  private:
    vec3 position_;
    quat rotation_;
    vec3 scale_;

    mutable mat4 matrix_{1.0f};
    mutable bool dirty_ = true;
  };
}

#endif
