//
// Created by kinami on 4/19/26.
//

#include "CompositionPass.h"

#include "graphics/context.h"
#include "resources/context.h"
#include "systems/render/RenderPassRegistry.h"
#include "systems/render/Rendergraph.h"

namespace lisa::render {
  REGISTER_RENDER_PASS(CompositionPass);

  void CompositionPass::setup(
    systems::render::Rendergraph& graph, const pugi::xml_node& node
  ) {
    auto final_fmt =
      graph
        .get_resource(node.find_child_by_attribute("output", "id", "final")
                        .attribute("ref")
                        .value())
        .format();

    shader_ = resources::context::manager().load<resources::Shader>(
      "composition.deferred"
    );

    pipeline_ = std::make_unique<graphics::Pipeline>(
      graphics::context::descriptor_container().layout(),
      vk::PushConstantRange{
        .stageFlags =
          vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment,
        .offset = 0,
        .size = sizeof(CompositionPushConstants)
      },
      *shader_.get(),
      false,
      vector<vk::Format>{final_fmt},
      vk::Format::eUndefined,
      true
    );

    sampler_ = std::make_unique<graphics::Sampler>(
      1.0f, vk::Filter::eNearest, vk::Filter::eNearest
    );

    auto register_image = [&](const str& id) {
      const str ref = node.find_child_by_attribute("input", "id", id.c_str())
                        .attribute("ref")
                        .value();
      const auto& image = graph.get_resource(ref);
      const vk::DescriptorImageInfo img_info{
        **sampler_,
        *image.image.view(
          {vk::ImageViewType::e2D,
           image.format(),
           {vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1}}
        ),
        vk::ImageLayout::eShaderReadOnlyOptimal
      };
      return graphics::context::descriptor_container().write(img_info);
    };

    albedo_idx_ = register_image("albedo");
    normal_idx_ = register_image("normal");
    position_idx_ = register_image("position");
    material_idx_ = register_image("material");
  }

  void CompositionPass::execute(const systems::render::RenderContext& ctx) {
    ctx.cmdb->bindPipeline(vk::PipelineBindPoint::eGraphics, **pipeline_);

    ctx.cmdb->bindDescriptorSets(
      vk::PipelineBindPoint::eGraphics,
      pipeline_->layout(),
      0,
      {*graphics::context::descriptor_container().set()},
      {}
    );

    const CompositionPushConstants push{
      .global_bda = ctx.global_bda,
      .albedo_idx = albedo_idx_,
      .normal_idx = normal_idx_,
      .position_idx = position_idx_,
      .material_idx = material_idx_
    };

    ctx.cmdb->pushConstants<CompositionPushConstants>(
      pipeline_->layout(),
      vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment,
      0,
      push
    );

    ctx.cmdb->draw(3, 1, 0, 0);
  }
}
