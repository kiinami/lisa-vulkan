//
// Created by kinami on 4/4/26.
//

#ifndef LISA_VULKAN_PIPELINE_H
#define LISA_VULKAN_PIPELINE_H

#include "resources/Shader.h"
#include "utils/common.h"

#include <vulkan/vulkan_raii.hpp>

namespace lisa::graphics {

  class Pipeline {
  public:
    Pipeline(
      vk::DescriptorSetLayout descriptor_set_layout,
      vk::PushConstantRange push_constant_range,
      const resources::Shader& shader,
      bool is_stencil
    );
    ~Pipeline() = default;

  private:
    vk::raii::PipelineLayout layout_ = nullptr;
    vk::raii::Pipeline pipeline_ = nullptr;

    static vk::raii::PipelineLayout create_layout();
  };

}

#endif // LISA_VULKAN_PIPELINE_H
