//
// Created by kinami on 4/1/26.
//

#ifndef LISA_VULKAN_IMAGE_H
#define LISA_VULKAN_IMAGE_H

#include "ImageFormat.h"
#include "utils/common.h"
#include "vk_mem_alloc_raii.hpp"

#include <unordered_map>
#include <variant>
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

  class Image {
  public:
    Image() = default;
    ~Image() = default;

    Image(
      ImageFormat format,
      vk::ImageUsageFlags usage,
      const vec3& size,
      vk::ImageType type = vk::ImageType::e2D,
      uint32 mips = 1,
      const vma::AllocationCreateInfo& allocation_ci = DEFAULT_ALLOCATION_CI
    );

    Image(
      const vk::Image& image,
      const ImageFormat format,
      const vec3& size,
      const vk::ImageUsageFlags usage
    ) :
      image_(image),
      size_(size),
      format_(format),
      usage_(usage),
      type_(vk::ImageType::e2D) {}

    Image(const Image&) = delete;
    Image& operator=(const Image&) = delete;

    Image(Image&&) noexcept = default;
    Image& operator=(Image&&) noexcept = default;

    operator const vk::Image&() const {
      if (std::holds_alternative<vma::raii::Image>(image_))
        return *std::get<vma::raii::Image>(image_);

      return std::get<vk::Image>(image_);
    }

    const vk::raii::ImageView& view(const ImageViewDesc& desc);

    const vec3& size() const { return size_; }

    uint32 mipmaps() const { return mips_; }

    ImageFormat format() const { return format_; }

    vk::ImageUsageFlags usage() const { return usage_; }

  protected:
    std::variant<vma::raii::Image, vk::Image> image_ = vk::Image(nullptr);

    std::unordered_map<ImageViewDesc, vk::raii::ImageView, ImageViewDescHash>
      views_;

    vec3 size_;
    uint32 mips_ = 1;

    ImageFormat format_;
    vk::ImageUsageFlags usage_{ 0 };
    vk::ImageType type_;

    void allocate(
      const vma::AllocationCreateInfo& allocation_ci = DEFAULT_ALLOCATION_CI
    );
  };
}

#endif // LISA_VULKAN_IMAGE_H
