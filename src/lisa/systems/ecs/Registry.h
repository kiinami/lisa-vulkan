//
// Created by kinami on 4/6/26.
//

#ifndef LISA_VULKAN_ECS_REGISTRY_H
#define LISA_VULKAN_ECS_REGISTRY_H
#include "utils/common.h"

#include <entt/entt.hpp>

namespace lisa::systems::ecs {
  using Entity = entt::entity;

  class Registry {
  public:
    Registry() = default;
    ~Registry() = default;

    entt::registry* operator->() { return &registry_; }

    Entity create() { return registry_.create(); }

  private:
    entt::registry registry_;
  };
}

#endif // LISA_VULKAN_REGISTRY_H
