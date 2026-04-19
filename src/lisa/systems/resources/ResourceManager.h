//
// Created by kinami on 3/29/26.
//

#ifndef LISA_RESOURCEMANAGER_H
#define LISA_RESOURCEMANAGER_H
#include "Resource.h"
#include "ResourceHandle.h"
#include "utils/common.h"

#include <memory>
#include <ranges>
#include <typeindex>
#include <unordered_map>
#include <vector>

namespace lisa::systems::resources {
  template<typename T>
  concept ResourceDerived = std::derived_from<T, Resource>;

  class ResourceManager {
  public:
    template<ResourceDerived T> ResourceHandle<T> load(const str& id);

    template<ResourceDerived T> T* get(const str& id);

    template<ResourceDerived T> bool exists(const str& id);

    template<ResourceDerived T> void add_ref(const str& id);

    template<ResourceDerived T> void release(const str& id);

    void flush_deleted_resources();

    void unload_all();

  private:
    struct ResourceData {
      uptr<Resource> resource;
      int references = 0;
    };

    umap<std::type_index, umap<str, ResourceData>> resources_;
    vector<uptr<Resource>> dead_resources_;
  };

  template<ResourceDerived T>
  ResourceHandle<T> ResourceManager::load(const str& id) {
    auto& type_resources = resources_[std::type_index(typeid(T))];

    if (type_resources.contains(id)) {
      ++type_resources[id].references;
      return ResourceHandle<T>(id, this);
    }

    auto resource = std::make_unique<T>(id);
    if (!resource->load()) return ResourceHandle<T>();

    type_resources[id] = ResourceData{std::move(resource), 1};

    return ResourceHandle<T>(id, this);
  }

  template<ResourceDerived T> T* ResourceManager::get(const str& id) {
    auto& type_resources = resources_[std::type_index(typeid(T))];

    if (const auto it = type_resources.find(id); it != type_resources.end())
      return static_cast<T*>(it->second.resource.get());

    return nullptr;
  }

  template<ResourceDerived T> bool ResourceManager::exists(const str& id) {
    const auto& type_resources = resources_[std::type_index(typeid(T))];
    return type_resources.contains(id);
  }

  template<ResourceDerived T> void ResourceManager::add_ref(const str& id) {
    auto& type_resources = resources_[std::type_index(typeid(T))];
    if (const auto it = type_resources.find(id); it != type_resources.end())
      it->second.references++;
  }

  template<ResourceDerived T> void ResourceManager::release(const str& id) {
    auto& type_resources = resources_[std::type_index(typeid(T))];
    if (const auto it = type_resources.find(id); it != type_resources.end()) {
      it->second.references--;
      if (it->second.references <= 0) {
        dead_resources_.push_back(std::move(it->second.resource));
        type_resources.erase(it);
      }
    }
  }

  // =========================================================================
  // ResourceHandle inline implementations
  // =========================================================================
  template<typename T>
  ResourceHandle<T>::ResourceHandle() : manager_(nullptr) {}

  template<typename T>
  ResourceHandle<T>::ResourceHandle(const str& id, ResourceManager* manager) :
    id_(id),
    manager_(manager) {}

  template<typename T> ResourceHandle<T>::~ResourceHandle() {
    if (manager_ && !id_.empty()) manager_->release<T>(id_);
  }

  template<typename T>
  ResourceHandle<T>::ResourceHandle(const ResourceHandle& other) :
    id_(other.id_),
    manager_(other.manager_) {
    if (manager_ && !id_.empty()) manager_->add_ref<T>(id_);
  }

  template<typename T>
  ResourceHandle<T>& ResourceHandle<T>::operator=(const ResourceHandle& other) {
    if (this != &other) {
      if (manager_ && !id_.empty()) manager_->release<T>(id_);
      id_ = other.id_;
      manager_ = other.manager_;
      if (manager_ && !id_.empty()) manager_->add_ref<T>(id_);
    }
    return *this;
  }

  template<typename T>
  ResourceHandle<T>::ResourceHandle(ResourceHandle&& other) noexcept :
    id_(std::move(other.id_)),
    manager_(other.manager_) {
    other.manager_ = nullptr;
    other.id_.clear();
  }

  template<typename T>
  ResourceHandle<T>&
    ResourceHandle<T>::operator=(ResourceHandle&& other) noexcept {
    if (this != &other) {
      if (manager_ && !id_.empty()) manager_->release<T>(id_);
      id_ = std::move(other.id_);
      manager_ = other.manager_;
      other.manager_ = nullptr;
      other.id_.clear();
    }
    return *this;
  }

  template<typename T> T* ResourceHandle<T>::operator->() const {
    return manager_ ? manager_->get<T>(id_) : nullptr;
  }

  template<typename T> T* ResourceHandle<T>::get() const {
    return manager_ ? manager_->get<T>(id_) : nullptr;
  }

  template<typename T> ResourceHandle<T>::operator bool() const {
    return manager_ != nullptr && manager_->exists<T>(id_);
  }

}

#endif // LISA_RESOURCEMANAGER_H
