//
// Created by kinami on 4/1/26.
//

#ifndef LISA_VULKAN_IMAGEFORMAT_H
#define LISA_VULKAN_IMAGEFORMAT_H

#include "utils/common.h"

#include <vulkan/vulkan_raii.hpp>

namespace lisa::graphics {

  class ImageFormat {
  public:
    explicit ImageFormat(const vk::Format format = vk::Format::eUndefined) :
      format_(format) {}

    explicit ImageFormat(const VkFormat format = VK_FORMAT_UNDEFINED) :
      format_(static_cast<vk::Format>(format)) {}

    ImageFormat(const ImageFormat&) = default;
    ImageFormat& operator=(const ImageFormat&) = default;
    bool operator==(const ImageFormat&) const = default;

    operator const vk::Format&() const { return format_; }

    const vk::Format* operator*() const { return &format_; }

    operator uint32() const { return static_cast<uint32>(format_); }

    bool is_depth() const;
    bool is_stencil() const;
    vk::ImageAspectFlags aspect_mask() const;

  private:
    vk::Format format_;
  };

}

#endif // LISA_VULKAN_IMAGEFORMAT_H
