//
// Created by kinami on 5/2/26.
//

#ifndef LISA_VULKAN_RESOURCEMANAGER_H
#define LISA_VULKAN_RESOURCEMANAGER_H

#include "Resource.h"
#include "graphics/context.h"
#include "utils/common.h"

#include <typeindex>

namespace lisa::systems::resources {
  template<typename R>
  concept ResourceDerived = std::derived_from<R, Resource>;

  template<typename S>
  concept ResourceSpecDerived = requires(S* s) {
    []<typename R>(ResourceSpec<R>*)
      requires ResourceDerived<R>
    {}(s);
  };

  class ResourceManager {
  public:
    ResourceManager() = default;
    ~ResourceManager() = default;

    template<ResourceDerived R, ResourceSpecDerived S, typename... Args>
    str add(const str& id, Args&&... args) {
      if (auto* existing = get<R>(id); existing) return id;
      auto& bucket = specs_[std::type_index(typeid(R))];

      bucket[id] = std::make_unique<S>(std::forward<Args>(args)...);
      return id;
    }

    template<ResourceDerived R> void load(const str& id) {
      auto& bucket = specs_[std::type_index(typeid(R))];

      if (const auto it = bucket.find(id); it != bucket.end()) {
        const auto& cmdb = graphics::context::device().cmd_buffer();
        cmdb.begin_onetime();
        resources_[std::type_index(typeid(R))][id] = it->second->load(cmdb);
        cmdb->end();
        graphics::context::device().submit_cmd_buffer_with_fence(cmdb);
        bucket.erase(it);
      }
    }

    void load_all() {
      const auto& cmdb = graphics::context::device().cmd_buffer();
      cmdb.begin_onetime();

      for (auto& [type, bucket] : specs_)
        for (auto& [id, spec] : bucket)
          resources_[type][id] = spec->load(cmdb);

      cmdb->end();
      graphics::context::device().submit_cmd_buffer_with_fence(cmdb);

      specs_.clear();
    }

    template<ResourceDerived R> bool is_added(const str& id) {
      if (const auto it = specs_.find(std::type_index(typeid(R))); it == specs_.end())
        return false;
      const auto& bucket = specs_[std::type_index(typeid(R))];
      return bucket.contains(id);
    }

    template<ResourceDerived T> T* get(const str& id) {
      auto& bucket = resources_[std::type_index(typeid(T))];

      if (const auto it = bucket.find(id); it != bucket.end())
        return static_cast<T*>(it->second.get());
      return nullptr;
    }

  private:
    umap<std::type_index, umap<str, uptr<ResourceSpecBase>>> specs_;
    umap<std::type_index, umap<str, uptr<Resource>>> resources_;
  };

}

#endif // LISA_VULKAN_RESOURCEMANAGER_H
