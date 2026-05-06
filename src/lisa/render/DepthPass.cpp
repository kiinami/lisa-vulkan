//
// Created by kinami on 4/22/26.
//

#include "DepthPass.h"

#include "components/MeshComponent.h"
#include "components/TransformComponent.h"
#include "components/context.h"
#include "graphics/context.h"
#include "resources/context.h"
#include "systems/render/RenderPassRegistry.h"
#include "systems/render/Rendergraph.h"

namespace lisa::render {
  REGISTER_RENDER_PASS(DepthPass);

  void DepthPass::setup(
    systems::render::Rendergraph& graph, const pugi::xml_node& node
  ) {
    const auto depth_fmt =
      graph
        .get_resource(node.find_child_by_attribute("output", "id", "depth")
                        .attribute("ref")
                        .value())
        .format();

    static path shader_path =
      resources::Shader::SHADERS_PATH / "preprocess/depth.slang";
    resources::context::manager().add<resources::Shader, resources::ShaderSpec>(
      shader_path.string(), shader_path
    );
    resources::context::manager().load<resources::Shader>(shader_path.string());
    shader_ = resources::context::manager().get<resources::Shader>(
      shader_path.string()
    );

    graphics::Pipeline::CreateParameters params{
      .push_constant_range =
        vk::PushConstantRange{
          .stageFlags = vk::ShaderStageFlagBits::eVertex,
          .offset = 0,
          .size = sizeof(systems::render::PushConstants)
        },
      .shader = *shader_,
      .position_only = true,
      .depth_test_write = true,
      .depth_attachment_format = depth_fmt
    };
    pipeline_ = std::make_unique<graphics::Pipeline>(params);
  }

  void DepthPass::execute(const systems::render::RenderContext& ctx) {
    ctx.cmdb->bindPipeline(vk::PipelineBindPoint::eGraphics, **pipeline_);
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
        pipeline_->layout(), vk::ShaderStageFlagBits::eVertex, 0, push_constants
      );

      ctx.cmdb->drawIndexed(mesh->index_count(), 1, 0, 0, 0);
      i++;
    }
  }
}
