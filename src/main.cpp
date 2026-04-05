//
// Created by kinami on 1/26/26.
//

#include "lisa/graphics/context.h"
#include "lisa/graphics/descriptors/DescriptorContainer.h"
#include "lisa/graphics/pipeline/Pipeline.h"
#include "lisa/resources/Mesh.h"
#include "lisa/resources/Shader.h"
#include "lisa/resources/Texture.h"
#include "lisa/utils/logging.h"
#include "systems/render/Renderer.h"
#include "systems/render/Rendergraph.h"
#include "systems/resources/ResourceManager.h"
#include "window/Window.h"
#include "window/context.h"

#include <CLI/CLI.hpp>
#include <glm/gtc/matrix_transform.hpp>

using namespace lisa;

namespace {
  CLI::App app{"lisa"};
  str log_level = "debug";
  int device = 0;
}

static int cli_args(int argc, char** argv) {
  argv = app.ensure_utf8(argv);

  app
    .add_option(
      "-l,--log-level", log_level, "The logging level of the application"
    )
    ->check(
      CLI::IsMember(
        {"trace", "debug", "info", "warning", "error", "critical"},
        CLI::ignore_case
      )
    );

  app.add_option("-d,--device", device, "The GPU device to use");

  CLI11_PARSE(app, argc, argv);
  return 0;
}

int main(int argc, char** argv) {
  auto result = cli_args(argc, argv);
  if (result != 0) return result;

  logging::init(log_level);

  {
    window::context::init(1280, 720);
    graphics::context::init();
    logging::info("After graphics::context::init");

    {
      systems::resources::ResourceManager resource_manager;
      logging::info("Loading textures...");

      auto t0 = resource_manager.load<resources::Texture>("suzanne0");
      auto t1 = resource_manager.load<resources::Texture>("suzanne1");
      auto t2 = resource_manager.load<resources::Texture>("suzanne2");
      logging::info("Textures loaded.");

      auto mesh = resource_manager.load<resources::Mesh>("suzanne");
      logging::info("Mesh loaded.");

      auto shader = resource_manager.load<resources::Shader>("shader");
      logging::info("Shader loaded.");

      auto rendergraph = systems::render::Rendergraph();
      rendergraph.add_resource(
        {"ColorTarget",
         vk::Format::eR8G8B8A8Unorm,
         {window::context::window_width(), window::context::window_height(), 1},
         vk::ImageUsageFlagBits::eColorAttachment |
           vk::ImageUsageFlagBits::eTransferSrc}
      );
      rendergraph.add_resource(
        {"DepthTarget",
         vk::Format::eD32Sfloat,
         {window::context::window_width(), window::context::window_height(), 1},
         vk::ImageUsageFlagBits::eDepthStencilAttachment}
      );

      auto descriptor_container = graphics::DescriptorContainer(
        3, vk::DescriptorType::eCombinedImageSampler
      );
      vector<vk::DescriptorImageInfo> image_infos{
        vk::DescriptorImageInfo{
          .sampler = *t0->sampler(),
          .imageView = *t0->image().view(
            {.type = vk::ImageViewType::e2D,
             .format = t0->image().format(),
             .range =
               {.aspectMask = vk::ImageAspectFlagBits::eColor,
                .baseMipLevel = 0,
                .levelCount = t0->image().mipmaps(),
                .baseArrayLayer = 0,
                .layerCount = 1}}
          ),
          .imageLayout = vk::ImageLayout::eReadOnlyOptimal
        },
        vk::DescriptorImageInfo{
          .sampler = *t1->sampler(),
          .imageView = *t1->image().view(
            {.type = vk::ImageViewType::e2D,
             .format = t1->image().format(),
             .range =
               {.aspectMask = vk::ImageAspectFlagBits::eColor,
                .baseMipLevel = 0,
                .levelCount = t1->image().mipmaps(),
                .baseArrayLayer = 0,
                .layerCount = 1}}
          ),
          .imageLayout = vk::ImageLayout::eReadOnlyOptimal
        },
        vk::DescriptorImageInfo{
          .sampler = *t2->sampler(),
          .imageView = *t2->image().view(
            {.type = vk::ImageViewType::e2D,
             .format = t2->image().format(),
             .range =
               {.aspectMask = vk::ImageAspectFlagBits::eColor,
                .baseMipLevel = 0,
                .levelCount = t2->image().mipmaps(),
                .baseArrayLayer = 0,
                .layerCount = 1}}
          ),
          .imageLayout = vk::ImageLayout::eReadOnlyOptimal
        }
      };

      vk::WriteDescriptorSet write_desc{
        .dstSet = descriptor_container.set(),
        .dstBinding = 0,
        .dstArrayElement = 0,
        .descriptorCount = static_cast<uint32>(image_infos.size()),
        .descriptorType = vk::DescriptorType::eCombinedImageSampler,
        .pImageInfo = image_infos.data()
      };
      graphics::context::device()->updateDescriptorSets(write_desc, nullptr);

      auto pipeline = graphics::Pipeline(
        descriptor_container.layout(),
        vk::PushConstantRange{
          .stageFlags = vk::ShaderStageFlagBits::eVertex |
                        vk::ShaderStageFlagBits::eFragment,
          .offset = 0,
          .size = sizeof(systems::render::PushConstants)
        },
        *shader.get(),
        true,
        vk::Format::eR8G8B8A8Unorm
      );

      std::unique_ptr<systems::render::Renderer> renderer;

      auto pass_render_function =
        [&](const systems::render::RenderPass::RenderPassInput& in) {
          vk::RenderingAttachmentInfo color_attachment{
            .imageView = in.image_views.at("ColorTarget"),
            .imageLayout = vk::ImageLayout::eColorAttachmentOptimal,
            .loadOp = vk::AttachmentLoadOp::eClear,
            .storeOp = vk::AttachmentStoreOp::eStore,
            .clearValue = {
              .color = {std::array<float, 4>{0.0f, 0.0f, 0.0f, 1.0f}}
            }
          };
          vk::RenderingAttachmentInfo depth_attachment{
            .imageView = in.image_views.at("DepthTarget"),
            .imageLayout = vk::ImageLayout::eDepthAttachmentOptimal,
            .loadOp = vk::AttachmentLoadOp::eClear,
            .storeOp = vk::AttachmentStoreOp::eStore,
            .clearValue = {.depthStencil = {1.0f, 0}}
          };

          in.cmd_buffer->beginRendering(
            {.renderArea = {.extent = in.extent},
             .layerCount = 1,
             .colorAttachmentCount = 1,
             .pColorAttachments = &color_attachment,
             .pDepthAttachment = &depth_attachment}
          );

          in.cmd_buffer->setViewport(
            0,
            {{.width = static_cast<float>(in.width),
              .height = static_cast<float>(in.height),
              .minDepth = 0.0f,
              .maxDepth = 1.0f}}
          );
          in.cmd_buffer->setScissor(0, {{.extent = in.extent}});

          vk::DeviceSize offset = 0;
          in.cmd_buffer->bindVertexBuffers(0, {mesh->vertex_buffer()}, offset);
          in.cmd_buffer->bindIndexBuffer(
            mesh->index_buffer(), mesh->index_offset(), vk::IndexType::eUint16
          );

          in.cmd_buffer->bindPipeline(
            vk::PipelineBindPoint::eGraphics, *pipeline
          );
          in.cmd_buffer->bindDescriptorSets(
            vk::PipelineBindPoint::eGraphics,
            pipeline.layout(),
            0,
            {*descriptor_container.set()},
            {}
          );

          auto global_bda = renderer->global_view_buffer().address();
          auto object_bda = renderer->object_data_buffer().address();

          for (uint32 i = 0; i < 3; i++) {
            auto push_constants = systems::render::PushConstants{
              .global_bda = global_bda,
              .object_bda = object_bda + i * sizeof(systems::render::ObjectData)
            };

            in.cmd_buffer->pushConstants<systems::render::PushConstants>(
              pipeline.layout(),
              vk::ShaderStageFlagBits::eVertex |
                vk::ShaderStageFlagBits::eFragment,
              0,
              push_constants
            );

            in.cmd_buffer->drawIndexed(mesh->index_count(), 1, 0, 0, 0);
          }

          in.cmd_buffer->endRendering();
        };

      vector<systems::render::RenderPass::ResourceUsage> pass_outputs = {
        {.id = "ColorTarget",
         .layout = vk::ImageLayout::eColorAttachmentOptimal,
         .access = vk::AccessFlagBits::eColorAttachmentWrite,
         .stage = vk::PipelineStageFlagBits::eColorAttachmentOutput,
         .aspect = vk::ImageAspectFlagBits::eColor},
        {.id = "DepthTarget",
         .layout = vk::ImageLayout::eDepthAttachmentOptimal,
         .access = vk::AccessFlagBits::eDepthStencilAttachmentWrite,
         .stage = vk::PipelineStageFlagBits::eEarlyFragmentTests |
                  vk::PipelineStageFlagBits::eLateFragmentTests,
         .aspect = vk::ImageAspectFlagBits::eDepth}
      };

      auto pass = systems::render::RenderPass(
        "GeometryPass", pass_render_function, {}, pass_outputs
      );
      rendergraph.add_pass(pass);
      rendergraph.compile();

      renderer =
        std::make_unique<systems::render::Renderer>(std::move(rendergraph));

      float time = 0.0f;
      int frame_count = 0;
      while (!window::context::should_close()) {
        frame_count++;
        window::context::poll_events();

        // Update GlobalViewData
        auto* global_data = static_cast<systems::render::GlobalViewData*>(
          renderer->global_view_buffer().mapped_data()
        );
        global_data->view = glm::translate(mat4(1.0f), vec3(0.0f, 0.0f, -5.0f));
        global_data->projection = glm::perspective(
          glm::radians(45.0f),
          static_cast<float>(window::context::window_width()) /
            static_cast<float>(window::context::window_height()),
          0.1f,
          100.0f
        );
        global_data->view_projection =
          global_data->projection * global_data->view;
        global_data->camera_position = vec4(0.0f, 0.0f, -5.0f, 1.0f);

        // Update ObjectData
        auto* object_data = static_cast<systems::render::ObjectData*>(
          renderer->object_data_buffer().mapped_data()
        );

        for (uint32 i = 0; i < 3; i++) {
          float x_offset = (i - 1.0f) * 2.0f;
          object_data[i].model =
            glm::translate(mat4(1.0f), vec3(x_offset, 0.0f, 0.0f)) *
            glm::rotate(mat4(1.0f), time, vec3(0.0f, 1.0f, 0.0f));
          object_data[i].color = vec4(1.0f);
          object_data[i].texture_index = i;
        }

        renderer->render();
        time += 0.016f;
      }
      graphics::context::device()->waitIdle();
    }

    graphics::context::destroy();
    window::context::destroy();
  }

  return 0;
}
