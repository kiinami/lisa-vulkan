//
// Created by kinami on 4/4/26.
//

#include "Pipeline.h"

#include "graphics/context.h"
#include "graphics/images/ImageFormat.h"
#include "resources/Vertex.h"

namespace lisa::graphics {
  vk::raii::PipelineLayout Pipeline::create_layout(
    vk::DescriptorSetLayout descriptor_set_layout,
    vk::PushConstantRange push_constant_range
  ) {
    const vk::PipelineLayoutCreateInfo layout_ci{
      .setLayoutCount = 1,
      .pSetLayouts = &descriptor_set_layout,
      .pushConstantRangeCount = 1,
      .pPushConstantRanges = &push_constant_range
    };
    return context::device()->createPipelineLayout(layout_ci);
  }

  Pipeline::Pipeline(
    const vk::DescriptorSetLayout descriptor_set_layout,
    const vk::PushConstantRange push_constant_range,
    const resources::Shader& shader,
    bool is_stencil,
    graphics::ImageFormat format
  ) {
    layout_ = create_layout(descriptor_set_layout, push_constant_range);

    const vk::VertexInputBindingDescription vertex_binding{
      .binding = 0,
      .stride = sizeof(resources::Vertex),
      .inputRate = vk::VertexInputRate::eVertex
    };
    const vector<vk::VertexInputAttributeDescription> vertex_attributes =
      resources::Vertex::attribute_descriptions(vertex_binding.binding);

    const vk::PipelineVertexInputStateCreateInfo vertex_input_state{
      .vertexBindingDescriptionCount =
        static_cast<uint32_t>(vertex_attributes.size()),
      .pVertexBindingDescriptions = &vertex_binding,
      .vertexAttributeDescriptionCount = 1,
      .pVertexAttributeDescriptions = vertex_attributes.data()
    };

    const vk::PipelineInputAssemblyStateCreateInfo input_assembly_state{
      .topology = vk::PrimitiveTopology::eTriangleList
    };

    vector<vk::PipelineShaderStageCreateInfo> shader_stages;
    for (const auto& [stage, entry_point] : shader.stages()) {
      shader_stages.push_back(
        {.stage = stage,
         .module = shader.module(),
         .pName = (shader.id() + "::" + entry_point).c_str()}
      );
    }

    const vector dynamic_states{
      vk::DynamicState::eViewport, vk::DynamicState::eScissor
    };
    const vk::PipelineDynamicStateCreateInfo dynamic_state{
      .dynamicStateCount = static_cast<uint32>(dynamic_states.size()),
      .pDynamicStates = dynamic_states.data()
    };

    const vk::PipelineViewportStateCreateInfo viewport_state{
      .viewportCount = 1, .scissorCount = 1
    };

    const vk::PipelineRasterizationStateCreateInfo rasterization_state{
      .lineWidth = 1.0f
    };

    const vk::PipelineMultisampleStateCreateInfo multisample_state{
      .rasterizationSamples = vk::SampleCountFlagBits::e1
    };

    const vk::PipelineDepthStencilStateCreateInfo depth_stencil_state{
      .depthTestEnable = static_cast<vk::Bool32>(is_stencil),
      .depthWriteEnable = static_cast<vk::Bool32>(is_stencil),
      .depthCompareOp = vk::CompareOp::eLessOrEqual
    };

    const vk::PipelineColorBlendAttachmentState blend_attachment{
      .colorWriteMask = vk::ColorComponentFlagBits::eR |
                        vk::ColorComponentFlagBits::eG |
                        vk::ColorComponentFlagBits::eB |
                        vk::ColorComponentFlagBits::eA
    };
    const vk::PipelineColorBlendStateCreateInfo color_blend_state{
      .attachmentCount = 1,
      .pAttachments = &blend_attachment
    };

    const vk::PipelineRenderingCreateInfo rendering_ci{
      .colorAttachmentCount = 1,
      .pColorAttachmentFormats = *format
    };

    const vk::GraphicsPipelineCreateInfo pipeline_ci{
      .pNext = rendering_ci,
      .stageCount = static_cast<uint32>(shader_stages.size()),
      .pStages = shader_stages.data(),
      .pVertexInputState = &vertex_input_state,
      .pInputAssemblyState = &input_assembly_state,
      .pViewportState = &viewport_state,
      .pRasterizationState = &rasterization_state,
      .pMultisampleState = &multisample_state,
      .pDepthStencilState = &depth_stencil_state,
      .pColorBlendState = &color_blend_state,
      .pDynamicState = &dynamic_state,
      .layout = layout_
    };
    pipeline_ = context::device()->createGraphicsPipeline(nullptr, pipeline_ci);
  }
}
