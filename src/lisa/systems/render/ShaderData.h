//
// Created by kinami on 4/4/26.
//

#ifndef LISA_VULKAN_SHADERDATA_H
#define LISA_VULKAN_SHADERDATA_H

#include "components/AmbientLightComponent.h"
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
    float near;
    float far;
    uint32 shadow_base_index = std::numeric_limits<uint32>::max();

    float source_radius = 0.0f;

    PointLightData(
      const components::TransformComponent& transform,
      const components::PointLightComponent& light
    ) {
      position = transform.position();
      intensity = light.intensity;
      color = light.color;
      radius = light.radius;
      near = light.near();
      far = light.far();
      source_radius = light.source_radius;
    }
  };

  struct alignas(16) DirLightData {
    vec3 direction;
    float intensity;
    rgb color;
    uint32 shadow_index = std::numeric_limits<uint32>::max();
    float angular_radius = 0.0f;
    float padding[3]{};

    DirLightData(
      const components::TransformComponent& transform,
      const components::DirectionalLightComponent& light
    ) {
      direction = transform.rotation() * vec3(0.0f, 0.0f, -1.0f);
      intensity = light.intensity;
      color = light.color;
      angular_radius = light.angular_radius;
    }
  };

  struct alignas(16) AmbientLightData {
    float intensity;
    rgb color;

    explicit AmbientLightData(const components::AmbientLightComponent& light) {
      intensity = light.intensity;
      color = light.color;
    }
  };

  struct alignas(16) ShadowData {
    mat4 view_projection;
    uint32 layer;

    uint32 padding[3];
  };

  struct alignas(16) GlobalData {
    mat4 view_projection;
    mat4 projection;
    mat4 view;

    vk::DeviceAddress point_lights_bda;
    vk::DeviceAddress dir_lights_bda;
    vk::DeviceAddress ambient_lights_bda;
    vk::DeviceAddress shadow_data_bda;

    uint32 point_lights_count;
    uint32 dir_lights_count;
    uint32 ambient_lights_count;
    uint32 shadow_count;

    vec3 camera_position;
    vec2 texel_size;

    uint32 padding[2];

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

    void
      update_ambient_lights(const vk::DeviceAddress bda, const uint32 count) {
      ambient_lights_bda = bda;
      ambient_lights_count = count;
    }

    void update_shadow_data(const vk::DeviceAddress bda, const uint32 count) {
      shadow_data_bda = bda;
      shadow_count = count;
    }
  };

  struct alignas(16) ObjectData {
    mat4 model;
    vec4 color = vec4(1.0f);
    float roughness = 1.0f;
    float metallic = 0.0f;
    uint32 diffuse_texture_index = std::numeric_limits<uint32>::max();
    uint32 roughness_texture_index = std::numeric_limits<uint32>::max();
    uint32 metallic_texture_index = std::numeric_limits<uint32>::max();
    uint32 normal_texture_index = std::numeric_limits<uint32>::max();
    uint32 padding[2];
  };

  struct PushConstants {
    uint64 global_bda;
    uint64 object_bda;
  };

}

#endif // LISA_VULKAN_SHADERDATA_H
