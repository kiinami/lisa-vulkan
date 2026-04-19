//
// Created by kinami on 3/29/26.
//

#ifndef LISA_RESOURCEHANDLE_H
#define LISA_RESOURCEHANDLE_H
#include "ResourceManager.h"
#include "utils/common.h"

namespace lisa::systems::resources {

  template<typename T> class ResourceHandle {
  public:
    ResourceHandle();
    ResourceHandle(const str& id, ResourceManager* manager);

    ~ResourceHandle();

    ResourceHandle(const ResourceHandle& other);
    ResourceHandle& operator=(const ResourceHandle& other);

    ResourceHandle(ResourceHandle&& other) noexcept;
    ResourceHandle& operator=(ResourceHandle&& other) noexcept;

    T* operator->() const;
    T* get() const;
    explicit operator bool() const;

  private:
    str id_;
    ResourceManager* manager_;
  };

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

#endif // LISA_RESOURCEHANDLE_H
