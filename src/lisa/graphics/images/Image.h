//
// Created by kinami on 4/1/26.
//

#ifndef LISA_VULKAN_IMAGE_H
#define LISA_VULKAN_IMAGE_H

#include "ImageFormat.h"
#include "graphics/commands/CommandBuffer.h"
#include "graphics/vk/NamedVkObject.h"
#include "utils/common.h"

#include <unordered_map>
#include <variant>
#include <vk_mem_alloc_raii.hpp>
#include <vulkan/vulkan_raii.hpp>

namespace lisa::graphics {

  struct ImageViewDesc {
    vk::ImageViewType type;
    ImageFormat format;
    vk::ImageSubresourceRange range;

    bool operator==(const ImageViewDesc&) const = default;
  };

  struct ImageViewDescHash {
    size_t operator()(const ImageViewDesc& d) const noexcept {
      size_t h = 0;

      auto hashCombine = [&h]<typename T>(T v) {
        std::hash<std::decay_t<T>> hasher;
        h ^= hasher(v) + 0x9e3779b9 + (h << 6) + (h >> 2);
      };

      hashCombine(static_cast<uint32>(d.type));
      hashCombine(static_cast<uint32>(d.format));

      hashCombine(static_cast<uint32>(d.range.aspectMask));
      hashCombine(d.range.baseMipLevel);
      hashCombine(d.range.levelCount);
      hashCombine(d.range.baseArrayLayer);
      hashCombine(d.range.layerCount);

      return h;
    }
  };

  static constexpr vma::AllocationCreateInfo DEFAULT_ALLOCATION_CI = {
    .flags = vma::AllocationCreateFlagBits::eDedicatedMemory,
    .usage = vma::MemoryUsage::eAuto
  };

  class Image : public NamedVkObject<vma::raii::Image> {
  public:
    Image() = default;

    Image(
      const str& id,
      ImageFormat format,
      vk::ImageUsageFlags usage,
      const vec3& size,
      vk::ImageType type = vk::ImageType::e2D,
      uint32 mips = 1,
      vk::ImageLayout initial_layout = vk::ImageLayout::eUndefined,
      const vma::AllocationCreateInfo& allocation_ci = DEFAULT_ALLOCATION_CI
    );

    const vk::raii::ImageView& view(const ImageViewDesc& desc) const;

    const vec3& size() const { return size_; }

    uint32 mipmaps() const { return mips_; }

    ImageFormat format() const { return format_; }

    vk::ImageUsageFlags usage() const { return usage_; }

    vk::ImageLayout initial_layout() const { return initial_layout_; }

    static Image from_data(
      const str& id,
      const void* data,
      size_t size,
      const vec3& extent,
      ImageFormat format,
      vk::ImageUsageFlags usage,
      const CommandBuffer& cmdb,
      uint32 mip_levels = 1
    );

    static Image from_data(
      const str& id,
      const void* data,
      size_t size,
      const vec3& extent,
      ImageFormat format,
      vk::ImageUsageFlags usage,
      uint32 mip_levels = 1
    );

  protected:
    mutable umap<ImageViewDesc, vk::raii::ImageView, ImageViewDescHash> views_;

    vec3 size_;
    uint32 mips_ = 1;

    ImageFormat format_{vk::Format::eUndefined};
    vk::ImageUsageFlags usage_{0};
    vk::ImageType type_{};
    vk::ImageLayout initial_layout_{};

    void allocate(
      const vma::AllocationCreateInfo& allocation_ci = DEFAULT_ALLOCATION_CI
    );
  };
}

#endif // LISA_VULKAN_IMAGE_H
