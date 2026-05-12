//
// Created by kinami on 5/2/26.
//

#ifndef LISA_VULKAN_RESOURCEMANAGER_H
#define LISA_VULKAN_RESOURCEMANAGER_H

#include "Resource.h"
#include "graphics/context.h"
#include "utils/common.h"
#include "utils/logging.h"

#include <boost/multi_index/hashed_index.hpp>
#include <boost/multi_index/key.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index_container.hpp>
#include <typeindex>

namespace mi = boost::multi_index;

namespace lisa::systems::resources {
  template<typename R>
  concept ResourceDerived = std::derived_from<R, Resource>;

  template<typename S>
  concept ResourceSpecDerived = requires(S* s) {
    []<typename R>(ResourceSpec<R>*)
      requires ResourceDerived<R>
    {}(s);
  };

  template<typename T> struct ById {
    using result_type = const str&;

    result_type operator()(const uptr<T>& r) const { return r->id; }
  };

  template<typename T> struct ByTypeIndex;

  template<> struct ByTypeIndex<Resource> {
    using result_type = std::type_index;

    result_type operator()(const uptr<Resource>& r) const {
      return std::type_index(typeid(*r));
    }
  };

  template<> struct ByTypeIndex<ResourceSpecBase> {
    using result_type = std::type_index;

    result_type operator()(const uptr<ResourceSpecBase>& r) const {
      return r->resource_type();
    }
  };

  using ResourceStore = mi::multi_index_container<
    uptr<Resource>,
    mi::indexed_by<
      mi::hashed_unique<
        mi::
          composite_key<uptr<Resource>, ByTypeIndex<Resource>, ById<Resource>>,
        mi::composite_key_hash<std::hash<std::type_index>, std::hash<str>>>,
      mi::hashed_non_unique<ById<Resource>>>>;

  using ResourceSpecStore = mi::multi_index_container<
    uptr<ResourceSpecBase>,
    mi::indexed_by<
      mi::hashed_unique<
        mi::composite_key<
          uptr<ResourceSpecBase>,
          ByTypeIndex<ResourceSpecBase>,
          ById<ResourceSpecBase>>,
        mi::composite_key_hash<std::hash<std::type_index>, std::hash<str>>>,
      mi::hashed_non_unique<ById<ResourceSpecBase>>>>;

  class ResourceManager {
  public:
    ResourceManager() = default;
    ~ResourceManager() = default;

    template<ResourceDerived R, ResourceSpecDerived S, typename... Args>
    str add(const str& id, Args&&... args);
    template<ResourceDerived R>
    void load(const str& id, const graphics::CommandBuffer& cmdb);
    template<ResourceDerived R> void load(const str& id);
    void load_all(const graphics::CommandBuffer& cmdb);
    void load_all();
    template<ResourceDerived R> bool is_added(const str& id);
    template<ResourceDerived R> R* get(const str& id);
    auto get(const str& id);

  private:
    ResourceSpecStore specs_;
    ResourceStore resources_;
  };

  template<ResourceDerived R, ResourceSpecDerived S, typename... Args>
  str ResourceManager::add(const str& id, Args&&... args) {
    if (is_added<R>(id)) return id;

    specs_.insert(std::make_unique<S>(id, std::forward<Args>(args)...));
    return id;
  }

  template<ResourceDerived R>
  void
    ResourceManager::load(const str& id, const graphics::CommandBuffer& cmdb) {
    auto& primary = specs_.get<0>();
    const auto it =
      primary.find(std::make_tuple(std::type_index(typeid(R)), id));
    if (it == primary.end()) return;
    resources_.insert((*it)->load(cmdb));
    primary.erase(it);
  }

  template<ResourceDerived R> void ResourceManager::load(const str& id) {
    const auto& cmdb = graphics::context::device().cmd_buffer();
    cmdb.begin_onetime();
    load<R>(id, cmdb);
    cmdb->end();
    graphics::context::device().submit_cmd_buffer_with_fence(cmdb);
  }

  inline void ResourceManager::load_all(const graphics::CommandBuffer& cmdb) {
    for (const auto& spec : specs_)
      resources_.insert(spec->load(cmdb));
    specs_.clear();
  }

  inline void ResourceManager::load_all() {
    const auto& cmdb = graphics::context::device().cmd_buffer();
    cmdb.begin_onetime();
    load_all(cmdb);
    cmdb->end();
    graphics::context::device().submit_cmd_buffer_with_fence(cmdb);
  }

  template<ResourceDerived R> bool ResourceManager::is_added(const str& id) {
    auto& res_primary = resources_.get<0>();
    if (res_primary.find(std::make_tuple(std::type_index(typeid(R)), id)) !=
        res_primary.end()) {
      return true;
    }

    auto& spec_primary = specs_.get<0>();
    return spec_primary.find(std::make_tuple(std::type_index(typeid(R)), id)) !=
           spec_primary.end();
  }

  template<ResourceDerived R> R* ResourceManager::get(const str& id) {
    auto& primary = resources_.get<0>();
    const auto it =
      primary.find(std::make_tuple(std::type_index(typeid(R)), id));
    if (it == primary.end()) {
      logging::warning(
        "Resource of type {} with ID '{}' not found.", typeid(R).name(), id
      );
      return nullptr;
    }
    return static_cast<R*>(it->get());
  }

  inline auto ResourceManager::get(const str& id) {
    const auto& secondary = resources_.get<1>();
    return secondary.equal_range(id);
  }

}

#endif // LISA_VULKAN_RESOURCEMANAGER_H
