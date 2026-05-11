//
// Created by kinami on 4/23/26.
//

#include "ConvolutionalPass.h"

#include "graphics/context.h"
#include "resources/context.h"
#include "systems/render/RenderPassRegistry.h"
#include "systems/render/Rendergraph.h"
#include "utils/xml.h"
#include "window/context.h"

namespace lisa::render {
  REGISTER_RENDER_PASS(ConvolutionalPass)

  void ConvolutionalPass::setup(
    systems::render::Rendergraph& graph, const pugi::xml_node& node
  ) {
    auto [buffer, size] = kernel_buffer(
      utils::xml::parse<vector<float>>(
        node.find_child_by_attribute("parameter", "id", "kernel")
          .text()
          .as_string()
      )
    );
    kernel_buffer_ = std::move(buffer);
    kernel_size_ = size;

    const auto output_format =
      graph
        .get_resource(node.find_child_by_attribute("output", "id", "output")
                        .attribute("ref")
                        .value())
        .format();

    set_shader("postprocess/convolutional.slang");

    graphics::Pipeline::CreateParameters params{
      .push_constant_range =
        vk::PushConstantRange{
          .stageFlags = vk::ShaderStageFlagBits::eVertex |
                        vk::ShaderStageFlagBits::eFragment,
          .offset = 0,
          .size = sizeof(ConvolutionalPushConstants)
        },
      .shader = *shader(),
      .vertex_input = false,
      .color_attachment_formats = vector<vk::Format>{output_format},
      .depth_test_write = false,
    };
    pipeline_ = std::make_unique<graphics::Pipeline>(id(), params);
  }

  void ConvolutionalPass::execute(const systems::render::RenderContext& ctx) {
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

    const ConvolutionalPushConstants push{
      .global_bda = ctx.global_bda,
      .kernel_bda = kernel_buffer_.address(),
      .kernel_size = kernel_size_,
      .input_idx = input_indices_.at("input")
    };

    ctx.cmdb->pushConstants<ConvolutionalPushConstants>(
      pipeline_->layout(),
      vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment,
      0,
      push
    );

    ctx.cmdb->draw(3, 1, 0, 0);
  }

  pair<graphics::Buffer, uint32>
    ConvolutionalPass::kernel_buffer(const vector<float>& kernel) {
    const auto sqrt_size = std::sqrt(static_cast<float>(kernel.size()));
    const auto N = static_cast<int>(std::round(sqrt_size));

    if (N * N != static_cast<int>(kernel.size()))
      throw std::invalid_argument(
        "Vector of size " +
        std::to_string(kernel.size()) +
        " is not a perfect square."
      );

    vector<float> normalized_kernel = kernel;
    float sum = 0.0f;
    for (const float v : normalized_kernel)
      sum += v;
    if (sum != 0.0f)
      for (float& v : normalized_kernel)
        v /= sum;

    return {
      graphics::Buffer::from_data(
        "convolutional_kernel_buffer",
        normalized_kernel.data(),
        normalized_kernel.size() * sizeof(float),
        vk::BufferUsageFlagBits::eTransferDst |
          vk::BufferUsageFlagBits::eShaderDeviceAddress,
        vma::AllocationCreateInfo{.usage = vma::MemoryUsage::eAuto}
      ),
      N
    };
  }
}
