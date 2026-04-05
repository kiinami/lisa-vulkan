//
// Created by kinami on 4/3/26.
//

#ifndef LISA_VULKAN_DESCRIPTORCONTAINER_H
#define LISA_VULKAN_DESCRIPTORCONTAINER_H
#include "graphics/device/LogicalDevice.h"
#include "utils/common.h"

#include <vulkan/vulkan_raii.hpp>

namespace lisa::graphics {

  class DescriptorContainer {
  public:
    explicit DescriptorContainer(
      uint32 size,
      vk::DescriptorType type = vk::DescriptorType::eCombinedImageSampler
    );
    ~DescriptorContainer() = default;

    const vk::raii::DescriptorPool& pool() { return pool_; }

    const vk::raii::DescriptorSetLayout& layout() { return layout_; }

    const vk::raii::DescriptorSet& set() { return set_; }

  private:
    uint32 size_;
    vk::raii::DescriptorPool pool_ = nullptr;
    vk::raii::DescriptorSetLayout layout_ = nullptr;
    vk::raii::DescriptorSet set_ = nullptr;

    static vk::raii::DescriptorPool
      create_pool(uint32 descriptor_count, vk::DescriptorType type);

    static vk::raii::DescriptorSetLayout create_layout(
      uint32 size,
      vk::DescriptorType type,
      vk::ShaderStageFlags shader_stage = vk::ShaderStageFlagBits::eAll
    );

    static vk::raii::DescriptorSet create_set(
      const vk::raii::DescriptorPool& pool,
      const vk::raii::DescriptorSetLayout& layout,
      uint32 size
    );
  };

}

#endif // LISA_VULKAN_DESCRIPTORCONTAINER_H
