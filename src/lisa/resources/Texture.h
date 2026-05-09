//
// Created by kinami on 4/3/26.
//

#ifndef LISA_VULKAN_TEXTURE_H
#define LISA_VULKAN_TEXTURE_H
#include "fastgltf/types.hpp"
#include "graphics/descriptors/DescriptorAllocator.h"
#include "graphics/images/Image.h"
#include "graphics/images/Sampler.h"
#include "systems/resources/Resource.h"
#include "utils/common.h"
#include "utils/logging.h"

namespace lisa::resources {

  class Texture : public systems::resources::Resource {
  public:
    explicit Texture(
      const str& id,
      const path& filepath,
      const graphics::CommandBuffer& cmdb,
      int channel = -1
    );
    explicit Texture(
      const str& id,
      const vector<std::byte>& data,
      fastgltf::MimeType mime,
      const graphics::CommandBuffer& cmdb,
      int channel = -1
    );

    void unload() override {}

    graphics::Image& image() { return image_; }

    const graphics::Image& image() const { return image_; }

    graphics::Sampler& sampler() { return sampler_; }

    const graphics::Sampler& sampler() const { return sampler_; }

    DescriptorIndex descriptor_index() const { return descriptor_index_; }

  private:
    graphics::Image image_;
    graphics::Sampler sampler_;
    DescriptorIndex descriptor_index_ = 0;
    uint32 levels_ = 1;

    static graphics::Image load_ktx(
      const str& id,
      const vector<std::byte>& data,
      const graphics::CommandBuffer& cmdb,
      int channel = -1
    );
    static graphics::Image load_jpg(
      const str& id,
      const vector<std::byte>& data,
      const graphics::CommandBuffer& cmdb,
      int channel = -1
    );
    static graphics::Image load_exr(
      const str& id,
      const path& filepath,
      const graphics::CommandBuffer& cmdb,
      int channel = -1
    );

    void setup();
  };

  struct TextureSpec : systems::resources::ResourceSpec<Texture> {
    optional<path> filepath;
    optional<pair<fastgltf::MimeType, vector<std::byte>>> data;
    int channel = -1;

    explicit TextureSpec(
      const str& id, const path& filepath, const int channel = -1
    ) :
      ResourceSpec(id),
      filepath(filepath),
      channel(channel) {}

    explicit TextureSpec(
      const str& id,
      const fastgltf::MimeType& mime,
      const vector<std::byte>& data,
      const int channel = -1
    ) :
      ResourceSpec(id),
      data({mime, data}),
      channel(channel) {}

    Texture load_resource(const graphics::CommandBuffer& cmdb) override {
      if (data.has_value())
        return Texture(id, data->second, data->first, cmdb, channel);
      if (filepath.has_value())
        return Texture(id, filepath.value(), cmdb, channel);

      throw std::runtime_error(
        "TextureSpec must have either a filepath or data"
      );
    }
  };

}

#endif // LISA_VULKAN_TEXTURE_H
