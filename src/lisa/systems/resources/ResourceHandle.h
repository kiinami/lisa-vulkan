//
// Created by kinami on 3/29/26.
//

#ifndef LISA_RESOURCEHANDLE_H
#define LISA_RESOURCEHANDLE_H
#include "utils/common.h"

namespace lisa::systems::resources {

  class ResourceManager;

  template<typename T> class ResourceHandle {
  public:
    ResourceHandle();
    ResourceHandle(const str& id, ResourceManager* manager);

    // RAII methods
    ~ResourceHandle();

    // Copy semantics
    ResourceHandle(const ResourceHandle& other);
    ResourceHandle& operator=(const ResourceHandle& other);

    // Move semantics
    ResourceHandle(ResourceHandle&& other) noexcept;
    ResourceHandle& operator=(ResourceHandle&& other) noexcept;

    // Accessors
    T* operator->() const;
    T* get() const;
    explicit operator bool() const;

  private:
    str id_;
    ResourceManager* manager_;
  };

}

#endif // LISA_RESOURCEHANDLE_H
