//
// Created by kinami on 4/1/26.
//

#include "Swapchain.h"

#include "graphics/context.h"
#include "utils/chk.h"
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
      .imageUsage = vk::ImageUsageFlagBits::eColorAttachment |
                    vk::ImageUsageFlagBits::eTransferDst,
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
        img,
        color_format_,
        size_vec,
        vk::ImageUsageFlagBits::eColorAttachment |
          vk::ImageUsageFlagBits::eTransferDst
      );
    logging::debug("Got {} images from the swapchain", images_.size());
  }

  uint32
    Swapchain::acquire_next_image(const vk::raii::Semaphore& semaphore) const {
    const vk::AcquireNextImageInfoKHR info{
      .swapchain = swapchain_,
      .timeout = UINT64_MAX,
      .semaphore = semaphore,
      .deviceMask = 1
    };
    return utils::chkv(context::device()->acquireNextImage2KHR(info));
  }

  uint32 Swapchain::copy(
    const Image& image,
    const CommandBuffer& cmd_buffer,
    const vk::raii::Semaphore& semaphore
  ) const {
    const auto image_index = acquire_next_image(semaphore);
    auto& swapchain_image = images_[image_index];

    const vk::ImageMemoryBarrier barrier_color_target{
      .srcAccessMask = vk::AccessFlagBits::eColorAttachmentWrite,
      .dstAccessMask = vk::AccessFlagBits::eTransferRead,
      .oldLayout = vk::ImageLayout::eColorAttachmentOptimal,
      .newLayout = vk::ImageLayout::eTransferSrcOptimal,
      .image = image,
      .subresourceRange = {
        .aspectMask = vk::ImageAspectFlagBits::eColor,
        .levelCount = 1,
        .layerCount = 1
      }
    };
    const vk::ImageMemoryBarrier barrier_swapchain_image{
      .srcAccessMask = vk::AccessFlagBits::eNone,
      .dstAccessMask = vk::AccessFlagBits::eTransferWrite,
      .oldLayout = vk::ImageLayout::eUndefined,
      .newLayout = vk::ImageLayout::eTransferDstOptimal,
      .image = swapchain_image,
      .subresourceRange = {
        .aspectMask = vk::ImageAspectFlagBits::eColor,
        .levelCount = 1,
        .layerCount = 1
      }
    };
    cmd_buffer->pipelineBarrier(
      vk::PipelineStageFlagBits::eColorAttachmentOutput,
      vk::PipelineStageFlagBits::eTransfer,
      vk::DependencyFlagBits::eByRegion,
      nullptr,
      nullptr,
      {barrier_color_target, barrier_swapchain_image}
    );

    const std::array offsets = {
      vk::Offset3D{0, 0, 0},
      vk::Offset3D{
        static_cast<int32>(swapchain_image.size().x),
        static_cast<int32>(swapchain_image.size().y),
        1
      }
    };
    const vk::ImageBlit blit{
      .srcSubresource =
        {.aspectMask = vk::ImageAspectFlagBits::eColor, .layerCount = 1},
      .srcOffsets = offsets,
      .dstSubresource =
        {.aspectMask = vk::ImageAspectFlagBits::eColor, .layerCount = 1},
      .dstOffsets = offsets,
    };
    cmd_buffer->blitImage(
      image,
      vk::ImageLayout::eTransferSrcOptimal,
      swapchain_image,
      vk::ImageLayout::eTransferDstOptimal,
      blit,
      vk::Filter::eLinear
    );

    const vk::ImageMemoryBarrier barrier_present{
      .srcAccessMask = vk::AccessFlagBits::eTransferWrite,
      .dstAccessMask = vk::AccessFlagBits::eNone,
      .oldLayout = vk::ImageLayout::eTransferDstOptimal,
      .newLayout = vk::ImageLayout::ePresentSrcKHR,
      .image = swapchain_image,
      .subresourceRange = {
        .aspectMask = vk::ImageAspectFlagBits::eColor,
        .levelCount = 1,
        .layerCount = 1
      }
    };
    cmd_buffer->pipelineBarrier(
      vk::PipelineStageFlagBits::eTransfer,
      vk::PipelineStageFlagBits::eBottomOfPipe,
      vk::DependencyFlagBits::eByRegion,
      nullptr,
      nullptr,
      barrier_present
    );

    return image_index;
  }

  void Swapchain::present(
    uint32 image, const vk::raii::Semaphore& semaphore
  ) const {
    const vk::PresentInfoKHR present_info{
      .waitSemaphoreCount = 1,
      .pWaitSemaphores = &*semaphore,
      .swapchainCount = 1,
      .pSwapchains = &(*swapchain_),
      .pImageIndices = &image
    };

    utils::chk(context::device().queue().presentKHR(present_info));
  }
}
