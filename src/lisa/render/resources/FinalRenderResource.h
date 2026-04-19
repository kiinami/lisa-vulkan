//
// Created by kinami on 4/19/26.
//

#ifndef LISA_VULKAN_FINALRENDERRESOURCE_H
#define LISA_VULKAN_FINALRENDERRESOURCE_H
#include "systems/render/ImageRenderResource.h"

namespace lisa::render::resources {

  class FinalRenderResource : public systems::render::ImageRenderResource {
  public:
    static constexpr auto TYPE_ID = "final";
    static constexpr auto FORMAT = vk::Format::eR8G8B8A8Unorm;
    static constexpr auto USAGE = vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eTransferSrc;
    static constexpr auto ASPECT = vk::ImageAspectFlagBits::eColor;

    explicit FinalRenderResource(const str& id, const vec3& size) :
      ImageRenderResource(id, FORMAT, USAGE, size) {}

    ~FinalRenderResource() override = default;

    str type() const override { return TYPE_ID; }

    vk::ImageAspectFlags aspect() override { return ASPECT; }
  };

  static const systems::render::ImageRenderResourceDesc final_attachment = {
    "final",
    vk::ImageLayout::eColorAttachmentOptimal,
    vk::AccessFlagBits2::eColorAttachmentWrite,
    vk::PipelineStageFlagBits2::eColorAttachmentOutput,
    {.color = {std::array{0.0f, 0.0f, 0.0f, 1.0f}}}
  };

}

#endif // LISA_VULKAN_FINALRENDERRESOURCE_H
