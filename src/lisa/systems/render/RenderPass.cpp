//
// Created by kinami on 4/3/26.
//

#include "RenderPass.h"

#include "ImageRenderResource.h"
#include "graphics/context.h"
#include "resources/context.h"

namespace lisa::systems::render {
  RenderPass::RenderPass(
    const pugi::xml_node& node,
    const umap<LocalResourceID, const RenderResourceDesc*>& valid_inputs,
    const umap<LocalResourceID, const RenderResourceDesc*>& valid_outputs,
    const str& shader_id
  ) :
    id_(node.attribute("id").value()),
    input_descs_(valid_inputs),
    output_descs_(valid_outputs),
    shader_(
      lisa::resources::context::manager().load<lisa::resources::Shader>(
        shader_id
      )
    ),
    pipeline_(
      std::make_unique<graphics::Pipeline>(
        graphics::context::descriptor_container().layout(),
        vk::PushConstantRange{
          .stageFlags = vk::ShaderStageFlagBits::eVertex |
                        vk::ShaderStageFlagBits::eFragment,
          .offset = 0,
          .size = sizeof(PushConstants)
        },
        *shader_.get(),
        true,
        vk::Format::eR8G8B8A8Unorm
      )
    ) {
    for (const auto& input : node.children("input")) {
      const auto id = input.attribute("id").value();
      auto maps_to = input.attribute("maps_to").value();

      if (!valid_inputs.contains(maps_to))
        logging::abort("maps_to value '{}' not found", maps_to);

      inputs_map_[maps_to] = id;
    }

    for (const auto& output : node.children("output")) {
      const auto id = output.attribute("id").value();
      auto maps_to = output.attribute("maps_to").value();

      if (!valid_outputs.contains(maps_to))
        logging::abort("maps_to value '{}' not found", maps_to);

      outputs_map_[maps_to] = id;
    }
  }

  void RenderPass::render(const RenderContext& ctx) {
    vector<vk::RenderingAttachmentInfo> color_attachments;
    std::optional<vk::RenderingAttachmentInfo> depth_attachment;

    for (const auto& [local_id, global_id] : output_entries()) {
      auto& desc = static_cast<const ImageRenderResourceDesc&>(
        *output_descs().at(local_id)
      );
      auto* img =
        static_cast<const ImageRenderResource*>(ctx.resources.at(local_id));

      vk::RenderingAttachmentInfo info{
        .imageView = img->view(),
        .imageLayout = desc.layout,
        .loadOp = desc.load_op,
        .storeOp = desc.store_op,
        .clearValue = desc.clear_value
      };

      if (desc.type == "depth")
        depth_attachment = info;
      else
        color_attachments.push_back(info);
    }

    const auto extent = graphics::context::swapchain().extent();

    ctx.cmdb->beginRendering(
      {.renderArea = {.extent = extent},
       .layerCount = 1,
       .colorAttachmentCount = static_cast<uint32>(color_attachments.size()),
       .pColorAttachments = color_attachments.data(),
       .pDepthAttachment = depth_attachment ? &*depth_attachment : nullptr}
    );
    ctx.cmdb->setViewport(
      0,
      {{.width = static_cast<float>(extent.width),
        .height = static_cast<float>(extent.height),
        .minDepth = 0.0f,
        .maxDepth = 1.0f}}
    );
    ctx.cmdb->setScissor(0, {{.extent = extent}});

    ctx.cmdb->bindPipeline(vk::PipelineBindPoint::eGraphics, **pipeline_);
    ctx.cmdb->bindDescriptorSets(
      vk::PipelineBindPoint::eGraphics,
      pipeline_->layout(),
      0,
      {*graphics::context::descriptor_container().set()},
      {}
    );

    do_render(ctx);

    ctx.cmdb->endRendering();
  }
}
