//
// Created by kinami on 5/2/26.
//

#ifndef LISA_VULKAN_RESOURCEMANAGER_H
#define LISA_VULKAN_RESOURCEMANAGER_H

#include "Resource.h"
#include "utils/common.h"

#include <typeindex>

namespace lisa::systems::resources {
  template<typename T>
  concept ResourceDerived = std::derived_from<T, Resource>;

  class ResourceManager {
  public:
    ResourceManager() = default;
    ~ResourceManager() = default;

    template<ResourceDerived T, typename... Args>
    T* load(const str& id, Args&&... args) {
      if (auto* existing = get<T>(id); existing)
        return existing;

      auto& bucket = resources_[std::type_index(typeid(T))];
      auto resource = std::make_unique<T>(std::forward<Args>(args)...);

      auto* ptr = resource.get();
      bucket[id] = std::move(resource);
      return ptr;
    }

    template<ResourceDerived T> T* get(const str& id) {
      auto& bucket = resources_[std::type_index(typeid(T))];

      if (const auto it = bucket.find(id); it != bucket.end())
        return static_cast<T*>(it->second.get());
      return nullptr;
    }

  private:
    umap<std::type_index, umap<str, uptr<Resource>>> resources_;
  };

}

#endif // LISA_VULKAN_RESOURCEMANAGER_H
