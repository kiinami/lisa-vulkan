//
// Created by kinami on 4/4/26.
//

#include "Pipeline.h"

#include "graphics/context.h"
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

  Pipeline::Pipeline(CreateParameters params) {
    layout_ =
      create_layout(params.descriptor_set_layout, params.push_constant_range);

    vk::PipelineVertexInputStateCreateInfo vertex_input_state{
      .vertexBindingDescriptionCount = 0u,
      .pVertexBindingDescriptions = nullptr,
      .vertexAttributeDescriptionCount = 0u,
      .pVertexAttributeDescriptions = nullptr
    };

    vk::VertexInputBindingDescription vertex_binding{};
    vector<vk::VertexInputAttributeDescription> vertex_attributes;
    if (params.vertex_input) {
      vertex_binding = {
        .binding = 0,
        .stride = sizeof(resources::Vertex),
        .inputRate = vk::VertexInputRate::eVertex
      };

      vertex_attributes =
        resources::Vertex::attribute_descriptions(vertex_binding.binding);

      if (params.position_only && !vertex_attributes.empty())
        vertex_attributes.resize(1);

      vertex_input_state.vertexBindingDescriptionCount = 1u;
      vertex_input_state.pVertexBindingDescriptions = &vertex_binding;
      vertex_input_state.vertexAttributeDescriptionCount =
        static_cast<uint32>(vertex_attributes.size());
      vertex_input_state.pVertexAttributeDescriptions =
        vertex_attributes.data();
    }

    const vk::PipelineInputAssemblyStateCreateInfo input_assembly_state{
      .topology = vk::PrimitiveTopology::eTriangleList
    };

    vector<str> entry_names;
    vector<vk::PipelineShaderStageCreateInfo> shader_stages;
    entry_names.reserve(params.shader.stages().size());
    shader_stages.reserve(params.shader.stages().size());

    for (const auto& [stage, entry_point] : params.shader.stages()) {
      shader_stages.push_back(
        vk::PipelineShaderStageCreateInfo{
          .stage = stage, .module = params.shader.module(), .pName = entry_point
        }
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

    vector blend_attachments(
      params.color_attachment_formats.size(),
      vk::PipelineColorBlendAttachmentState{
        .colorWriteMask = vk::ColorComponentFlagBits::eR |
                          vk::ColorComponentFlagBits::eG |
                          vk::ColorComponentFlagBits::eB |
                          vk::ColorComponentFlagBits::eA
      }
    );
    const vk::PipelineColorBlendStateCreateInfo color_blend_state{
      .attachmentCount = static_cast<uint32>(blend_attachments.size()),
      .pAttachments = blend_attachments.data()
    };

    const vk::PipelineDepthStencilStateCreateInfo depth_stencil_state{
      .depthTestEnable = static_cast<vk::Bool32>(params.depth_test_read),
      .depthWriteEnable = static_cast<vk::Bool32>(params.depth_test_write),
      .depthCompareOp = params.depth_compare_op
    };

    const vk::PipelineRenderingCreateInfo rendering_ci{
      .colorAttachmentCount =
        static_cast<uint32>(params.color_attachment_formats.size()),
      .pColorAttachmentFormats = params.color_attachment_formats.data(),
      .depthAttachmentFormat = params.depth_attachment_format
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
