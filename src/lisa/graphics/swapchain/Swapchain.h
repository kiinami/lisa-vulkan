//
// Created by kinami on 4/1/26.
//

#ifndef LISA_VULKAN_SWAPCHAIN_H
#define LISA_VULKAN_SWAPCHAIN_H

#include "Surface.h"
#include "graphics/device/LogicalDevice.h"
#include "graphics/images/ImageFormat.h"

#include <vulkan/vulkan_raii.hpp>

namespace lisa::graphics {
  class Image;

  struct SwapchainImage {
    vk::Image image;
    ImageFormat format;
    vec3 size;
    vk::ImageUsageFlags usage;

    explicit SwapchainImage(
      const vk::Image image,
      const ImageFormat& format,
      const vec3& size,
      const vk::ImageUsageFlags usage
    ) :
      image(image),
      format(format),
      size(size),
      usage(usage) {}
  };

  class Swapchain {
  public:
    explicit Swapchain(const Surface& surface, const LogicalDevice& device);
    ~Swapchain() = default;

    operator const vk::raii::SwapchainKHR&() { return swapchain_; }

    const vector<SwapchainImage>& images() const { return images_; }

    uint32 acquire_next_image(const vk::raii::Semaphore& semaphore) const;
    uint32 copy(
      const Image& image,
      const CommandBuffer& cmd_buffer,
      const vk::raii::Semaphore& semaphore
    ) const;
    void present(uint32 image, const vk::raii::Semaphore& semaphore) const;

    vk::Extent2D extent() const {
      const auto& size = images_.front().size;
      return {static_cast<uint32>(size.x), static_cast<uint32>(size.y)};
    }

  private:
    vk::raii::SwapchainKHR swapchain_ = nullptr;
    ImageFormat color_format_{vk::Format::eUndefined};
    vk::ColorSpaceKHR color_space_;

    vector<SwapchainImage> images_;
  };

}

#endif // LISA_VULKAN_SWAPCHAIN_H
