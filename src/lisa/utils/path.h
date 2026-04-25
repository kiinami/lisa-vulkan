//
// Created by kinami on 25/04/2026.
//

#ifndef LISA_VULKAN_UTILS_PATH_H
#define LISA_VULKAN_UTILS_PATH_H

#include "common.h"

namespace lisa::utils {
  inline str pstr(const path& p) {
#ifdef _WIN32
    return p.string();
#else
    return p.native();
#endif
  }

}

#endif // LISA_VULKAN_PATH_H
