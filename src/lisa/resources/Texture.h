//
// Created by kinami on 4/3/26.
//

#ifndef LISA_VULKAN_TEXTURE_H
#define LISA_VULKAN_TEXTURE_H
#include "graphics/descriptors/DescriptorAllocator.h"
#include "graphics/images/Image.h"
#include "graphics/images/Sampler.h"
#include "systems/resources/Resource.h"
#include "utils/common.h"

#include <ktx.h>

namespace lisa::resources {

  class Texture : public systems::resources::Resource {
  public:
    explicit Texture(const path& filepath) : Resource(filepath) {}

    ~Texture() override { unload(); }

    graphics::Image& image() { return image_; }

    const graphics::Image& image() const { return image_; }

    graphics::Sampler& sampler() { return sampler_; }

    const graphics::Sampler& sampler() const { return sampler_; }

    DescriptorIndex descriptor_index() const { return descriptor_index_; }

  protected:
    bool load_function() override;
    bool unload_function() override;

    str type_name() override { return "Texture"; }

  private:
    graphics::Image image_;
    graphics::Sampler sampler_;
    DescriptorIndex descriptor_index_ = 0;
    uint32 levels_;

    static graphics::Image load_ktx(const path& filepath);
    static graphics::Image load_jpg(const path& filepath);
    static graphics::Image load_exr(const path& filepath);
    static void generate_mipmaps(
      const void* pixel_data,
      size_t buffer_size,
      uint32 width,
      uint32 height,
      uint32 mip_levels,
      const graphics::Image& image
    );
  };

}

#endif // LISA_VULKAN_TEXTURE_H
