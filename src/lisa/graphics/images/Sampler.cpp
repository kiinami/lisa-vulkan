//
// Created by kinami on 4/3/26.
//

#include "Sampler.h"

#include "graphics/context.h"

namespace lisa::graphics {
  Sampler::Sampler(
    const float max_lod,
    const vk::Filter mag_filter,
    const vk::Filter min_filter,
    const vk::SamplerMipmapMode mipmap_mode,
    const bool anisotropic,
    const float max_anisotropy
  ) :
    sampler_(
      context::device()->createSampler(
        {.magFilter = mag_filter,
         .minFilter = min_filter,
         .mipmapMode = mipmap_mode,
         .anisotropyEnable = anisotropic,
         .maxAnisotropy = max_anisotropy,
         .maxLod = max_lod}
      )
    ) {}
}
