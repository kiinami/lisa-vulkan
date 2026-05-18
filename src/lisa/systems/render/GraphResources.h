//
// Created by kinami on 4/19/26.
//

#ifndef LISA_VULKAN_GRAPHRESOURCES_H
#define LISA_VULKAN_GRAPHRESOURCES_H
#include "graphics/images/Image.h"
#include "utils/common.h"
#include "utils/logging.h"

namespace lisa::systems::render {
  using GraphResourceHandle = uint32;

  enum GraphResourceUsage {
    ColorAttachmentWrite,
    DepthStencilAttachmentWrite,
    DepthStencilAttachmentRead,
    SampledFragment,
    StorageComputeWrite,
    TransferSrc,
    TransferDst
  };

  struct GraphResourceState {
    vk::PipelineStageFlags2 stage = vk::PipelineStageFlagBits2::eTopOfPipe;
    vk::AccessFlags2 access = vk::AccessFlagBits2::eNone;
    vk::ImageLayout layout = vk::ImageLayout::eUndefined;
  };

  enum class GraphResourceSamplerProfile {
    Linear,
    Nearest,
    ShadowPCF
  };

  struct ImageGraphResourceMetadata {
    graphics::ImageFormat format{vk::Format::eUndefined};
    vk::ImageUsageFlags usage{vk::ImageUsageFlagBits::eColorAttachment};
    GraphResourceSamplerProfile sampler_profile{
      GraphResourceSamplerProfile::Linear
    };

    static ImageGraphResourceMetadata from_type(const str& type) {
      static const umap<str, ImageGraphResourceMetadata> aliases = {
        {"color",
         {vk::Format::eR8G8B8A8Unorm,
          vk::ImageUsageFlagBits::eColorAttachment |
            vk::ImageUsageFlagBits::eSampled,
          GraphResourceSamplerProfile::Linear}},
        {"depth",
         {vk::Format::eD32Sfloat,
          vk::ImageUsageFlagBits::eDepthStencilAttachment |
            vk::ImageUsageFlagBits::eSampled,
          GraphResourceSamplerProfile::Linear}},
        {"final",
         {vk::Format::eR8G8B8A8Unorm,
          vk::ImageUsageFlagBits::eColorAttachment |
            vk::ImageUsageFlagBits::eTransferSrc,
          GraphResourceSamplerProfile::Linear}},
        {"normal",
         {vk::Format::eR16G16B16A16Sfloat,
          vk::ImageUsageFlagBits::eColorAttachment |
            vk::ImageUsageFlagBits::eSampled,
          GraphResourceSamplerProfile::Linear}},
        {"position",
         {vk::Format::eR32G32B32A32Sfloat,
          vk::ImageUsageFlagBits::eColorAttachment |
            vk::ImageUsageFlagBits::eSampled,
          GraphResourceSamplerProfile::Linear}},
        {"material",
         {vk::Format::eR8G8B8A8Unorm,
          vk::ImageUsageFlagBits::eColorAttachment |
            vk::ImageUsageFlagBits::eSampled,
          GraphResourceSamplerProfile::Linear}},
        {"ssao",
         {vk::Format::eR8Unorm,
          vk::ImageUsageFlagBits::eColorAttachment |
            vk::ImageUsageFlagBits::eSampled,
          GraphResourceSamplerProfile::Linear}},
        {"shadow_depth",
         {vk::Format::eD32Sfloat,
          vk::ImageUsageFlagBits::eDepthStencilAttachment |
            vk::ImageUsageFlagBits::eSampled,
          GraphResourceSamplerProfile::ShadowPCF}},
      };

      if (const auto it = aliases.find(type); it != aliases.end())
        return it->second;

      logging::abort("Unknown render resource type alias: {}", type);
    }
  };

  struct ImageGraphResource {
    graphics::Image image;
    GraphResourceSamplerProfile sampler_profile;

    graphics::ImageFormat format() const { return image.format(); }

    vk::ImageUsageFlags usage() const { return image.usage(); }

    vk::ImageAspectFlags aspect() const { return image.format().aspect_mask(); }

    vk::Extent2D extent() const { return image.extent2d(); }

    uint32 layers() const { return image.layers(); }

    ImageGraphResource(
      const str& id,
      const ImageGraphResourceMetadata metadata,
      const vec3& size,
      const uint32 layers = 1
    ) :
      image(
        id,
        metadata.format,
        metadata.usage,
        size,
        vk::ImageType::e2D,
        1,
        vk::ImageLayout::eUndefined,
        layers
      ),
      sampler_profile(metadata.sampler_profile) {}

    vk::ImageMemoryBarrier2 transition(
      GraphResourceState& src_state, GraphResourceUsage dst_usage
    ) const {
      GraphResourceState dst_state;

      switch (dst_usage) {
        case ColorAttachmentWrite:
          dst_state = {
            vk::PipelineStageFlagBits2::eColorAttachmentOutput,
            vk::AccessFlagBits2::eColorAttachmentWrite,
            vk::ImageLayout::eColorAttachmentOptimal
          };
          break;
        case DepthStencilAttachmentWrite:
          dst_state = {
            vk::PipelineStageFlagBits2::eEarlyFragmentTests |
              vk::PipelineStageFlagBits2::eLateFragmentTests,
            vk::AccessFlagBits2::eDepthStencilAttachmentWrite,
            vk::ImageLayout::eDepthAttachmentOptimal
          };
          break;
        case DepthStencilAttachmentRead:
          dst_state = {
            vk::PipelineStageFlagBits2::eEarlyFragmentTests |
              vk::PipelineStageFlagBits2::eLateFragmentTests,
            vk::AccessFlagBits2::eDepthStencilAttachmentRead,
            vk::ImageLayout::eDepthReadOnlyOptimal
          };
          break;
        case SampledFragment:
          dst_state = {
            vk::PipelineStageFlagBits2::eFragmentShader,
            vk::AccessFlagBits2::eShaderRead,
            vk::ImageLayout::eShaderReadOnlyOptimal
          };
          break;
        default: logging::abort("Unhandled resource usage transition");
      }

      vk::ImageMemoryBarrier2 barrier{
        .srcStageMask = src_state.stage,
        .srcAccessMask = src_state.access,
        .dstStageMask = dst_state.stage,
        .dstAccessMask = dst_state.access,
        .oldLayout = src_state.layout,
        .newLayout = dst_state.layout,
        .image = image,
        .subresourceRange = {aspect(), 0, 1, 0, layers()}
      };

      src_state = dst_state;

      return barrier;
    }

    vk::RenderingAttachmentInfo attachment_info(
      const bool is_load, const bool is_read_only = false
    ) const {
      const bool is_depth = image.format().is_depth();
      const bool is_layered = layers() > 1;
      const graphics::ImageViewDesc view_desc{
        .type =
          is_layered ? vk::ImageViewType::e2DArray : vk::ImageViewType::e2D,
        .format = format(),
        .range = {aspect(), 0, 1, 0, is_layered ? layers() : 1}
      };

      auto layout = is_depth ? vk::ImageLayout::eDepthAttachmentOptimal
                             : vk::ImageLayout::eColorAttachmentOptimal;
      if (is_depth && is_read_only)
        layout = vk::ImageLayout::eDepthReadOnlyOptimal;

      return vk::RenderingAttachmentInfo{
        .imageView = image.view(view_desc),
        .imageLayout = layout,
        .loadOp =
          is_load ? vk::AttachmentLoadOp::eLoad : vk::AttachmentLoadOp::eClear,
        .storeOp = vk::AttachmentStoreOp::eStore,
        .clearValue = is_depth
                        ? vk::ClearValue{.depthStencil = {1.0f, 0}}
                        : vk::ClearValue{{std::array{0.0f, 0.0f, 0.0f, 1.0f}}}
      };
    }
  };
}
#endif // LISA_VULKAN_GRAPHRESOURCES_H
