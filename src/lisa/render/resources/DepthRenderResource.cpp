//
// Created by kinami on 4/18/26.
//

#include "DepthRenderResource.h"

#include "systems/render/RenderResourceRegistry.h"

namespace lisa::render::resources {
  REGISTER_RENDER_RESOURCE(DepthRenderResource);

  vk::RenderingAttachmentInfo DepthRenderResource::rendering_attachment() {
    return {
      .imageView = view(),
      .imageLayout = vk::ImageLayout::eDepthAttachmentOptimal,
      .loadOp = vk::AttachmentLoadOp::eClear,
      .storeOp = vk::AttachmentStoreOp::eStore,
      .clearValue = {.depthStencil = {1.0f, 0}}
    };
  }
}
