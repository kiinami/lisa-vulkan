//
// Created by kinami on 4/4/26.
//

#ifndef LISA_VULKAN_SHADERDATA_H
#define LISA_VULKAN_SHADERDATA_H

#include "components/CameraComponent.h"
#include "components/DirectionalLightComponent.h"
#include "components/PointLightComponent.h"
#include "components/TransformComponent.h"
#include "utils/common.h"

namespace lisa::systems::render {

  struct alignas(16) PointLightData {
    vec3 position;
    float intensity;
    rgb color;
    float radius;

    PointLightData(
      const components::TransformComponent& transform,
      const components::PointLightComponent& light
    ) {
      position = transform.position();
      intensity = light.intensity;
      color = light.color;
      radius = light.radius;
    }
  };

  struct alignas(16) DirLightData {
    vec3 direction;
    float intensity;
    rgb color;

    DirLightData(
      const components::TransformComponent& transform,
      const components::DirectionalLightComponent& light
    ) {
      direction = transform.rotation() * vec3(0.0f, 0.0f, -1.0f);
      intensity = light.intensity;
      color = light.color;
    }
  };

  struct alignas(16) GlobalData {
    mat4 view_projection;
    mat4 projection;
    mat4 view;

    vk::DeviceAddress point_lights_bda;
    uint32 point_lights_count;

    vk::DeviceAddress dir_lights_bda;
    uint32 dir_lights_count;

    vec3 camera_position;
    float padding;

    void update_camera(
      const components::TransformComponent& transform,
      const components::CameraComponent& camera
    ) {
      view = glm::inverse(transform.matrix());
      projection = camera.projection_matrix();
      view_projection = projection * view;
      camera_position = transform.position();
    }

    void update_point_lights(const vk::DeviceAddress bda, const uint32 count) {
      point_lights_bda = bda;
      point_lights_count = count;
    }

    void update_dir_lights(const vk::DeviceAddress bda, const uint32 count) {
      dir_lights_bda = bda;
      dir_lights_count = count;
    }
  };

  struct alignas(16) ObjectData {
    mat4 model;
    vec4 color;
    uint32 texture_index;
    float roughness;
    float metallic;
    vec2 padding;
  };

  struct PushConstants {
    uint64 global_bda;
    uint64 object_bda;
  };

}

#endif // LISA_VULKAN_SHADERDATA_H
