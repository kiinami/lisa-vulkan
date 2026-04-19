//
// Created by kinami on 4/18/26.
//

#ifndef LISA_VULKAN_DEPTHRENDERRESOURCE_H
#define LISA_VULKAN_DEPTHRENDERRESOURCE_H
#include "systems/render/ImageRenderResource.h"

namespace lisa::render::resources {

  class DepthRenderResource : public systems::render::ImageRenderResource {
  public:
    static constexpr auto TYPE_ID = "depth";
    static constexpr auto FORMAT = vk::Format::eD32Sfloat;
    static constexpr auto USAGE =
      vk::ImageUsageFlagBits::eDepthStencilAttachment;
    static constexpr auto ASPECT = vk::ImageAspectFlagBits::eDepth;

    explicit DepthRenderResource(const str& id, const vec3& size) :
      ImageRenderResource(id, FORMAT, USAGE, size) {}

    ~DepthRenderResource() override = default;

    str type() const override { return TYPE_ID; }

    vk::ImageAspectFlags aspect() override { return ASPECT; }

    vk::RenderingAttachmentInfo rendering_attachment();
  };

  static const systems::render::ImageRenderResourceDesc depth_attachment = {
    "depth",
    vk::ImageLayout::eDepthStencilAttachmentOptimal,
    vk::AccessFlagBits2::eDepthStencilAttachmentWrite,
    vk::PipelineStageFlagBits2::eEarlyFragmentTests,
    {.depthStencil = {1.0f, 0}}
  };
  static const systems::render::ImageRenderResourceDesc depth_sampled = {
    "depth",
    vk::ImageLayout::eDepthStencilReadOnlyOptimal,
    vk::AccessFlagBits2::eShaderRead,
    vk::PipelineStageFlagBits2::eFragmentShader,
    {}
  };
}

#endif // LISA_VULKAN_DEPTHRENDERRESOURCE_H
