//
// Created by kinami on 4/3/26.
//

#include "DescriptorContainer.h"

#include "graphics/context.h"

namespace lisa::graphics {
  DescriptorContainer::DescriptorContainer(
    const uint32 size, const vk::DescriptorType type
  ) :
    size_(size),
    type_(type),
    index_allocator_(size),
    dummy_image_(create_dummy_image()),
    dummy_sampler_(create_dummy_sampler()),
    pool_(create_pool(size, type)),
    layout_(create_layout(size, type)),
    set_(create_set(pool_, layout_, size)) {}

  DescriptorIndex
    DescriptorContainer::write(const vk::DescriptorImageInfo& image_info) {
    const auto index = index_allocator_.allocate();
    const vk::WriteDescriptorSet write_desc{
      .dstSet = set_,
      .dstBinding = 0,
      .dstArrayElement = index,
      .descriptorCount = 1,
      .descriptorType = type_,
      .pImageInfo = &image_info
    };
    context::device()->updateDescriptorSets(write_desc, nullptr);
    return index;
  }

  void DescriptorContainer::free(const DescriptorIndex descriptor) {
    index_allocator_.free(descriptor);
    write_null(descriptor);
  }

  Image DescriptorContainer::create_dummy_image() {
    return Image(
      "descriptor_dummy_image", // TODO: add ID
      ImageFormat(vk::Format::eR8G8B8A8Unorm),
      vk::ImageUsageFlagBits::eSampled,
      vec3(1, 1, 1),
      vk::ImageType::e2D,
      1,
      vk::ImageLayout::eUndefined
    );
  }

  Sampler DescriptorContainer::create_dummy_sampler() {
    return Sampler(
      "descriptor_dummy_sampler", // TODO: add ID
      0.0f,
      vk::Filter::eNearest,
      vk::Filter::eNearest,
      vk::SamplerMipmapMode::eNearest,
      false
    );
  }

  void DescriptorContainer::transition_dummy_image() const {
    const auto cmdb = context::device().cmd_buffer();
    cmdb.begin_onetime();

    cmdb->pipelineBarrier(
      vk::PipelineStageFlagBits::eTopOfPipe,
      vk::PipelineStageFlagBits::eFragmentShader,
      {},
      {},
      {},
      vk::ImageMemoryBarrier{
        .srcAccessMask = {},
        .dstAccessMask = vk::AccessFlagBits::eShaderRead,
        .oldLayout = vk::ImageLayout::eUndefined,
        .newLayout = vk::ImageLayout::eShaderReadOnlyOptimal,
        .image = static_cast<const vk::Image&>(dummy_image_),
        .subresourceRange = {
          .aspectMask = vk::ImageAspectFlagBits::eColor,
          .baseMipLevel = 0,
          .levelCount = 1,
          .baseArrayLayer = 0,
          .layerCount = 1,
        },
      }
    );

    cmdb->end();
    context::device().submit_cmd_buffer_with_fence(cmdb);
  }

  void DescriptorContainer::write_null(const DescriptorIndex index) const {
    const ImageViewDesc dummy_view_desc{
      .type = vk::ImageViewType::e2D,
      .format = ImageFormat(vk::Format::eR8G8B8A8Unorm),
      .range = {
        .aspectMask = vk::ImageAspectFlagBits::eColor,
        .baseMipLevel = 0,
        .levelCount = 1,
        .baseArrayLayer = 0,
        .layerCount = 1,
      }
    };
    const vk::DescriptorImageInfo null_info{
      .sampler = dummy_sampler_.handle(),
      .imageView = *dummy_image_.view(dummy_view_desc),
      .imageLayout = vk::ImageLayout::eShaderReadOnlyOptimal,
    };
    context::device()->updateDescriptorSets(
      vk::WriteDescriptorSet{
        .dstSet = *set_,
        .dstBinding = 0,
        .dstArrayElement = index,
        .descriptorCount = 1,
        .descriptorType = type_,
        .pImageInfo = &null_info,
      },
      nullptr
    );
  }

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
