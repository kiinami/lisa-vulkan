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
      "deferred/composition.slang"
    );

    const graphics::Pipeline::CreateParameters params{
      .push_constant_range =
        vk::PushConstantRange{
          .stageFlags = vk::ShaderStageFlagBits::eVertex |
                        vk::ShaderStageFlagBits::eFragment,
          .offset = 0,
          .size = sizeof(CompositionPushConstants)
        },
      .shader = *shader_.get(),
      .vertex_input = false,
      .color_attachment_formats = vector<vk::Format>{final_fmt},
    };
    pipeline_ = std::make_unique<graphics::Pipeline>(params);
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
      .albedo_idx = input_indices_.at("albedo"),
      .normal_idx = input_indices_.at("normal"),
      .position_idx = input_indices_.at("position"),
      .material_idx = input_indices_.at("material"),
      .ssao_idx = input_indices_.at("ssao")
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
