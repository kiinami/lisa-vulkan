//
// Created by kinami on 4/18/26.
//

#ifndef LISA_VULKAN_RENDERPASSREGISTRY_H
#define LISA_VULKAN_RENDERPASSREGISTRY_H

#include "RenderPass.h"
#include "utils/Registry.h"

namespace lisa::systems::render {
  class RenderPassRegistry :
    public ::lisa::utils::Registry<RenderPass, const pugi::xml_node&> {};
}

#define REGISTER_RENDER_PASS(TYPE)                       \
  namespace {                                            \
    const bool registered_ = []() {                      \
      ::lisa::systems::render::RenderPassRegistry::add(  \
        TYPE::TYPE_ID,                                   \
        [](const pugi::xml_node& node)                   \
          -> uptr<::lisa::systems::render::RenderPass> { \
          return std::make_unique<TYPE>(node);           \
        }                                                \
      );                                                 \
      return true;                                       \
    }();                                                 \
  }

#endif // LISA_VULKAN_RENDERPASSREGISTRY_H
