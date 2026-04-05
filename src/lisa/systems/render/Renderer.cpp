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
      available_s_[i] = graphics::Semaphore();
      finished_s_[i] = graphics::Semaphore();
    }
  }

  void Renderer::reset() const {
    fence_.wait();
    fence_.reset();
    cmd_buffer_.reset();
  }

  void Renderer::submit_to_queue() const {
    vk::PipelineStageFlags wait_stage =
      vk::PipelineStageFlagBits::eColorAttachmentOutput;
    const vk::Semaphore wait_semaphore = *available_s_[current_frame_];
    const vk::Semaphore signal_semaphore = *finished_s_[current_frame_];
    const vk::SubmitInfo submit_info{
      .waitSemaphoreCount = 1,
      .pWaitSemaphores = &wait_semaphore,
      .pWaitDstStageMask = &wait_stage,
      .commandBufferCount = 1,
      .pCommandBuffers = &*cmd_buffer_,
      .signalSemaphoreCount = 1,
      .pSignalSemaphores = &signal_semaphore
    };
    graphics::context::device().queue().submit(submit_info, *fence_);
  }

  void Renderer::render() {
    reset();

    cmd_buffer_.begin_onetime();

    graph_.render(cmd_buffer_);
    auto swapchain_image = swapchain_.copy(
      graph_.get_resource("FinalTarget")->image(),
      cmd_buffer_,
      available_s_[current_frame_]
    );

    cmd_buffer_->end();

    submit_to_queue();

    swapchain_.present(swapchain_image, finished_s_[current_frame_]);

    current_frame_ =
      (current_frame_ + 1) % graphics::constants::MAX_FRAMES_IN_FLIGHT;
  }
}
