//
// Created by kinami on 4/24/26.
//

#ifndef LISA_VULKAN_VKENUM_H
#define LISA_VULKAN_VKENUM_H

#include "utils/common.h"

#include <string_view>
#include <type_traits>

namespace lisa::graphics {

  template<typename E>
  concept VulkanEnum = requires {
    requires std::is_enum_v<E>;
    typename vk::CppType<E, static_cast<E>(0)>;
  };

  template<typename E>
    requires std::is_enum_v<E>
  class VkEnum {
  public:
    using UnderlyingType = std::underlying_type_t<E>;

    constexpr VkEnum() noexcept : value_(static_cast<E>(0)) {}

    constexpr explicit VkEnum(E value) noexcept : value_(value) {}

    constexpr explicit VkEnum(UnderlyingType raw) noexcept :
      value_(static_cast<E>(raw)) {}

    constexpr E get() const noexcept { return value_; }

    constexpr UnderlyingType raw() const noexcept {
      return static_cast<UnderlyingType>(value_);
    }

    constexpr operator E() const noexcept { return value_; }

    constexpr bool operator==(const VkEnum& rhs) const noexcept = default;
    constexpr bool operator!=(const VkEnum& rhs) const noexcept = default;

    constexpr bool operator==(E rhs) const noexcept { return value_ == rhs; }

    constexpr bool operator!=(E rhs) const noexcept { return value_ != rhs; }

    constexpr explicit operator bool() const noexcept { return raw() != 0; }

    constexpr bool is_undefined() const noexcept { return raw() == 0; }

    constexpr VkEnum operator|(const VkEnum& rhs) const noexcept {
      return VkEnum(raw() | rhs.raw());
    }

    constexpr VkEnum operator&(const VkEnum& rhs) const noexcept {
      return VkEnum(raw() & rhs.raw());
    }

    constexpr VkEnum operator^(const VkEnum& rhs) const noexcept {
      return VkEnum(raw() ^ rhs.raw());
    }

    constexpr VkEnum operator~() const noexcept { return VkEnum(~raw()); }

    constexpr VkEnum& operator|=(const VkEnum& rhs) noexcept {
      value_ = static_cast<E>(raw() | rhs.raw());
      return *this;
    }

    constexpr VkEnum& operator&=(const VkEnum& rhs) noexcept {
      value_ = static_cast<E>(raw() & rhs.raw());
      return *this;
    }

    constexpr VkEnum& operator^=(const VkEnum& rhs) noexcept {
      value_ = static_cast<E>(raw() ^ rhs.raw());
      return *this;
    }

    constexpr bool has(E flag) const noexcept {
      return (raw() & static_cast<UnderlyingType>(flag)) != 0;
    }

    constexpr bool has_all(UnderlyingType mask) const noexcept {
      return (raw() & mask) == mask;
    }

    constexpr bool has_none(UnderlyingType mask) const noexcept {
      return (raw() & mask) == 0;
    }

    [[nodiscard]]
    str to_string() const noexcept {
      return vk::to_string(value_);
    }

  protected:
    E value_;

    constexpr void set(E value) noexcept { value_ = value; }

    constexpr void set(UnderlyingType v) noexcept {
      value_ = static_cast<E>(v);
    }
  };

}

#endif // LISA_VULKAN_VKENUM_H
