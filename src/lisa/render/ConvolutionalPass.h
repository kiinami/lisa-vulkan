//
// Created by kinami on 4/23/26.
//

#ifndef LISA_VULKAN_CONVOLUTIONALPASS_H
#define LISA_VULKAN_CONVOLUTIONALPASS_H
#pragma once
#include "graphics/buffer/Buffer.h"
#include "graphics/pipeline/Pipeline.h"
#include "systems/render/RenderPass.h"
#include "systems/resources/ResourceHandle.h"

namespace lisa::render {

  struct ConvolutionalPushConstants {
    uint64 global_bda;
    uint64 kernel_bda;
    uint32 kernel_size;
    DescriptorIndex input_idx;
  };

  class ConvolutionalPass : public systems::render::RenderPass {
  public:
    static constexpr auto TYPE_ID = "postprocess::convolutional";

    explicit ConvolutionalPass(const pugi::xml_node& node) : RenderPass(node) {}

    ~ConvolutionalPass() override = default;

    void setup(
      systems::render::Rendergraph& graph, const pugi::xml_node& node
    ) override;
    void execute(const systems::render::RenderContext& ctx) override;

  private:
    systems::resources::ResourceHandle<resources::Shader> shader_;
    uptr<graphics::Pipeline> pipeline_;
    graphics::Buffer kernel_buffer_;
    uint32 kernel_size_;

    static pair<graphics::Buffer, uint32>
      kernel_buffer(const vector<float>& kernel);
  };

}

#endif // LISA_VULKAN_CONVOLUTIONALPASS_H
