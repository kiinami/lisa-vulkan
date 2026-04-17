//
// Created by kinami on 4/17/26.
//

#ifndef LISA_VULKAN_UTILS_XML_H
#define LISA_VULKAN_UTILS_XML_H
#include "utils/common.h"

#include <pugixml.hpp>

namespace lisa::utils::xml {
  pugi::xml_document read(const path& filepath, const str& top_level = "");
  template<typename T> T parse(const str& value);
}

#endif // LISA_VULKAN_UTILS_XML_H
