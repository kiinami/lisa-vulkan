//
// Created by kinami on 4/14/26.
//

#ifndef LISA_VULKAN_META_H
#define LISA_VULKAN_META_H

#include "entt/entity/registry.hpp"
#include "entt/meta/factory.hpp"
#include "entt/meta/meta.hpp"

namespace lisa::systems::ecs {
  template<typename T>
  void emplace_helper(entt::registry& registry, entt::entity e, T& comp) {
    registry.emplace_or_replace<T>(e, std::move(comp));
  }

  template<typename T> auto reflect_component(const entt::hashed_string& name) {
    using namespace entt::literals;
    return entt::meta<T>().type(name).template func<&emplace_helper<T>>(
      "emplace"_hs
    );
  }
}

#endif // LISA_VULKAN_META_H
