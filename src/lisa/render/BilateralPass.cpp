//
// Created by kinami on 6/18/26.
//

#include "BilateralPass.h"

#include "graphics/context.h"
#include "systems/render/RenderPassRegistry.h"
#include "systems/render/Rendergraph.h"
#include "utils/xml.h"

namespace lisa::render {
  REGISTER_RENDER_PASS(BilateralPass);

  void BilateralPass::setup(
    systems::render::Rendergraph& graph, const pugi::xml_node& node
  ) {
    auto parse_param = [&](const char* id, float default_val) -> float {
      const auto child = node.find_child_by_attribute("parameter", "id", id);
      if (!child.empty())
        return utils::xml::parse<float>(child.text().as_string());
      return default_val;
    };

    sigma_spatial_ = parse_param("sigma_spatial", 200.0f);
    sigma_depth_ = parse_param("sigma_depth", 0.5f);
    sigma_normal_ = parse_param("sigma_normal", 8.0f);

    const auto output_fmt =
      graph
        .get_resource(node.find_child_by_attribute("output", "id", "output")
                        .attribute("ref")
                        .value())
        .format();

    set_shader("postprocess/bilateral.slang");

    graphics::Pipeline::CreateParameters params{
      .push_constant_range =
        vk::PushConstantRange{
          .stageFlags = vk::ShaderStageFlagBits::eVertex |
                        vk::ShaderStageFlagBits::eFragment,
          .offset = 0,
          .size = sizeof(BilateralPushConstants)
        },
      .shader = shader()->module(),
      .vertex_input = false,
      .color_attachment_formats = vector<vk::Format>{output_fmt},
      .depth_test_read = false,
      .depth_test_write = false,
    };
    pipeline_ = std::make_unique<graphics::Pipeline>(id(), params);
  }

  void BilateralPass::execute(const systems::render::RenderContext& ctx) {
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

    const BilateralPushConstants push{
      .global_bda = ctx.global_bda,
      .shadow_idx = input_indices_.at("shadow"),
      .position_idx = input_indices_.at("position"),
      .normal_idx = input_indices_.at("normal"),
      .sigma_spatial = sigma_spatial_,
      .sigma_depth = sigma_depth_,
      .sigma_normal = sigma_normal_,
    };

    ctx.cmdb->pushConstants<BilateralPushConstants>(
      pipeline_->layout(),
      vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment,
      0,
      push
    );

    ctx.cmdb->draw(3, 1, 0, 0);
  }
}
