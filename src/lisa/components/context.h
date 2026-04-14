//
// Created by kinami on 4/6/26.
//

#ifndef LISA_VULKAN_COMPONENTS_CONTEXT_H
#define LISA_VULKAN_COMPONENTS_CONTEXT_H
#include "systems/ecs/Registry.h"

namespace lisa::components::context {
  void init();
  void destroy();

  systems::ecs::Registry& registry();
}

#endif
