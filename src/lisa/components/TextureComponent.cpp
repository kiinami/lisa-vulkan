//
// Created by kinami on 4/14/26.
//

#include "TextureComponent.h"

#include "entt/entity/registry.hpp"
#include "entt/meta/factory.hpp"
#include "systems/ecs/meta.h"

namespace lisa::components { namespace {
  const bool registered = [] {
    using namespace entt::literals;

    systems::ecs::reflect_component<TextureComponent>("texture"_hs)
      .data<&TextureComponent::filepath>("path"_hs);

    return true;
  }();
}}
