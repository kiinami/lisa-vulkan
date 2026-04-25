//
// Created by kinami on 4/14/26.
//

#include "DirectionalLightComponent.h"

#include <entt/entity/registry.hpp>
#include <entt/meta/factory.hpp>
#include "systems/ecs/meta.h"

namespace lisa::components { namespace {
  const bool registered = [] {
    using namespace entt::literals;

    systems::ecs::reflect_component<DirectionalLightComponent>(
      "directional_light"_hs
    )
      .data<&DirectionalLightComponent::color>("color"_hs)
      .data<&DirectionalLightComponent::intensity>("intensity"_hs);

    return true;
  }();
}}
