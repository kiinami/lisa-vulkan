//
// Created by kinami on 4/18/26.
//

#include "ForwardPass.h"

#include "components/MeshComponent.h"
#include "components/TextureComponent.h"
#include "components/TransformComponent.h"
#include "components/context.h"
#include "systems/render/RenderPassRegistry.h"

namespace lisa::render::passes {
  REGISTER_RENDER_PASS(ForwardPass);

  void ForwardPass::do_render(const systems::render::RenderContext& ctx) {
    const auto view = components::context::registry()
                        ->view<
                          const components::TransformComponent,
                          const components::MeshComponent,
                          const components::TextureComponent>();

    uint32 i = 0;
    for (auto [entity, transform, mesh_component, texture_component] :
         view.each()) {
      const auto mesh = mesh_component.resource();

      vk::DeviceSize offset = 0;
      ctx.cmdb->bindVertexBuffers(
        0, static_cast<const vk::Buffer&>(mesh->vertex_buffer()), offset
      );
      ctx.cmdb->bindIndexBuffer(
        mesh->index_buffer(), mesh->index_offset(), vk::IndexType::eUint16
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
