//
// Created by kinami on 4/1/26.
//

#include "Swapchain.h"

#include "graphics/context.h"
#include "utils/logging.h"
#include "window/Window.h"
#include "window/context.h"

#include <algorithm>

namespace lisa::graphics {
  SwapchainImage::SwapchainImage(
    const vk::Image image,
    const ImageFormat& format,
    const vec3& size,
    const vk::ImageUsageFlags usage
  ) :
    Image(image, format, size, usage) {}

  Swapchain::Swapchain(const Surface& surface, const LogicalDevice& device) {
    const auto [color_format, color_space] =
      surface.image_format(context::physical_device());
    color_format_ = color_format;
    color_space_ = color_space;

    vk::Extent2D extent;
    if (surface.capabilities().currentExtent.width == 0xFFFFFFFF) {
      auto size = window::context::window().size();
      extent.width = std::clamp(
        static_cast<uint32_t>(size.x),
        surface.capabilities().minImageExtent.width,
        surface.capabilities().maxImageExtent.width
      );
      extent.height = std::clamp(
        static_cast<uint32_t>(size.y),
        surface.capabilities().minImageExtent.height,
        surface.capabilities().maxImageExtent.height
      );
    } else {
      extent = surface.capabilities().currentExtent;
    }

    const vk::SwapchainCreateInfoKHR swapchain_ci{
      .surface = surface,
      .minImageCount = surface.capabilities().minImageCount,
      .imageFormat = color_format_,
      .imageColorSpace = color_space_,
      .imageExtent = extent,
      .imageArrayLayers = 1,
      .imageUsage = vk::ImageUsageFlagBits::eColorAttachment,
      .preTransform = vk::SurfaceTransformFlagBitsKHR::eIdentity,
      .compositeAlpha = vk::CompositeAlphaFlagBitsKHR::eOpaque,
      .presentMode = vk::PresentModeKHR::eFifo
    };

    swapchain_ = vk::raii::SwapchainKHR(device, swapchain_ci);
    logging::debug("Swapchain created");

    vec3 size_vec{
      static_cast<float>(extent.width), static_cast<float>(extent.height), 1.0f
    };

    for (const auto img : swapchain_.getImages())
      images_.emplace_back(
        img, color_format_, size_vec, vk::ImageUsageFlagBits::eColorAttachment
      );
    logging::debug("Got {} images from the swapchain", images_.size());
  }
}
