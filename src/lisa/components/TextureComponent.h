//
// Created by kinami on 4/14/26.
//

#ifndef LISA_VULKAN_TEXTURECOMPONENT_H
#define LISA_VULKAN_TEXTURECOMPONENT_H
#pragma once
#include "resources/Texture.h"
#include "systems/ecs/ResourceComponent.h"
#include "systems/resources/ResourceHandle.h"

namespace lisa::components {

  struct TextureComponent : systems::ecs::Component {
    path diffuse;
    path roughness = "";
    path metallic = "";
    path normal = "";

    resources::Texture* diffuse_resource() const {
      if (!diffuse_handle_)
        diffuse_handle_ =
          resources::context::manager().load<resources::Texture>(diffuse);
      return diffuse_handle_.get();
    }

    resources::Texture* roughness_resource() const {
      if (roughness.empty()) return nullptr;
      if (!roughness_handle_)
        roughness_handle_ =
          resources::context::manager().load<resources::Texture>(roughness);
      return roughness_handle_.get();
    }

    resources::Texture* metallic_resource() const {
      if (metallic.empty()) return nullptr;
      if (!metallic_handle_)
        metallic_handle_ =
          resources::context::manager().load<resources::Texture>(metallic);
      return metallic_handle_.get();
    }

    resources::Texture* normal_resource() const {
      if (normal.empty()) return nullptr;
      if (!normal_handle_)
        normal_handle_ =
          resources::context::manager().load<resources::Texture>(normal);
      return normal_handle_.get();
    }

  private:
    mutable systems::resources::ResourceHandle<resources::Texture>
      diffuse_handle_;
    mutable systems::resources::ResourceHandle<resources::Texture>
      roughness_handle_;
    mutable systems::resources::ResourceHandle<resources::Texture>
      metallic_handle_;
    mutable systems::resources::ResourceHandle<resources::Texture>
      normal_handle_;
  };

}

#endif // LISA_VULKAN_TEXTURECOMPONENT_H
