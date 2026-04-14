//
// Created by kinami on 4/14/26.
//

#ifndef LISA_VULKAN_TEXTURECOMPONENT_H
#define LISA_VULKAN_TEXTURECOMPONENT_H
#pragma once
#include "resources/Texture.h"
#include "systems/ecs/ResourceComponent.h"

namespace lisa::components {

  struct TextureComponent :
    systems::ecs::ResourceComponent<resources::Texture> {};

}

#endif // LISA_VULKAN_TEXTURECOMPONENT_H
