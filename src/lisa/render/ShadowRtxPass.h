//
// Created by kinami on 6/18/26.
//

#ifndef LISA_VULKAN_SHADOWRTXPASS_H
#define LISA_VULKAN_SHADOWRTXPASS_H
#pragma once

#ifdef VK_KHR_acceleration_structure

#include "ShaderPass.h"
#include "constants.h"
#include "graphics/pipeline/Pipeline.h"
#include "systems/render/RenderPass.h"

#include <vulkan/vulkan_raii.hpp>

namespace lisa::render {
  struct ShadowRtxPushConstants {
    uint64 global_bda;
    DescriptorIndex position_idx;
    DescriptorIndex normal_idx;
    uint32 has_tlas;
  };

  class ShadowRtxPass : public ShaderPass {
  public:
    static constexpr auto TYPE_ID = "deferred::shadow_rtx";

    explicit ShadowRtxPass(const pugi::xml_node& node) : ShaderPass(node) {}

    void setup(
      systems::render::Rendergraph& graph, const pugi::xml_node& node
    ) override;
    void execute(const systems::render::RenderContext& ctx) override;

  private:
    uptr<graphics::Pipeline> pipeline_;
    vk::raii::DescriptorSetLayout tlas_layout_ = nullptr;
    vk::raii::DescriptorPool tlas_pool_ = nullptr;
    vector<vk::raii::DescriptorSet> tlas_sets_;
    vector<bool> tlas_sets_valid_;
  };

}

#endif // VK_KHR_acceleration_structure
#endif // LISA_VULKAN_SHADOWRTXPASS_H
