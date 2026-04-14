//
// Created by kinami on 4/5/26.
//

#include "DefaultPass.h"

#include "components/MeshComponent.h"
#include "components/TextureComponent.h"
#include "components/TransformComponent.h"
#include "components/context.h"
#include "graphics/context.h"
#include "resources/Mesh.h"
#include "resources/Texture.h"
#include "resources/context.h"

namespace lisa::passes {
  DefaultPass::DefaultPass(const str& name) : RenderPass(name) {
    add_output(
      {.id = "FinalTarget",
       .layout = vk::ImageLayout::eColorAttachmentOptimal,
       .access = vk::AccessFlagBits::eColorAttachmentWrite,
       .stage = vk::PipelineStageFlagBits::eColorAttachmentOutput,
       .aspect = vk::ImageAspectFlagBits::eColor}
    );
    add_output(
      {.id = "DepthTarget",
       .layout = vk::ImageLayout::eDepthAttachmentOptimal,
       .access = vk::AccessFlagBits::eDepthStencilAttachmentWrite,
       .stage = vk::PipelineStageFlagBits::eEarlyFragmentTests |
                vk::PipelineStageFlagBits::eLateFragmentTests,
       .aspect = vk::ImageAspectFlagBits::eDepth}
    );

    const auto shader =
      resources::context::manager().load<resources::Shader>("shader");

    descriptor_container_ = std::make_unique<graphics::DescriptorContainer>(
      1000, vk::DescriptorType::eCombinedImageSampler
    );

    pipeline_ = std::make_unique<graphics::Pipeline>(
      descriptor_container_->layout(),
      vk::PushConstantRange{
        .stageFlags =
          vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment,
        .offset = 0,
        .size = sizeof(systems::render::PushConstants)
      },
      *shader.get(),
      true,
      vk::Format::eR8G8B8A8Unorm
    );
  }

  void DefaultPass::render(const RenderPassInput& in) {
    vk::RenderingAttachmentInfo color_attachment{
      .imageView = in.image_views.at("FinalTarget"),
      .imageLayout = vk::ImageLayout::eColorAttachmentOptimal,
      .loadOp = vk::AttachmentLoadOp::eClear,
      .storeOp = vk::AttachmentStoreOp::eStore,
      .clearValue = {.color = {std::array{0.0f, 0.0f, 0.0f, 1.0f}}}
    };
    vk::RenderingAttachmentInfo depth_attachment{
      .imageView = in.image_views.at("DepthTarget"),
      .imageLayout = vk::ImageLayout::eDepthAttachmentOptimal,
      .loadOp = vk::AttachmentLoadOp::eClear,
      .storeOp = vk::AttachmentStoreOp::eStore,
      .clearValue = {.depthStencil = {1.0f, 0}}
    };

    in.cmd->beginRendering(
      {.renderArea = {.extent = in.extent},
       .layerCount = 1,
       .colorAttachmentCount = 1,
       .pColorAttachments = &color_attachment,
       .pDepthAttachment = &depth_attachment}
    );

    in.cmd->setViewport(
      0,
      {{.width = static_cast<float>(in.width),
        .height = static_cast<float>(in.height),
        .minDepth = 0.0f,
        .maxDepth = 1.0f}}
    );
    in.cmd->setScissor(0, {{.extent = in.extent}});

    in.cmd->bindPipeline(vk::PipelineBindPoint::eGraphics, **pipeline_);
    in.cmd->bindDescriptorSets(
      vk::PipelineBindPoint::eGraphics,
      pipeline_->layout(),
      0,
      {*descriptor_container_->set()},
      {}
    );

    auto view = components::context::registry()
                  ->view<
                    const components::MeshComponent,
                    const components::TextureComponent>();

    uint32 i = 0;
    for (auto [entity, mesh_component, texture_component] : view.each()) {
      const auto mesh = mesh_component.resource();
      const auto texture = texture_component.resource();

      vk::DeviceSize offset = 0;
      in.cmd->bindVertexBuffers(0, {mesh->vertex_buffer()}, offset);
      in.cmd->bindIndexBuffer(
        mesh->index_buffer(), mesh->index_offset(), vk::IndexType::eUint16
      );

      vk::DescriptorImageInfo image_info{
        .sampler = *texture->sampler(),
        .imageView = *texture->image().view(
          {.type = vk::ImageViewType::e2D,
           .format = texture->image().format(),
           .range =
             {.aspectMask = vk::ImageAspectFlagBits::eColor,
              .baseMipLevel = 0,
              .levelCount = texture->image().mipmaps(),
              .baseArrayLayer = 0,
              .layerCount = 1}}
        ),
        .imageLayout = vk::ImageLayout::eReadOnlyOptimal
      };

      vk::WriteDescriptorSet write_desc{
        .dstSet = descriptor_container_->set(),
        .dstBinding = 0,
        .dstArrayElement = i,
        .descriptorCount = 1,
        .descriptorType = vk::DescriptorType::eCombinedImageSampler,
        .pImageInfo = &image_info
      };
      graphics::context::device()->updateDescriptorSets(write_desc, nullptr);

      auto push_constants = systems::render::PushConstants{
        .global_bda = in.global_bda,
        .object_bda = in.object_bda + i * sizeof(systems::render::ObjectData)
      };

      in.cmd->pushConstants<systems::render::PushConstants>(
        pipeline_->layout(),
        vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment,
        0,
        push_constants
      );

      in.cmd->drawIndexed(mesh->index_count(), 1, 0, 0, 0);
      i++;
    }

    in.cmd->endRendering();
  }
}
