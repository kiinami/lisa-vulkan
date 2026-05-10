//
// Created by kinami on 4/24/26.
//

#ifndef LISA_VULKAN_VKOBJECT_H
#define LISA_VULKAN_VKOBJECT_H
#include "utils/common.h"

#include <utility>

namespace lisa::graphics {

  template<typename T> class VkObject {
  public:
    VkObject() : object_(nullptr) {}

    ~VkObject() = default;

    VkObject(const VkObject&) = delete;
    VkObject& operator=(const VkObject&) = delete;

    VkObject(VkObject&&) noexcept = default;
    VkObject& operator=(VkObject&&) noexcept = default;

    explicit VkObject(const T& object) : object_(object) {}

    explicit VkObject(T&& object) : object_(std::move(object)) {}

    operator const T&() const { return object_; }

    operator decltype (*std::declval<const T&>())() const { return *object_; }

    T* operator->() { return &object_; }

    const T* operator->() const { return &object_; }

    T& get() { return object_; }

    const T& get() const { return object_; }

    explicit operator bool() const { return static_cast<bool>(object_); }

    decltype(*std::declval<const T&>()) handle() const { return *object_; }

    vk::ObjectType type() { return object_.objectType; }

  protected:
    T object_ = nullptr;

    void set(T&& object) { object_ = std::move(object); }

    uint64_t vk_handle() const {
      auto handle = static_cast<T::CType>(*object_);
      uint64_t result;
      std::memcpy(&result, &handle, sizeof(handle));
      return result;
    }
  };

}

#endif // LISA_VULKAN_VKOBJECT_H
