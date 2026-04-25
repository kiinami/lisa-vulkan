//
// Created by kinami on 3/29/26.
//

#ifndef LISA_RESOURCEMANAGER_H
#define LISA_RESOURCEMANAGER_H
#include "Resource.h"
#include "utils/common.h"

#include <memory>
#include <ranges>
#include <typeindex>
#include <unordered_map>
#include <vector>

namespace lisa::systems::resources {
  template<typename T>
  concept ResourceDerived = std::derived_from<T, Resource>;

  template<typename T> class ResourceHandle;

  class ResourceManager {
  public:
    template<ResourceDerived T> ResourceHandle<T> load(const path& filepath);

    template<ResourceDerived T> T* get(const path& filepath);

    template<ResourceDerived T> bool exists(const path& filepath);

    template<ResourceDerived T> void add_ref(const path& filepath);

    template<ResourceDerived T> void release(const path& filepath);

    void flush_deleted_resources();

    void unload_all();

  private:
    struct ResourceData {
      uptr<Resource> resource;
      int references = 0;
    };

    umap<std::type_index, umap<path, ResourceData>> resources_;
    vector<uptr<Resource>> dead_resources_;
  };

  template<ResourceDerived T>
  ResourceHandle<T> ResourceManager::load(const path& filepath) {
    auto& type_resources = resources_[std::type_index(typeid(T))];

    if (type_resources.contains(filepath)) {
      ++type_resources[filepath].references;
      return ResourceHandle<T>(filepath, this);
    }

    auto resource = std::make_unique<T>(filepath);
    if (!resource->load()) return ResourceHandle<T>();

    type_resources[filepath] = ResourceData{std::move(resource), 1};

    return ResourceHandle<T>(filepath, this);
  }

  template<ResourceDerived T> T* ResourceManager::get(const path& filepath) {
    auto& type_resources = resources_[std::type_index(typeid(T))];

    if (
      const auto it = type_resources.find(filepath); it != type_resources.end()
    )
      return static_cast<T*>(it->second.resource.get());

    return nullptr;
  }

  template<ResourceDerived T>
  bool ResourceManager::exists(const path& filepath) {
    const auto& type_resources = resources_[std::type_index(typeid(T))];
    return type_resources.contains(filepath);
  }

  template<ResourceDerived T>
  void ResourceManager::add_ref(const path& filepath) {
    auto& type_resources = resources_[std::type_index(typeid(T))];
    if (
      const auto it = type_resources.find(filepath); it != type_resources.end()
    )
      ++it->second.references;
  }

  template<ResourceDerived T>
  void ResourceManager::release(const path& filepath) {
    auto& type_resources = resources_[std::type_index(typeid(T))];
    if (
      const auto it = type_resources.find(filepath); it != type_resources.end()
    ) {
      --it->second.references;
      if (it->second.references <= 0) {
        dead_resources_.push_back(std::move(it->second.resource));
        type_resources.erase(it);
      }
    }
  }
}

#endif // LISA_RESOURCEMANAGER_H
