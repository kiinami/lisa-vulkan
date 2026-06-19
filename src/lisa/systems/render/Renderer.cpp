//
// Created by kinami on 4/3/26.
//

#include "Renderer.h"

#include "ShaderData.h"
#include "components/CameraComponent.h"
#include "components/DirectionalLightComponent.h"
#include "components/MaterialComponent.h"
#include "components/MeshComponent.h"
#include "components/PointLightComponent.h"
#include "components/TransformComponent.h"
#include "components/context.h"
#include "graphics/context.h"
#include "window/context.h"

namespace lisa::systems::render {
  Renderer::Renderer(const path& graph_filepath) :
    graph_(graph_filepath),
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
    for (size i = 0; i < constants::MAX_FRAMES_IN_FLIGHT; i++) {
      global_data_buffers_[i] = graphics::Buffer(
        logging::genid("graph", "frames", i, "global_data"),
        sizeof(GlobalData),
        usage,
        allocation_ci
      );
      object_data_buffers_[i] = graphics::Buffer(
        logging::genid("graph", "frames", i, "object_data"),
        sizeof(ObjectData) * constants::MAX_OBJECTS,
        usage,
        allocation_ci
      );
      point_lights_buffers_[i] = graphics::Buffer(
        logging::genid("graph", "frames", i, "point_lights"),
        sizeof(PointLightData) * constants::MAX_POINT_LIGHTS,
        usage,
        allocation_ci
      );
      dir_lights_buffers_[i] = graphics::Buffer(
        logging::genid("graph", "frames", i, "dir_lights"),
        sizeof(DirLightData) * constants::MAX_DIR_LIGHTS,
        usage,
        allocation_ci
      );
      ambient_lights_buffers_[i] = graphics::Buffer(
        logging::genid("graph", "frames", i, "ambient_lights"),
        sizeof(AmbientLightData) * constants::MAX_AMBIENT_LIGHTS,
        usage,
        allocation_ci
      );
      shadow_data_buffers_[i] = graphics::Buffer(
        logging::genid("graph", "frames", i, "shadow_data"),
        sizeof(ShadowData) * (constants::MAX_SHADOW_LAYERS),
        usage,
        allocation_ci
      );
      available_s_[i] = graphics::Semaphore();
    }
    for (size i = 0; i < swapchain_.images().size(); i++)
      finished_s_.emplace_back();
  }

  void Renderer::reset() const {
    fence_.wait();
    fence_.reset();
    cmd_buffer_.reset();
  }

  void Renderer::submit_to_queue(const uint32 swapchain_image_index) const {
    vk::PipelineStageFlags wait_stage =
      vk::PipelineStageFlagBits::eColorAttachmentOutput;
    auto wait_s_handle = available_s_[current_frame_].handle();
    auto finished_s_handle = finished_s_[swapchain_image_index].handle();
    auto cmdbuffer_handle = cmd_buffer_.handle();
    const vk::SubmitInfo submit_info{
      .waitSemaphoreCount = 1,
      .pWaitSemaphores = &wait_s_handle,
      .pWaitDstStageMask = &wait_stage,
      .commandBufferCount = 1,
      .pCommandBuffers = &cmdbuffer_handle,
      .signalSemaphoreCount = 1,
      .pSignalSemaphores = &finished_s_handle
    };
    graphics::context::device().queue().submit(submit_info, fence_.handle());
  }

  void Renderer::render() {
    reset();

    cmd_buffer_.begin_onetime();
    cmd_buffer_.begin_region("setup");

    auto* global_data = static_cast<GlobalData*>(
      global_data_buffers_[current_frame_].mapped_data()
    );

    {
      auto cameras_view =
        components::context::registry()
          ->view<components::TransformComponent, components::CameraComponent>();

      for (auto [e, transform_c, camera_c] : cameras_view.each()) {
        const auto [width, height] = swapchain_.extent();
        camera_c.set_aspect_ratio(
          static_cast<float>(width) / static_cast<float>(height)
        );
        global_data->update_camera(transform_c, camera_c);
        break;
      }
    }

    auto* shadow_data = static_cast<ShadowData*>(
      shadow_data_buffers_[current_frame_].mapped_data()
    );
    uint32 shadow_count = 0;

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
      uint32 shadowed_point_lights = 0;
      for (auto [e, transform_c, point_light_c] : point_lights_view.each()) {
        point_light_data[i] = PointLightData(transform_c, point_light_c);
        if (
          point_light_c.cast_shadows &&
          shadowed_point_lights <
          constants::MAX_POINT_LIGHT_SHADOWS &&
          shadow_count +
          6 <= constants::MAX_SHADOW_LAYERS
        ) {
          const auto proj = point_light_c.projection();
          const auto& views = point_light_c.views(transform_c);
          point_light_data[i].shadow_base_index = shadow_count;
          for (uint32 j = 0; j < 6; j++) {
            shadow_data[shadow_count] = ShadowData{
              .view_projection = proj * views[j], .layer = shadow_count
            };
            shadow_count++;
          }
          shadowed_point_lights++;
        }
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
      uint32 shadowed_dir_lights = 0;
      for (auto [e, transform_c, dir_light_c] : dir_lights_view.each()) {
        dir_light_data[i] = DirLightData(transform_c, dir_light_c);
        if (
          dir_light_c.cast_shadows &&
          shadowed_dir_lights <
          constants::MAX_DIR_LIGHT_SHADOWS &&
          shadow_count < constants::MAX_SHADOW_LAYERS
        ) {
          dir_light_data[i].shadow_index = shadow_count;
          shadow_data[shadow_count] = ShadowData{
            .view_projection =
              dir_light_c.shadow_view_projection(transform_c.direction()),
            .layer = shadow_count
          };
          shadow_count++;
          shadowed_dir_lights++;
        }
        i++;
      }

      global_data->update_dir_lights(
        dir_lights_buffers_[current_frame_].address(), i
      );
    }

    {
      float cone_sum = 0.0f, weight_sum = 0.0f;
      const uint32 pt_count = global_data->point_lights_count;
      const uint32 dir_count = global_data->dir_lights_count;
      for (uint32 i = 0; i < pt_count; i++) {
        const auto& l = point_light_data[i];
        const float approx =
          l.radius > 0.0f ? l.source_radius / l.radius : 0.0f;
        cone_sum   += l.intensity * approx;
        weight_sum += l.intensity;
      }
      for (uint32 i = 0; i < dir_count; i++) {
        const auto& l = dir_light_data[i];
        cone_sum   += l.intensity * glm::sin(l.angular_radius);
        weight_sum += l.intensity;
      }
      global_data->soft_shadow_cone_r =
        weight_sum > 0.0f ? cone_sum / weight_sum : 0.0f;
    }

    auto* ambient_light_data = static_cast<AmbientLightData*>(
      ambient_lights_buffers_[current_frame_].mapped_data()
    );

    {
      const auto ambient_lights_view =
        components::context::registry()
          ->view<const components::AmbientLightComponent>();

      uint32 i = 0;
      for (auto [e, dir_light_c] : ambient_lights_view.each()) {
        ambient_light_data[i] = AmbientLightData(dir_light_c);
        i++;
      }

      global_data->update_ambient_lights(
        ambient_lights_buffers_[current_frame_].address(), i
      );
    }

    global_data->update_shadow_data(
      shadow_data_buffers_[current_frame_].address(), shadow_count
    );

    global_data->texel_size = window::context::texel_size();

    auto* object_data = static_cast<ObjectData*>(
      object_data_buffers_[current_frame_].mapped_data()
    );

    {
      const auto view = components::context::registry()
                          ->view<
                            const components::TransformComponent,
                            const components::MeshComponent>();
      uint32 i = 0;
      for (auto [entity, transform_component, mesh_component] : view.each()) {
        object_data[i].model = transform_component.matrix();

        const auto* material_component =
          components::context::registry()
            ->try_get<components::MaterialComponent>(entity);
        if (material_component) {
          object_data[i].color = vec4(material_component->albedo, 1.0f);
          object_data[i].roughness = material_component->roughness;
          object_data[i].metallic = material_component->metallic;

          object_data[i].diffuse_texture_index =
            std::numeric_limits<uint32>::max();
          object_data[i].roughness_texture_index =
            std::numeric_limits<uint32>::max();
          object_data[i].metallic_texture_index =
            std::numeric_limits<uint32>::max();
          object_data[i].normal_texture_index =
            std::numeric_limits<uint32>::max();
          if (
            const auto res = material_component->albedo_texture();
            res != nullptr
          )
            object_data[i].diffuse_texture_index = res->descriptor_index();
          if (
            const auto res = material_component->roughness_texture();
            res != nullptr
          )
            object_data[i].roughness_texture_index = res->descriptor_index();
          if (
            const auto res = material_component->metallic_texture();
            res != nullptr
          )
            object_data[i].metallic_texture_index = res->descriptor_index();
          if (
            const auto res = material_component->normal_texture();
            res != nullptr
          )
            object_data[i].normal_texture_index = res->descriptor_index();
        } else {
          object_data[i].color = vec4(1.0f);
          object_data[i].roughness = 1.0f;
          object_data[i].metallic = 0.0f;

          object_data[i].diffuse_texture_index =
            std::numeric_limits<uint32>::max();
          object_data[i].roughness_texture_index =
            std::numeric_limits<uint32>::max();
          object_data[i].metallic_texture_index =
            std::numeric_limits<uint32>::max();
          object_data[i].normal_texture_index =
            std::numeric_limits<uint32>::max();
        }

        i++;
      }
    }

    cmd_buffer_.end_region();

#ifdef VK_KHR_acceleration_structure
    if (graphics::constants::has_acceleration_structure()) {
      vector<graphics::AccelerationStructure::TlasInstance> instances;
      uint32 tlas_i = 0;
      const auto tlas_view = components::context::registry()
        ->view<const components::TransformComponent,
               const components::MeshComponent>();
      for (auto [entity, transform, mesh_comp] : tlas_view.each()) {
        const auto* mesh = mesh_comp.resource();
        if (mesh && mesh->has_blas())
          instances.push_back(
            {mesh->blas().address(), transform.matrix(), tlas_i}
          );
        tlas_i++;
      }
      tlas_[current_frame_] = instances.empty()
        ? nullopt
        : optional{graphics::AccelerationStructure::build_tlas(
            "scene_tlas", instances, cmd_buffer_)};
    }
#endif

    cmd_buffer_.begin_region("passes");

    graph_.render(
      cmd_buffer_,
      *global_data,
      *object_data,
      global_data_buffers_[current_frame_].address(),
      object_data_buffers_[current_frame_].address(),
      current_frame_
#ifdef VK_KHR_acceleration_structure
      , tlas_[current_frame_].has_value()
          ? tlas_[current_frame_]->handle()
          : vk::AccelerationStructureKHR{}
#endif
    );

    cmd_buffer_.end_region();
    cmd_buffer_.begin_region("copy");

    const auto swapchain_image = swapchain_.copy(
      graph_.output_resource().image, cmd_buffer_, available_s_[current_frame_]
    );

    cmd_buffer_.end_region();

    cmd_buffer_->end();

    submit_to_queue(swapchain_image);
    swapchain_.present(swapchain_image, finished_s_[swapchain_image]);

    current_frame_ = (current_frame_ + 1) % constants::MAX_FRAMES_IN_FLIGHT;
  }
}
