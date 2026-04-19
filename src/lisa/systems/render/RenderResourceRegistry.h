//
// Created by kinami on 4/17/26.
//

#ifndef LISA_VULKAN_RENDERRESOURCEREGISTRY_H
#define LISA_VULKAN_RENDERRESOURCEREGISTRY_H
#include "RenderResource.h"
#include "utils/Registry.h"

namespace lisa::systems::render {
  class RenderResourceRegistry :
    public utils::Registry<RenderResource, const char*, const vec3&> {};
}

#define REGISTER_RENDER_RESOURCE(TYPE)                     \
  namespace {                                              \
    const bool registered = []() {                         \
      lisa::systems::render::RenderResourceRegistry::add(  \
        TYPE::TYPE_ID,                                     \
        [](const char* id, const vec3& size)               \
          -> uptr<lisa::systems::render::RenderResource> { \
          return std::make_unique<TYPE>(id, size);         \
        }                                                  \
      );                                                   \
      return true;                                         \
    }();                                                   \
  }

#endif // LISA_VULKAN_RENDERRESOURCEREGISTRY_H
