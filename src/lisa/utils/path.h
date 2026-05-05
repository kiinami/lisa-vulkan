//
// Created by kinami on 25/04/2026.
//

#ifndef LISA_VULKAN_UTILS_PATH_H
#define LISA_VULKAN_UTILS_PATH_H

#include "common.h"

#include <fstream>

namespace lisa::utils {
  inline str pstr(const path& p) {
#ifdef _WIN32
    return p.string();
#else
    return p.native();
#endif
  }

  inline vector<std::byte> read_file(const path& filepath) {
    std::ifstream file(filepath, std::ios::ate | std::ios::binary);
    if (!file)
      throw std::runtime_error("Cannot open file: " + filepath.string());

    const std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);

    vector<std::byte> buffer(size);
    if (!file.read(reinterpret_cast<char*>(buffer.data()), size))
      throw std::runtime_error("Failed to read file: " + filepath.string());

    return buffer;
  }
}

#endif // LISA_VULKAN_PATH_H
