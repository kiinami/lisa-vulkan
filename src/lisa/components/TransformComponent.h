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
    mat4 matrix{1.0f};

    void parse(const pugi::xml_node& node);
  };
}

#endif
