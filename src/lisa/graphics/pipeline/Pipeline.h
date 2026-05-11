//
// Created by kinami on 4/4/26.
//

#ifndef LISA_VULKAN_PIPELINE_H
#define LISA_VULKAN_PIPELINE_H

#include "graphics/descriptors/DescriptorContainer.h"
#include "graphics/context.h"
#include "resources/Shader.h"
#include "utils/common.h"

#include <vulkan/vulkan_raii.hpp>

namespace lisa::graphics {

  class Pipeline : public NamedVkObject<vk::raii::Pipeline> {
  public:
    struct CreateParameters {
      vk::DescriptorSetLayout descriptor_set_layout =
        context::descriptor_container().layout();
      vk::PushConstantRange push_constant_range;
      const resources::Shader& shader;
      bool vertex_input = true;
      bool position_only = false;
      const vector<vk::Format>& color_attachment_formats = {};
      bool depth_test_read = true;
      bool depth_test_write = false;
      vk::Format depth_attachment_format = vk::Format::eUndefined;
      vk::CompareOp depth_compare_op = vk::CompareOp::eLessOrEqual;
    };

    explicit Pipeline(const str& id, CreateParameters params);

    const vk::raii::PipelineLayout& layout() const { return layout_; }

  private:
    vk::raii::PipelineLayout layout_ = nullptr;

    static vk::raii::PipelineLayout create_layout(
      vk::DescriptorSetLayout descriptor_set_layout,
      vk::PushConstantRange push_constant_range
    );
  };

}

#endif // LISA_VULKAN_PIPELINE_H
