//
// Created by kinami on 4/5/26.
//

#ifndef LISA_VULKAN_RESOURCES_CONTEXT_H
#define LISA_VULKAN_RESOURCES_CONTEXT_H
#include "systems/resources/ResourceManager.h"

namespace lisa::resources::context {
  void init();
  void destroy();
  systems::resources::ResourceManager& manager();
}

#endif // LISA_VULKAN_RESOURCES_CONTEXT_H
