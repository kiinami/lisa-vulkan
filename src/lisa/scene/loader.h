//
// Created by kinami on 5/2/26.
//

#ifndef LISA_VULKAN_LOADER_H
#define LISA_VULKAN_LOADER_H

#include "utils/common.h"

namespace lisa::scene {
  void load_xml(const path& filepath);
  void load_gltf(const path& filepath);
}

#endif //LISA_VULKAN_LOADER_H
