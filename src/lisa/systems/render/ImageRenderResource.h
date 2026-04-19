//
// Created by kinami on 4/18/26.
//

#ifndef LISA_VULKAN_IMAGERENDERRESOURCE_H
#define LISA_VULKAN_IMAGERENDERRESOURCE_H
#include "RenderResource.h"
#include "graphics/commands/CommandBuffer.h"

namespace lisa::systems::render {
  struct ImageRenderResourceDesc : RenderResourceDesc {
    vk::ImageLayout layout;
    vk::AccessFlags2 access;
    vk::PipelineStageFlags2 stage;
    vk::AttachmentLoadOp load_op = vk::AttachmentLoadOp::eClear;
    vk::AttachmentStoreOp store_op = vk::AttachmentStoreOp::eStore;
    vk::ClearValue clear_value;

    ImageRenderResourceDesc(
      const str& type,
      const vk::ImageLayout layout,
      const vk::AccessFlags2 access,
      const vk::PipelineStageFlags2 stage,
      const vk::ClearValue clear_value
    ) :
      RenderResourceDesc(type),
      layout(layout),
      access(access),
      stage(stage),
      clear_value(clear_value) {}
  };

  class ImageRenderResource : public RenderResource {
  public:
    explicit ImageRenderResource(
      const str& id,
      const graphics::ImageFormat format,
      vk::ImageUsageFlags usage,
      const vec3& size
    ) :
      RenderResource(id),
      image_(format, usage, size),
      desc_(
        "",
        vk::ImageLayout::eUndefined,
        vk::AccessFlagBits2::eNone,
        vk::PipelineStageFlagBits2::eNone,
        {}
      ) {}

    const vk::raii::ImageView& view() const;

    vk::RenderingAttachmentInfo rendering_attachment();

    void transition(
      const graphics::CommandBuffer& cmdb, const ImageRenderResourceDesc& desc
    );

    const graphics::Image& image() const { return image_; }

    virtual vk::ImageAspectFlags aspect() = 0;

  protected:
    graphics::Image image_;
    ImageRenderResourceDesc desc_;
  };

}

#endif // LISA_VULKAN_IMAGERENDERRESOURCE_H
