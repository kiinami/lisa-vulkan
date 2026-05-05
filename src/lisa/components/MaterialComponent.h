//
// Created by kinami on 4/14/26.
//

#ifndef LISA_VULKAN_MATERIALCOMPONENT_H
#define LISA_VULKAN_MATERIALCOMPONENT_H
#include "graphics/descriptors/DescriptorAllocator.h"
#include "resources/Texture.h"
#include "resources/context.h"
#include "systems/ecs/Component.h"
#include "utils/logging.h"

namespace lisa::components {

  struct MaterialComponent : systems::ecs::Component {
    MaterialComponent(
      const rgb& albedo,
      const float roughness,
      const float metallic,
      const optional<str>& albedo_texture_id = nullopt,
      const optional<str>& normal_texture_id = nullopt,
      const optional<str>& roughness_texture_id = nullopt,
      const optional<str>& metallic_texture_id = nullopt
    ) :
      albedo(albedo),
      roughness(roughness),
      metallic(metallic),
      albedo_texture_id_(albedo_texture_id),
      normal_texture_id_(normal_texture_id),
      roughness_texture_id_(roughness_texture_id),
      metallic_texture_id_(metallic_texture_id) {}

    rgb albedo = rgb(1.0f);
    float roughness = 0.5f;
    float metallic = 0.0f;

    resources::Texture* albedo_texture() const {
      if (!albedo_texture_id_) return nullptr;
      auto* tex = resources::context::manager().get<resources::Texture>(
        *albedo_texture_id_
      );
      if (!tex)
        logging::error(
          "Albedo texture with token '{}' not found",
          albedo_texture_id_->c_str()
        );
      return tex;
    }

    resources::Texture* normal_texture() const {
      if (!normal_texture_id_) return nullptr;
      auto* tex = resources::context::manager().get<resources::Texture>(
        *normal_texture_id_
      );
      if (!tex)
        logging::error(
          "Normal texture with token '{}' not found",
          normal_texture_id_->c_str()
        );
      return tex;
    }

    resources::Texture* roughness_texture() const {
      if (!roughness_texture_id_) return nullptr;
      auto* tex = resources::context::manager().get<resources::Texture>(
        *roughness_texture_id_
      );
      if (!tex)
        logging::error(
          "Roughness texture with token '{}' not found",
          roughness_texture_id_->c_str()
        );
      return tex;
    }

    resources::Texture* metallic_texture() const {
      if (!metallic_texture_id_) return nullptr;
      auto* tex = resources::context::manager().get<resources::Texture>(
        *metallic_texture_id_
      );
      if (!tex)
        logging::error(
          "Metallic texture with token '{}' not found",
          metallic_texture_id_->c_str()
        );
      return tex;
    }

  private:
    const optional<str> albedo_texture_id_;
    const optional<str> normal_texture_id_;
    const optional<str> roughness_texture_id_;
    const optional<str> metallic_texture_id_;
  };

}

#endif // LISA_VULKAN_MATERIALCOMPONENT_H
