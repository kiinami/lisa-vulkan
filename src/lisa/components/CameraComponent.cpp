//
// Created by kinami on 4/14/26.
//

#include "CameraComponent.h"

#include "systems/ecs/meta.h"

#include <entt/entity/registry.hpp>
#include <entt/meta/factory.hpp>

namespace lisa::components {
  namespace {
    const bool registered = [] {
      using namespace entt::literals;

      systems::ecs::reflect_component<CameraComponent>("camera"_hs)
        .data<&CameraComponent::fov>("fov"_hs)
        .data<&CameraComponent::aspect_ratio>("aspect_ratio"_hs)
        .data<&CameraComponent::near_plane>("near_plane"_hs)
        .data<&CameraComponent::far_plane>("far_plane"_hs);

      return true;
    }();
  }

  mat4 CameraComponent::projection_matrix() const {
    if (dirty_) {
      matrix_ = glm::perspective(fov, aspect_ratio, near_plane, far_plane);
      matrix_[1][1] *= -1.0f;
    }
    return matrix_;
  }
}
