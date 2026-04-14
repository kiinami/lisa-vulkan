//
// Created by kinami on 4/14/26.
//

#ifndef LISA_VULKAN_CAMERACOMPONENT_H
#define LISA_VULKAN_CAMERACOMPONENT_H
#pragma once
#include "systems/ecs/Component.h"
#include "utils/common.h"

namespace lisa::components {

  struct CameraComponent : systems::ecs::Component {
    float fov = glm::radians(45.0f);
    float aspect_ratio = 16.0f / 9.0f;
    float near_plane = 0.1f;
    float far_plane = 100.0f;

    mat4 projection_matrix() const;

  private:
    mutable mat4 matrix_;
    mutable bool dirty_ = true;
  };

}

#endif // LISA_VULKAN_CAMERACOMPONENT_H
