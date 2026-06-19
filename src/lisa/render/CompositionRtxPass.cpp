//
// Created by kinami on 6/18/26.
//

#include "CompositionRtxPass.h"

#include "graphics/context.h"
#include "systems/render/RenderPassRegistry.h"
#include "systems/render/Rendergraph.h"

namespace lisa::render {
  REGISTER_RENDER_PASS(CompositionRtxPass);

  void CompositionRtxPass::setup(
    systems::render::Rendergraph& graph, const pugi::xml_node& node
  ) {
    auto final_fmt =
      graph
        .get_resource(node.find_child_by_attribute("output", "id", "final")
                        .attribute("ref")
                        .value())
        .format();

    set_shader("deferred/composition_rtx.slang");

    const graphics::Pipeline::CreateParameters params{
      .push_constant_range =
        vk::PushConstantRange{
          .stageFlags = vk::ShaderStageFlagBits::eVertex |
                        vk::ShaderStageFlagBits::eFragment,
          .offset = 0,
          .size = sizeof(CompositionRtxPushConstants)
        },
      .shader = shader()->module(),
      .vertex_input = false,
      .color_attachment_formats = vector<vk::Format>{final_fmt},
      .depth_test_read = false,
      .depth_test_write = false
    };
    pipeline_ = std::make_unique<graphics::Pipeline>(id(), params);
  }

  void CompositionRtxPass::execute(const systems::render::RenderContext& ctx) {
    ctx.cmdb->bindPipeline(
      vk::PipelineBindPoint::eGraphics, pipeline_->handle()
    );

    ctx.cmdb->bindDescriptorSets(
      vk::PipelineBindPoint::eGraphics,
      pipeline_->layout(),
      0,
      {*graphics::context::descriptor_container().set()},
      {}
    );

    const CompositionRtxPushConstants push{
      .global_bda = ctx.global_bda,
      .albedo_idx = input_indices_.at("albedo"),
      .normal_idx = input_indices_.at("normal"),
      .position_idx = input_indices_.at("position"),
      .material_idx = input_indices_.at("material"),
      .ssao_idx = input_indices_.contains("ssao")
                    ? input_indices_.at("ssao")
                    : std::numeric_limits<uint32>::max(),
      .shadow_idx = input_indices_.at("shadow"),
    };

    ctx.cmdb->pushConstants<CompositionRtxPushConstants>(
      pipeline_->layout(),
      vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment,
      0,
      push
    );

    ctx.cmdb->draw(3, 1, 0, 0);
  }
}
