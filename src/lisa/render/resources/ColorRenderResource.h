//
// Created by kinami on 4/17/26.
//

#ifndef LISA_VULKAN_COLORRENDERRESOURCE_H
#define LISA_VULKAN_COLORRENDERRESOURCE_H
#include "systems/render/ImageRenderResource.h"

namespace lisa::render::resources {

  class ColorRenderResource : public systems::render::ImageRenderResource {
  public:
    static constexpr auto TYPE_ID = "color";
    static constexpr auto FORMAT = vk::Format::eR8G8B8A8Unorm;
    static constexpr auto USAGE = vk::ImageUsageFlagBits::eColorAttachment;
    static constexpr auto ASPECT = vk::ImageAspectFlagBits::eColor;

    explicit ColorRenderResource(const str& id, const vec3& size) :
      ImageRenderResource(id, FORMAT, USAGE, size) {}

    ~ColorRenderResource() override = default;

    str type() const override { return TYPE_ID; }

    vk::ImageAspectFlags aspect() override { return ASPECT; }
  };

  static const systems::render::ImageRenderResourceDesc color_attachment = {
    "color",
    vk::ImageLayout::eColorAttachmentOptimal,
    vk::AccessFlagBits2::eColorAttachmentWrite,
    vk::PipelineStageFlagBits2::eColorAttachmentOutput,
    {.color = {std::array{0.0f, 0.0f, 0.0f, 1.0f}}}
  };
  static const systems::render::ImageRenderResourceDesc color_sampled = {
    "color",
    vk::ImageLayout::eShaderReadOnlyOptimal,
    vk::AccessFlagBits2::eShaderRead,
    vk::PipelineStageFlagBits2::eFragmentShader,
    {}
  };

}

#endif // LISA_VULKAN_COLORRENDERRESOURCE_H
