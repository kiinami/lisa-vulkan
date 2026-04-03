//
// Created by kinami on 4/1/26.
//

#include "Image.h"

#include "ImageFormat.h"
#include "graphics/context.h"

namespace lisa::graphics {
  Image::Image(
    ImageFormat format,
    vk::ImageUsageFlags usage,
    const vec3& size,
    vk::ImageType type,
    uint32 mips,
    vk::ImageLayout initial_layout,
    const vma::AllocationCreateInfo& allocation_ci
  ) :
    size_(size),
    mips_(mips),
    format_(format),
    usage_(usage),
    type_(type),
    initial_layout_(initial_layout) {
    allocate(allocation_ci);
  }

  void Image::allocate(const vma::AllocationCreateInfo& allocation_ci) {
    const vk::ImageCreateInfo image_ci{
      .imageType = type_,
      .format = format_,
      .extent =
        vk::Extent3D{
          static_cast<uint32_t>(size_.x),
          static_cast<uint32_t>(size_.y),
          static_cast<uint32_t>(size_.z)
        },
      .mipLevels = mips_,
      .arrayLayers = 1,
      .samples = vk::SampleCountFlagBits::e1,
      .tiling = vk::ImageTiling::eOptimal,
      .usage = usage_,
      .sharingMode = vk::SharingMode::eExclusive,
      .initialLayout = initial_layout_
    };
    image_ = context::allocator().create_image(image_ci, allocation_ci);
  }

  const vk::raii::ImageView& Image::view(const ImageViewDesc& desc) {
    if (const auto it = views_.find(desc); it != views_.end())
      return it->second;

    const vk::ImageViewCreateInfo view_ci{
      .image = static_cast<const vk::Image&>(*this),
      .viewType = desc.type,
      .format = desc.format,
      .subresourceRange = desc.range
    };

    auto newView = context::device().create_image_view(view_ci);
    auto [it, inserted] = views_.emplace(desc, std::move(newView));

    return it->second;
  }
}
