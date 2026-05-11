//
// Created by kinami on 4/19/26.
//

#include "GeometryPass.h"

#include "components/MaterialComponent.h"
#include "components/MeshComponent.h"
#include "components/TransformComponent.h"
#include "components/context.h"
#include "graphics/context.h"
#include "resources/context.h"
#include "systems/render/RenderPassRegistry.h"
#include "systems/render/Rendergraph.h"

namespace lisa::render {
  REGISTER_RENDER_PASS(GeometryPass);

  void GeometryPass::setup(
    systems::render::Rendergraph& graph, const pugi::xml_node& node
  ) {
    auto albedo_fmt =
      graph
        .get_resource(node.find_child_by_attribute("output", "id", "albedo")
                        .attribute("ref")
                        .value())
        .format();
    auto normal_fmt =
      graph
        .get_resource(node.find_child_by_attribute("output", "id", "normal")
                        .attribute("ref")
                        .value())
        .format();
    auto position_fmt =
      graph
        .get_resource(node.find_child_by_attribute("output", "id", "position")
                        .attribute("ref")
                        .value())
        .format();
    auto depth_fmt =
      graph
        .get_resource(node.find_child_by_attribute("input", "id", "depth")
                        .attribute("ref")
                        .value())
        .format();
    auto material_fmt =
      graph
        .get_resource(node.find_child_by_attribute("output", "id", "material")
                        .attribute("ref")
                        .value())
        .format();

    set_shader("deferred/geometry.slang");

    graphics::Pipeline::CreateParameters params{
      .push_constant_range =
        vk::PushConstantRange{
          .stageFlags = vk::ShaderStageFlagBits::eVertex |
                        vk::ShaderStageFlagBits::eFragment,
          .offset = 0,
          .size = sizeof(systems::render::PushConstants)
        },
      .shader = shader()->module(),
      .color_attachment_formats =
        vector<vk::Format>{albedo_fmt, normal_fmt, position_fmt, material_fmt},
      .depth_attachment_format = depth_fmt,
      .depth_compare_op = vk::CompareOp::eLessOrEqual
    };
    pipeline_ = std::make_unique<graphics::Pipeline>(id(), params);
  }

  void GeometryPass::execute(const systems::render::RenderContext& ctx) {
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
