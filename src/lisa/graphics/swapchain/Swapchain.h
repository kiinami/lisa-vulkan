//
// Created by kinami on 4/1/26.
//

#ifndef LISA_VULKAN_SWAPCHAIN_H
#define LISA_VULKAN_SWAPCHAIN_H

#include "Surface.h"
#include "graphics/device/LogicalDevice.h"
#include "graphics/images/Image.h"
#include "graphics/images/ImageFormat.h"

#include <vulkan/vulkan_raii.hpp>

namespace lisa::graphics {

  class SwapchainImage : public Image {
  public:
    explicit SwapchainImage(
      vk::Image image,
      const ImageFormat& format,
      const vec3& size,
      vk::ImageUsageFlags usage
    );

    SwapchainImage(SwapchainImage&&) noexcept = default;
    SwapchainImage& operator=(SwapchainImage&&) noexcept = default;

  private:
    friend class Swapchain;
  };

  class Swapchain {
  public:
    explicit Swapchain(
      const Surface& surface,
      const LogicalDevice& device
    );
    ~Swapchain() = default;

  private:
    vk::raii::SwapchainKHR swapchain_ = nullptr;
    ImageFormat color_format_{vk::Format::eUndefined};
    vk::ColorSpaceKHR color_space_;

    vector<SwapchainImage> images_;
  };

}

#endif // LISA_VULKAN_SWAPCHAIN_H
