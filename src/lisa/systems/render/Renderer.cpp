//
// Created by kinami on 4/3/26.
//

#include "Renderer.h"

#include "ShaderData.h"
#include "graphics/context.h"

namespace lisa::systems::render {
  Renderer::Renderer(Rendergraph&& graph) :
    graph_(std::move(graph)),
    cmd_buffer_(graphics::context::device().cmd_buffer()),
    swapchain_(graphics::context::swapchain()) {
    constexpr vk::BufferUsageFlags usage =
      vk::BufferUsageFlagBits::eShaderDeviceAddress |
      vk::BufferUsageFlagBits::eStorageBuffer;
    constexpr vma::AllocationCreateInfo allocation_ci = {
      .flags = vma::AllocationCreateFlagBits::eMapped |
               vma::AllocationCreateFlagBits::eHostAccessSequentialWrite,
      .usage = vma::MemoryUsage::eAuto
    };
    for (size i = 0; i < graphics::constants::MAX_FRAMES_IN_FLIGHT; i++) {
      global_view_buffers_[i] =
        graphics::Buffer(sizeof(GlobalViewData), usage, allocation_ci);
      object_data_buffers_[i] = graphics::Buffer(
        sizeof(ObjectData) * MAX_OBJECTS, usage, allocation_ci
      );
      image_available_semaphores_[i] = graphics::Semaphore();
      render_finished_semaphores_[i] = graphics::Semaphore();
    }
  }

  void Renderer::render() {
    fence_.wait();
    fence_.reset();

    const auto image_index = swapchain_.acquire_next_image(
      image_available_semaphores_[current_frame_]
    );

    cmd_buffer_.reset();
    cmd_buffer_.begin_onetime();

    graph_.render(cmd_buffer_);
    copy_to_swapchain(image_index);

    cmd_buffer_->end();

    vk::PipelineStageFlags wait_stage =
      vk::PipelineStageFlagBits::eColorAttachmentOutput;
    const vk::Semaphore wait_semaphore =
      *image_available_semaphores_[current_frame_];
    const vk::Semaphore signal_semaphore =
      *render_finished_semaphores_[current_frame_];
    vk::SubmitInfo submit_info{
      .waitSemaphoreCount = 1,
      .pWaitSemaphores = &wait_semaphore,
      .pWaitDstStageMask = &wait_stage,
      .commandBufferCount = 1,
      .pCommandBuffers = &*cmd_buffer_,
      .signalSemaphoreCount = 1,
      .pSignalSemaphores = &signal_semaphore
    };
    graphics::context::device().queue().submit(submit_info, *fence_);

    swapchain_.present(
      image_index, render_finished_semaphores_[current_frame_]
    );
    current_frame_ =
      (current_frame_ + 1) % graphics::constants::MAX_FRAMES_IN_FLIGHT;
  }

  void Renderer::copy_to_swapchain(const uint32 image_index) {
    const auto color_target = graph_.get_resource("ColorTarget");
    auto& swapchain_image = swapchain_.images()[image_index];

    const vk::ImageMemoryBarrier barrier_color_target{
      .srcAccessMask = vk::AccessFlagBits::eColorAttachmentWrite,
      .dstAccessMask = vk::AccessFlagBits::eTransferRead,
      .oldLayout = vk::ImageLayout::eColorAttachmentOptimal,
      .newLayout = vk::ImageLayout::eTransferSrcOptimal,
      .image = color_target->image(),
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
    cmd_buffer_->pipelineBarrier(
      vk::PipelineStageFlagBits::eColorAttachmentOutput,
      vk::PipelineStageFlagBits::eTransfer,
      vk::DependencyFlagBits::eByRegion,
      nullptr,
      nullptr,
      {barrier_color_target, barrier_swapchain_image}
    );
    std::array<vk::Offset3D, 2> offsets = {
      vk::Offset3D{0, 0, 0},
      vk::Offset3D{
        static_cast<int32>(swapchain_image.size().x),
        static_cast<int32>(swapchain_image.size().y),
        1
      }
    };

    vk::ImageBlit blit;
    blit.srcSubresource = {
      .aspectMask = vk::ImageAspectFlagBits::eColor, .layerCount = 1
    };
    blit.dstSubresource = {
      .aspectMask = vk::ImageAspectFlagBits::eColor, .layerCount = 1
    };
    blit.srcOffsets[0] = offsets[0];
    blit.srcOffsets[1] = offsets[1];
    blit.dstOffsets[0] = offsets[0];
    blit.dstOffsets[1] = offsets[1];

    cmd_buffer_->blitImage(
      color_target->image(),
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
    cmd_buffer_->pipelineBarrier(
      vk::PipelineStageFlagBits::eTransfer,
      vk::PipelineStageFlagBits::eBottomOfPipe,
      vk::DependencyFlagBits::eByRegion,
      nullptr,
      nullptr,
      barrier_present
    );
  }
}
