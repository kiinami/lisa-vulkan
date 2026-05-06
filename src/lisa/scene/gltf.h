//
// Created by kinami on 5/2/26.
//

#ifndef LISA_VULKAN_GLTF_H
#define LISA_VULKAN_GLTF_H

#include "utils/common.h"

namespace lisa::scene::gltf {

  void load(const path& filepath, mat4 transform = mat4(1.0f));

}

#endif //LISA_VULKAN_GLTF_H
