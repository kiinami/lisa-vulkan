//
// Created by kinami on 4/14/26.
//

#ifndef LISA_VULKAN_MATERIALCOMPONENT_H
#define LISA_VULKAN_MATERIALCOMPONENT_H
#include "graphics/descriptors/DescriptorAllocator.h"
#include "resources/Texture.h"
#include "systems/ecs/Component.h"

namespace lisa::components {

  struct MaterialComponent : systems::ecs::Component {
    rgb albedo = rgb(1.0f);
    float roughness = 0.5f;
    float metallic = 0.0f;

    const resources::Texture* albedo_texture = nullptr;
    const resources::Texture* normal_texture = nullptr;
    const resources::Texture* roughness_texture = nullptr;
    const resources::Texture* metallic_texture = nullptr;
  };

}

#endif // LISA_VULKAN_MATERIALCOMPONENT_H
