//
// Created by kinami on 4/14/26.
//

#include "PointLightComponent.h"

#include "entt/entity/registry.hpp"
#include "entt/meta/factory.hpp"
#include "systems/ecs/meta.h"

namespace lisa::components { namespace {
  const bool registered = [] {
    using namespace entt::literals;

    systems::ecs::reflect_component<PointLightComponent>("point_light"_hs)
      .data<&PointLightComponent::color>("color"_hs)
      .data<&PointLightComponent::intensity>("intensity"_hs)
      .data<&PointLightComponent::attenuation>("attenuation"_hs)
      .data<&PointLightComponent::radius>("radius"_hs);

    return true;
  }();
}}
