//
// Created by kinami on 4/14/26.
//

#include "MaterialComponent.h"

#include "entt/entity/registry.hpp"
#include "entt/meta/factory.hpp"
#include "systems/ecs/meta.h"

namespace lisa::components { namespace {
  const bool registered = [] {
    using namespace entt::literals;

    systems::ecs::reflect_component<MaterialComponent>("material"_hs)
      .data<&MaterialComponent::color>("color"_hs)
      .data<&MaterialComponent::roughness>("roughness"_hs)
      .data<&MaterialComponent::metallic>("metallic"_hs);

    return true;
  }();
}}
