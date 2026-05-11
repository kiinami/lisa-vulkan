//
// Created by kinami on 4/23/26.
//

#include "SSAOPass.h"

#include "components/MeshComponent.h"
#include "components/TransformComponent.h"
#include "graphics/context.h"
#include "resources/context.h"
#include "systems/render/RenderPassRegistry.h"
#include "systems/render/Rendergraph.h"

#include <random>

namespace lisa::render {
  REGISTER_RENDER_PASS(SSAOPass)

  SSAOPass::~SSAOPass() {
    graphics::context::descriptor_container().free(noise_idx_);
  }

  void SSAOPass::setup(
    systems::render::Rendergraph& graph, const pugi::xml_node& node
  ) {
    kernel_buffer_ = generate_kernel(64);
    noise_image_ = generate_noise(4);

    const auto ssao_fmt =
      graph
        .get_resource(node.find_child_by_attribute("output", "id", "ssao")
                        .attribute("ref")
                        .value())
        .format();

    set_shader("preprocess/ssao.slang");

    graphics::Pipeline::CreateParameters params{
      .push_constant_range =
        vk::PushConstantRange{
          .stageFlags = vk::ShaderStageFlagBits::eVertex |
                        vk::ShaderStageFlagBits::eFragment,
          .offset = 0,
          .size = sizeof(SSAOPushConstants)
        },
      .shader = *shader(),
      .vertex_input = false,
      .color_attachment_formats = vector<vk::Format>{ssao_fmt},
      .depth_test_write = false,
    };
    pipeline_ = std::make_unique<graphics::Pipeline>(id(), params);

    noise_sampler_ = std::make_unique<graphics::Sampler>(
      logging::genid(id(), "noise"),
      1.0f,
      vk::Filter::eNearest,
      vk::Filter::eNearest
    );
    const vk::DescriptorImageInfo image_info{
      noise_sampler_->handle(),
      *noise_image_.view(
        {.type = vk::ImageViewType::e2D,
         .format = noise_image_.format(),
         .range = {vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1}}
      ),
      vk::ImageLayout::eReadOnlyOptimal
    };
    noise_idx_ = graphics::context::descriptor_container().write(image_info);
  }

  void SSAOPass::execute(const systems::render::RenderContext& ctx) {
    ctx.cmdb->bindPipeline(vk::PipelineBindPoint::eGraphics, pipeline_->handle());

    ctx.cmdb->bindDescriptorSets(
      vk::PipelineBindPoint::eGraphics,
      pipeline_->layout(),
      0,
      {*graphics::context::descriptor_container().set()},
      {}
    );

    const SSAOPushConstants push{
      .global_bda = ctx.global_bda,
      .samples_bda = kernel_buffer_.address(),
      .depth_idx = input_indices_.at("depth"),
      .normal_idx = input_indices_.at("normal"),
      .position_idx = input_indices_.at("position"),
      .noise_idx = noise_idx_
    };

    ctx.cmdb->pushConstants<SSAOPushConstants>(
      pipeline_->layout(),
      vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment,
      0,
      push
    );

    ctx.cmdb->draw(3, 1, 0, 0);
  }

  graphics::Buffer SSAOPass::generate_kernel(const uint32 size) {
    vector<vec4> kernel;

    std::uniform_real_distribution random_floats(0.0f, 1.0f);
    std::default_random_engine generator;

    for (uint32 i = 0; i < size; i++) {
      vec3 sample{
        random_floats(generator) * 2.0f - 1.0f,
        random_floats(generator) * 2.0f - 1.0f,
        random_floats(generator)
      };

      sample = glm::normalize(sample);
      sample *= random_floats(generator);

      float scale = static_cast<float>(i) / 64.0f;
      scale = glm::mix(0.1f, 1.0f, scale * scale);
      sample *= scale;

      kernel.push_back(vec4(sample, 0.0f));
    }

    return graphics::Buffer::from_data(
      "ssao_kernel_buffer",
      kernel.data(),
      kernel.size() * sizeof(vec4),
      vk::BufferUsageFlagBits::eTransferDst |
        vk::BufferUsageFlagBits::eShaderDeviceAddress,
      vma::AllocationCreateInfo{.usage = vma::MemoryUsage::eAuto}
    );
  }

  graphics::Image SSAOPass::generate_noise(const uint32 size) {
    vector<vec4> noise;

    std::uniform_real_distribution random_floats(0.0f, 1.0f);
    std::default_random_engine generator;

    for (uint32 i = 0; i < size * size; i++) {
      vec3 n(
        random_floats(generator) * 2.0f - 1.0f,
        random_floats(generator) * 2.0f - 1.0f,
        0.0f
      );
      n = glm::normalize(n);
      noise.push_back(vec4(n, 0.0f));
    }

    return graphics::Image::from_data(
      "ssao_noise_texture",
      noise.data(),
      noise.size() * sizeof(vec4),
      vec3{4, 4, 1},
      graphics::ImageFormat(vk::Format::eR32G32B32A32Sfloat),
      vk::ImageUsageFlagBits::eSampled
    );
  }
}
