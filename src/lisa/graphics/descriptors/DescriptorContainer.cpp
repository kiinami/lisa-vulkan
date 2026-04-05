//
// Created by kinami on 4/3/26.
//

#include "DescriptorContainer.h"

#include "graphics/context.h"

namespace lisa::graphics {
  DescriptorContainer::DescriptorContainer(
    uint32 size, vk::DescriptorType type
  ) :
    size_(size),
    pool_(create_pool(size, type)),
    layout_(create_layout(size, type)),
    set_(create_set(pool_, layout_, size)) {}

  vk::raii::DescriptorPool DescriptorContainer::create_pool(
    const uint32 descriptor_count, const vk::DescriptorType type
  ) {
    const vk::DescriptorPoolSize pool_size{
      .type = type, .descriptorCount = descriptor_count
    };
    const vk::DescriptorPoolCreateInfo pool_ci{
      .flags = vk::DescriptorPoolCreateFlagBits::eUpdateAfterBind |
               vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet,
      .maxSets = 1,
      .poolSizeCount = 1,
      .pPoolSizes = &pool_size
    };
    return context::device()->createDescriptorPool(pool_ci);
  }

  vk::raii::DescriptorSetLayout DescriptorContainer::create_layout(
    const uint32 size,
    const vk::DescriptorType type,
    const vk::ShaderStageFlags shader_stage
  ) {
    constexpr vk::DescriptorBindingFlags binding_flags{
      vk::DescriptorBindingFlagBits::eVariableDescriptorCount |
      vk::DescriptorBindingFlagBits::ePartiallyBound |
      vk::DescriptorBindingFlagBits::eUpdateAfterBind |
      vk::DescriptorBindingFlagBits::eUpdateUnusedWhilePending
    };
    const vk::DescriptorSetLayoutBindingFlagsCreateInfo binding_flags_ci{
      .bindingCount = 1, .pBindingFlags = &binding_flags
    };
    const vk::DescriptorSetLayoutBinding layout_binding{
      .binding = 0,
      .descriptorType = type,
      .descriptorCount = size,
      .stageFlags = shader_stage
    };
    const vk::DescriptorSetLayoutCreateInfo layout_ci{
      .pNext = &binding_flags_ci,
      .flags = vk::DescriptorSetLayoutCreateFlagBits::eUpdateAfterBindPool,
      .bindingCount = 1,
      .pBindings = &layout_binding
    };
    return context::device()->createDescriptorSetLayout(layout_ci);
  }

  vk::raii::DescriptorSet DescriptorContainer::create_set(
    const vk::raii::DescriptorPool& pool,
    const vk::raii::DescriptorSetLayout& layout,
    const uint32 size
  ) {
    const vk::DescriptorSetVariableDescriptorCountAllocateInfo
      variable_descriptor_count_ai{
        .descriptorSetCount = 1, .pDescriptorCounts = &size
      };
    const vk::DescriptorSetAllocateInfo set_ai{
      .pNext = &variable_descriptor_count_ai,
      .descriptorPool = pool,
      .descriptorSetCount = 1,
      .pSetLayouts = &*layout
    };
    return std::move(context::device()->allocateDescriptorSets(set_ai)[0]);
  }
}
