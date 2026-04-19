//
// Created by kinami on 4/4/26.
//

#ifndef LISA_VULKAN_PIPELINE_H
#define LISA_VULKAN_PIPELINE_H

#include "graphics/images/ImageFormat.h"
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
      bool depth_test_enable,
      const vector<vk::Format>& color_attachment_formats,
      vk::Format depth_attachment_format = vk::Format::eUndefined
    );
    ~Pipeline() = default;

    operator const vk::raii::Pipeline&() { return pipeline_; }

    operator vk::Pipeline() const { return *pipeline_; }

    vk::Pipeline operator*() const { return *pipeline_; }

    const vk::raii::PipelineLayout& layout() const { return layout_; }

  private:
    vk::raii::PipelineLayout layout_ = nullptr;
    vk::raii::Pipeline pipeline_ = nullptr;

    static vk::raii::PipelineLayout create_layout(
      vk::DescriptorSetLayout descriptor_set_layout,
      vk::PushConstantRange push_constant_range
    );
  };

}

#endif // LISA_VULKAN_PIPELINE_H
