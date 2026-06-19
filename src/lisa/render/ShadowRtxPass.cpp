//
// Created by kinami on 6/18/26.
//

#include "ShadowRtxPass.h"

#ifdef VK_KHR_acceleration_structure

#include "graphics/context.h"
#include "systems/render/RenderPassRegistry.h"
#include "systems/render/Rendergraph.h"

namespace lisa::render {
  REGISTER_RENDER_PASS(ShadowRtxPass);

  void ShadowRtxPass::setup(
    systems::render::Rendergraph& graph, const pugi::xml_node& node
  ) {
    auto output_fmt =
      graph
        .get_resource(node.find_child_by_attribute("output", "id", "shadow")
                        .attribute("ref")
                        .value())
        .format();

    const vk::DescriptorSetLayoutBinding tlas_binding{
      .binding = 0,
      .descriptorType = vk::DescriptorType::eAccelerationStructureKHR,
      .descriptorCount = 1,
      .stageFlags = vk::ShaderStageFlagBits::eFragment,
    };
    tlas_layout_ = graphics::context::device()->createDescriptorSetLayout({
      .bindingCount = 1,
      .pBindings = &tlas_binding,
    });

    const vk::DescriptorPoolSize pool_size{
      .type = vk::DescriptorType::eAccelerationStructureKHR,
      .descriptorCount = 1,
    };
    tlas_pool_ = graphics::context::device()->createDescriptorPool({
      .flags = vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet,
      .maxSets = 1,
      .poolSizeCount = 1,
      .pPoolSizes = &pool_size,
    });

    auto sets = graphics::context::device()->allocateDescriptorSets({
      .descriptorPool = *tlas_pool_,
      .descriptorSetCount = 1,
      .pSetLayouts = &*tlas_layout_,
    });
    tlas_set_ = std::move(sets[0]);

    set_shader("deferred/shadow_rtx.slang");

    const graphics::Pipeline::CreateParameters params{
      .extra_descriptor_set_layout = *tlas_layout_,
      .push_constant_range =
        vk::PushConstantRange{
          .stageFlags = vk::ShaderStageFlagBits::eVertex |
                        vk::ShaderStageFlagBits::eFragment,
          .offset = 0,
          .size = sizeof(ShadowRtxPushConstants)
        },
      .shader = shader()->module(),
      .vertex_input = false,
      .color_attachment_formats = vector<vk::Format>{output_fmt},
      .depth_test_read = false,
      .depth_test_write = false
    };
    pipeline_ = std::make_unique<graphics::Pipeline>(id(), params);
  }

  void ShadowRtxPass::execute(const systems::render::RenderContext& ctx) {
    if (ctx.tlas_handle) {
      vk::WriteDescriptorSetAccelerationStructureKHR as_write{
        .accelerationStructureCount = 1,
        .pAccelerationStructures = &ctx.tlas_handle,
      };
      graphics::context::device()->updateDescriptorSets(
        {vk::WriteDescriptorSet{
          .pNext = &as_write,
          .dstSet = *tlas_set_,
          .dstBinding = 0,
          .descriptorCount = 1,
          .descriptorType = vk::DescriptorType::eAccelerationStructureKHR,
        }},
        {}
      );
    }

    ctx.cmdb->bindPipeline(
      vk::PipelineBindPoint::eGraphics, pipeline_->handle()
    );

    ctx.cmdb->bindDescriptorSets(
      vk::PipelineBindPoint::eGraphics,
      pipeline_->layout(),
      0,
      {*graphics::context::descriptor_container().set(), *tlas_set_},
      {}
    );

    const ShadowRtxPushConstants push{
      .global_bda = ctx.global_bda,
      .position_idx = input_indices_.at("position"),
      .normal_idx = input_indices_.at("normal"),
      .has_tlas = ctx.tlas_handle ? 1u : 0u,
    };

    ctx.cmdb->pushConstants<ShadowRtxPushConstants>(
      pipeline_->layout(),
      vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment,
      0,
      push
    );

    ctx.cmdb->draw(3, 1, 0, 0);
  }
}

#endif // VK_KHR_acceleration_structure
