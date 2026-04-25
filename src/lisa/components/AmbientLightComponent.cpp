//
// Created by kinami on 4/24/26.
//

#include "AmbientLightComponent.h"

#include "systems/ecs/meta.h"

#include <entt/entity/registry.hpp>
#include <entt/meta/factory.hpp>

namespace lisa::components { namespace {
  const bool registered = [] {
    using namespace entt::literals;

    systems::ecs::reflect_component<AmbientLightComponent>("ambient_light"_hs)
      .data<&AmbientLightComponent::color>("color"_hs)
      .data<&AmbientLightComponent::intensity>("intensity"_hs);

    return true;
  }();
}}
