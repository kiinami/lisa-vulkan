//
// Created by kinami on 4/23/26.
//

#ifndef LISA_VULKAN_SSAOPASS_H
#define LISA_VULKAN_SSAOPASS_H
#pragma once
#include "graphics/buffer/Buffer.h"
#include "graphics/pipeline/Pipeline.h"
#include "systems/render/RenderPass.h"

namespace lisa::render {

  struct SSAOPushConstants {
    uint64 global_bda;
    uint64 samples_bda;
    DescriptorIndex depth_idx;
    DescriptorIndex normal_idx;
    DescriptorIndex position_idx;
    DescriptorIndex noise_idx;
  };

  class SSAOPass : public systems::render::RenderPass {
  public:
    static constexpr auto TYPE_ID = "preprocess::ssao";

    explicit SSAOPass(const pugi::xml_node& node) : RenderPass(node) {}

    ~SSAOPass() override;

    void setup(
      systems::render::Rendergraph& graph, const pugi::xml_node& node
    ) override;
    void execute(const systems::render::RenderContext& ctx) override;

  private:
    const resources::Shader* shader_;
    uptr<graphics::Pipeline> pipeline_;
    graphics::Buffer kernel_buffer_;
    graphics::Image noise_image_;
    uptr<graphics::Sampler> noise_sampler_;
    DescriptorIndex noise_idx_ = 0;

    static graphics::Buffer generate_kernel(uint32 size);
    static graphics::Image generate_noise(uint32 size);
  };

}

#endif // LISA_VULKAN_SSAOPASS_H
