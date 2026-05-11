//
// Created by kinami on 4/18/26.
//

#include "ForwardPass.h"

#include "components/MeshComponent.h"
#include "components/TransformComponent.h"
#include "components/context.h"
#include "graphics/context.h"
#include "resources/context.h"
#include "systems/render/RenderPassRegistry.h"
#include "systems/render/Rendergraph.h"

namespace lisa::render {
  REGISTER_RENDER_PASS(ForwardPass);

  void ForwardPass::setup(
    systems::render::Rendergraph& graph, const pugi::xml_node& node
  ) {
    str color_ref = node.find_child_by_attribute("output", "id", "color")
                      .attribute("ref")
                      .value();
    str depth_ref = node.find_child_by_attribute("output", "id", "depth")
                      .attribute("ref")
                      .value();

    auto color_format = graph.get_resource(color_ref).format();
    auto depth_format = graph.get_resource(depth_ref).format();

    set_shader("forward/forward.slang");

    graphics::Pipeline::CreateParameters params{
      .push_constant_range =
        vk::PushConstantRange{
          .stageFlags = vk::ShaderStageFlagBits::eVertex |
                        vk::ShaderStageFlagBits::eFragment,
          .offset = 0,
          .size = sizeof(systems::render::PushConstants)
        },
      .shader = *shader(),
      .color_attachment_formats = vector<vk::Format>{color_format},
      .depth_test_write = true,
      .depth_attachment_format = depth_format
    };
    pipeline_ = std::make_unique<graphics::Pipeline>(id(), params);
  }

  void ForwardPass::execute(const systems::render::RenderContext& ctx) {
    ctx.cmdb->bindPipeline(vk::PipelineBindPoint::eGraphics, pipeline_->handle());
    ctx.cmdb->bindDescriptorSets(
      vk::PipelineBindPoint::eGraphics,
      pipeline_->layout(),
      0,
      {*graphics::context::descriptor_container().set()},
      {}
    );

    const auto view = components::context::registry()
                        ->view<
                          const components::TransformComponent,
                          const components::MeshComponent>();

    uint32 i = 0;
    for (auto [entity, transform, mesh_component] : view.each()) {
      const auto mesh = mesh_component.resource();

      vk::DeviceSize offset = 0;
      ctx.cmdb->bindVertexBuffers(
        0, static_cast<const vk::Buffer&>(mesh->vertex_buffer()), offset
      );
      ctx.cmdb->bindIndexBuffer(
        mesh->index_buffer(), 0, vk::IndexType::eUint32
      );

      auto push_constants = systems::render::PushConstants{
        .global_bda = ctx.global_bda,
        .object_bda = ctx.object_bda + i * sizeof(systems::render::ObjectData)
      };

      ctx.cmdb->pushConstants<systems::render::PushConstants>(
        pipeline_->layout(),
        vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment,
        0,
        push_constants
      );

      ctx.cmdb->drawIndexed(mesh->index_count(), 1, 0, 0, 0);
      i++;
    }
  }
}
