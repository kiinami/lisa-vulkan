//
// Created by kinami on 4/3/26.
//

#include "Renderer.h"

#include "ShaderData.h"
#include "components/CameraComponent.h"
#include "components/DirectionalLightComponent.h"
#include "components/MeshComponent.h"
#include "components/PointLightComponent.h"
#include "components/TextureComponent.h"
#include "components/TransformComponent.h"
#include "components/context.h"
#include "graphics/context.h"

#include <glm/gtc/matrix_transform.hpp>
#include <sys/stat.h>

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
      global_data_buffers_[i] =
        graphics::Buffer(sizeof(GlobalData), usage, allocation_ci);
      object_data_buffers_[i] = graphics::Buffer(
        sizeof(ObjectData) * graphics::constants::MAX_OBJECTS,
        usage,
        allocation_ci
      );
      point_lights_buffers_[i] = graphics::Buffer(
        sizeof(PointLightData) * graphics::constants::MAX_POINT_LIGHTS,
        usage,
        allocation_ci
      );
      dir_lights_buffers_[i] = graphics::Buffer(
        sizeof(DirLightData) * graphics::constants::MAX_DIR_LIGHTS,
        usage,
        allocation_ci
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

  void Renderer::render(const scene::Scene& scene) {
    reset();

    cmd_buffer_.begin_onetime();

    auto* global_data = static_cast<GlobalData*>(
      global_data_buffers_[current_frame_].mapped_data()
    );

    {
      const auto cameras_view = components::context::registry()
                                  ->view<
                                    const components::TransformComponent,
                                    const components::CameraComponent>();

      for (auto [e, transform_c, camera_c] : cameras_view.each()) {
        global_data->update_camera(transform_c, camera_c);
        break;
      }
    }

    auto* point_light_data = static_cast<PointLightData*>(
      point_lights_buffers_[current_frame_].mapped_data()
    );

    {
      const auto point_lights_view =
        components::context::registry()
          ->view<
            const components::TransformComponent,
            const components::PointLightComponent>();

      uint32 i = 0;
      for (auto [e, transform_c, point_light_c] : point_lights_view.each()) {
        point_light_data[i] = PointLightData(transform_c, point_light_c);
        i++;
      }

      global_data->update_point_lights(
        point_lights_buffers_[current_frame_].address(), i
      );
    }

    auto* dir_light_data = static_cast<DirLightData*>(
      dir_lights_buffers_[current_frame_].mapped_data()
    );

    {
      const auto dir_lights_view =
        components::context::registry()
          ->view<
            const components::TransformComponent,
            const components::DirectionalLightComponent>();

      uint32 i = 0;
      for (auto [e, transform_c, dir_light_c] : dir_lights_view.each()) {
        dir_light_data[i] = DirLightData(transform_c, dir_light_c);
        i++;
      }

      global_data->update_dir_lights(
        dir_lights_buffers_[current_frame_].address(), i
      );
    }

    auto* object_data = static_cast<ObjectData*>(
      object_data_buffers_[current_frame_].mapped_data()
    );

    {
      const auto view = components::context::registry()
                          ->view<
                            const components::TransformComponent,
                            const components::MeshComponent,
                            const components::TextureComponent>();
      uint32 i = 0;
      for (auto
           [entity, transform_component, mesh_component, texture_component] :
           view.each()) {
        object_data[i].model =
          transform_component.matrix() *
          glm::rotate(mat4(1.0f), time_, vec3(0.0f, 1.0f, 0.0f));
        object_data[i].color = vec4(1.0f);
        object_data[i].texture_index =
          texture_component.resource()->descriptor_index();
        i++;
      }
    }

    graph_.render(
      cmd_buffer_,
      scene,
      global_data_buffers_[current_frame_].address(),
      object_data_buffers_[current_frame_].address()
    );

    const auto swapchain_image = swapchain_.copy(
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
