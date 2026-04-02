//
// Created by kinami on 4/2/26.
//

#ifndef LISA_VULKAN_MESH_H
#define LISA_VULKAN_MESH_H
#include "graphics/buffer/Buffer.h"

#include <filesystem>
#include <vulkan/vulkan_raii.hpp>

namespace lisa::meshes {

  class Mesh {
  public:
    ~Mesh() = default;

    explicit Mesh(const std::filesystem::path& filename);

  private:
    graphics::Buffer vertex_buffer_;
    vk::DeviceSize count_;

    void load_obj(const std::filesystem::path& filename);
  };

}

#endif // LISA_VULKAN_MESH_H
