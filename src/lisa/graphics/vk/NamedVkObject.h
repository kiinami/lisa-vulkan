//
// Created by kinami on 4/24/26.
//

#ifndef LISA_VULKAN_NAMEDVKOBJECT_H
#define LISA_VULKAN_NAMEDVKOBJECT_H

#include "VkObject.h"
#include "build.h"
#include "graphics/context.h"
#include "utils/common.h"

namespace lisa::graphics {

  template<typename T> class NamedVkObject : public VkObject<T> {
  public:
    NamedVkObject() = default;

    NamedVkObject(const str& id) : id_(id) {}

    NamedVkObject(T&& object, str id) :
      VkObject<T>(std::move(object)),
      id_(std::move(id)) {}

    NamedVkObject(const NamedVkObject&) = delete;
    NamedVkObject& operator=(const NamedVkObject&) = delete;

    NamedVkObject(NamedVkObject&&) noexcept = default;
    NamedVkObject& operator=(NamedVkObject&&) noexcept = default;

    const str& id() const { return id_; }

  protected:
    void set(T&& object);

  private:
    str id_;
  };

  template<typename T> void NamedVkObject<T>::set(T&& object) {
    VkObject<T>::set(std::move(object));
    if constexpr (build::debug) {
      const vk::DebugUtilsObjectNameInfoEXT name_info{
        .objectType = this->type(),
        .objectHandle = this->vk_handle(),
        .pObjectName = id_.c_str()
      };
      context::device()->setDebugUtilsObjectNameEXT(name_info);
    }
  }

}

#endif // LISA_VULKAN_NAMEDVKOBJECT_H
