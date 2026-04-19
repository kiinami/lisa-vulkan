//
// Created by kinami on 4/18/26.
//

#include "ImageRenderResource.h"

namespace lisa::systems::render {
  const vk::raii::ImageView& ImageRenderResource::view() const {
    return image_.view(
      {.type = vk::ImageViewType::e2D,
       .format = image_.format(),
       .range = {
         .aspectMask = image_.format().aspect_mask(),
         .baseMipLevel = 0,
         .levelCount = 1,
         .baseArrayLayer = 0,
         .layerCount = 1
       }}
    );
  }

  vk::RenderingAttachmentInfo ImageRenderResource::rendering_attachment() {
    return {
      .imageView = view(),
      .imageLayout = vk::ImageLayout::eColorAttachmentOptimal,
      .loadOp = vk::AttachmentLoadOp::eClear,
      .storeOp = vk::AttachmentStoreOp::eStore,
      .clearValue = {.color = {std::array{0.0f, 0.0f, 0.0f, 1.0f}}}
    };
  }

  void ImageRenderResource::transition(
    const graphics::CommandBuffer& cmdb, const ImageRenderResourceDesc& desc
  ) {
    if (desc_.layout == desc.layout &&
        !(desc_.access & vk::AccessFlagBits2::eMemoryWrite))
      return;

    const vk::ImageMemoryBarrier2 barrier{
      .srcStageMask  = desc_.stage,
      .srcAccessMask = desc_.access,
      .dstStageMask  = desc.stage,
      .dstAccessMask = desc.access,
      .oldLayout     = desc_.layout,
      .newLayout     = desc.layout,
      .image         = image_,
      .subresourceRange = {
        .aspectMask     = aspect(),
        .baseMipLevel   = 0,
        .levelCount     = 1,
        .baseArrayLayer = 0,
        .layerCount     = 1
      }
    };

    cmdb->pipelineBarrier2(vk::DependencyInfo{
      .imageMemoryBarrierCount = 1,
      .pImageMemoryBarriers    = &barrier
    });

    desc_ = desc;
  }
}
