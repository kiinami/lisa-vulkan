//
// Created by kinami on 4/1/26.
//

#ifndef LISA_VULKAN_IMAGEFORMAT_H
#define LISA_VULKAN_IMAGEFORMAT_H

#include "graphics/vk/VkEnum.h"
#include "utils/common.h"

#include <vulkan/vulkan_raii.hpp>

namespace lisa::graphics {

  class ImageFormat : public VkEnum<vk::Format> {
  public:
    using VkEnum::VkEnum;

    explicit ImageFormat(VkFormat format) :
      VkEnum(static_cast<vk::Format>(format)) {}

    ImageFormat(vk::Format format = vk::Format::eUndefined) : VkEnum(format) {}

    bool is_depth() const noexcept;
    bool is_stencil() const noexcept;

    bool is_color() const noexcept { return !is_depth() && !is_stencil(); }

    vk::ImageAspectFlags aspect_mask() const noexcept;
  };

}

#endif // LISA_VULKAN_IMAGEFORMAT_H
