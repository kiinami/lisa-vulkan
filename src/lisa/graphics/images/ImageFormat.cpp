//
// Created by kinami on 4/1/26.
//

#include "ImageFormat.h"

namespace lisa::graphics {

  bool ImageFormat::is_depth() const noexcept {
    switch (value_) {
      case vk::Format::eD16Unorm:
      case vk::Format::eD32Sfloat:
      case vk::Format::eX8D24UnormPack32:
      case vk::Format::eD16UnormS8Uint:
      case vk::Format::eD24UnormS8Uint:
      case vk::Format::eD32SfloatS8Uint: return true;
      default: return false;
    }
  }

  bool ImageFormat::is_stencil() const noexcept {
    switch (value_) {
      case vk::Format::eS8Uint:
      case vk::Format::eD16UnormS8Uint:
      case vk::Format::eD24UnormS8Uint:
      case vk::Format::eD32SfloatS8Uint: return true;
      default: return false;
    }
  }

  vk::ImageAspectFlags ImageFormat::aspect_mask() const noexcept {
    vk::ImageAspectFlags flags;
    if (is_depth()) flags |= vk::ImageAspectFlagBits::eDepth;
    if (is_stencil()) flags |= vk::ImageAspectFlagBits::eStencil;
    if (is_color()) flags |= vk::ImageAspectFlagBits::eColor;
    return flags;
  }

}
