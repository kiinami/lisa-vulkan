//
// Created by kinami on 4/14/26.
//

#include "MaterialComponent.h"

#include "entt/entity/registry.hpp"
#include "entt/meta/factory.hpp"
#include "systems/ecs/meta.h"

namespace lisa::components {
  namespace {
    const bool registered = [] {
      using namespace entt::literals;

      systems::ecs::reflect_component<BrdfComponent>("brdf"_hs)
        .data<&BrdfComponent::color>("color"_hs)
        .data<&BrdfComponent::roughness>("roughness"_hs)
        .data<&BrdfComponent::metallic>("metallic"_hs)
        .data<&BrdfComponent::specular>("specular"_hs);

      return true;
    }();
  }
}
