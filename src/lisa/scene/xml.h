//
// Created by kinami on 5/2/26.
//

#ifndef LISA_VULKAN_SCENE_XML_H
#define LISA_VULKAN_SCENE_XML_H
#include "utils/path.h"

namespace lisa::scene::xml {

  void load(
    const path& filepath,
    const mat4& transform = mat4(1.0f),
    const str& parent_id = ""
  );

}

#endif // LISA_VULKAN_SCENE_XML_H
