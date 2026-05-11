//
// Created by kinami on 5/11/26.
//

#ifndef LISA_VULKAN_PIPELINELAYOUT_H
#define LISA_VULKAN_PIPELINELAYOUT_H

#include "graphics/vk/NamedVkObject.h"

#include <vulkan/vulkan_raii.hpp>

namespace lisa::graphics {

  class PipelineLayout : public NamedVkObject<vk::raii::PipelineLayout> {
  public:
    PipelineLayout() = default;

    explicit PipelineLayout(
      const str& id,
      vk::DescriptorSetLayout descriptor_set_layout,
      vk::PushConstantRange push_constant_range
    ) :
      NamedVkObject(id) {
      const vk::PipelineLayoutCreateInfo layout_ci{
        .setLayoutCount = 1,
        .pSetLayouts = &descriptor_set_layout,
        .pushConstantRangeCount = 1,
        .pPushConstantRanges = &push_constant_range
      };
      set(context::device()->createPipelineLayout(layout_ci));
    }
  };

}

#endif // LISA_VULKAN_PIPELINELAYOUT_H
