//
// Created by kinami on 4/1/26.
//

#include "ImageFormat.h"

namespace lisa::graphics {
  bool ImageFormat::is_depth() const {
    switch (format_) {
      case vk::Format::eD16Unorm:
      case vk::Format::eD32Sfloat:
      case vk::Format::eD16UnormS8Uint:
      case vk::Format::eD24UnormS8Uint:
      case vk::Format::eD32SfloatS8Uint: return true;

      default: return false;
    }
  }

  bool ImageFormat::is_stencil() const {
    switch (format_) {
      case vk::Format::eD16UnormS8Uint:
      case vk::Format::eD24UnormS8Uint:
      case vk::Format::eD32SfloatS8Uint: return true;

      default: return false;
    }
  }

  vk::ImageAspectFlags ImageFormat::aspect_mask() const {
    if (is_depth())
      if (is_stencil())
        return vk::ImageAspectFlagBits::eDepth |
               vk::ImageAspectFlagBits::eStencil;
      return vk::ImageAspectFlagBits::eDepth;
    return vk::ImageAspectFlagBits::eColor;
  }

}
