//
// Created by kinami on 3/24/26.
//

#include "context.h"

#include "utils/logging.h"
#include "window/context.h"
#include "descriptors/DescriptorContainer.h"

#include <memory>

namespace lisa::graphics::context {
  namespace {
    uptr<vk::raii::Context> context_;
    uptr<Instance> instance_;
    uptr<PhysicalDevice> physical_device_;
    uptr<LogicalDevice> device_;
    uptr<MemoryAllocator> allocator_;
    uptr<Surface> surface_;
    uptr<Swapchain> swapchain_;
    uptr<DescriptorContainer> descriptor_container_;
  }

  void init() {
    context_ = std::make_unique<vk::raii::Context>();
    instance_ = std::make_unique<Instance>(*context_);
    physical_device_ =
      std::make_unique<PhysicalDevice>(instance_->pick_physical_device());
    device_ = std::make_unique<LogicalDevice>(*physical_device_);
    allocator_ = std::make_unique<MemoryAllocator>(
      *instance_, *physical_device_, *device_
    );
    surface_ = std::make_unique<Surface>(
      window::context::window(), *instance_, *physical_device_
    );
    swapchain_ = std::make_unique<Swapchain>(*surface_, *device_);
    descriptor_container_ = std::make_unique<DescriptorContainer>(
      10000, vk::DescriptorType::eCombinedImageSampler
    );

    logging::debug("Graphics context initiated");
  }

  void destroy() {
    descriptor_container_.reset();
    swapchain_.reset();
    surface_.reset();
    allocator_.reset();
    device_.reset();
    physical_device_.reset();
    instance_.reset();
    context_.reset();
    logging::debug("Graphics context destroyed");
  }

  const Instance& instance() { return *instance_; }

  const PhysicalDevice& physical_device() { return *physical_device_; }

  const LogicalDevice& device() { return *device_; }

  const MemoryAllocator& allocator() { return *allocator_; }

  const Surface& surface() { return *surface_; }

  Swapchain& swapchain() { return *swapchain_; }

  DescriptorContainer& descriptor_container() { return *descriptor_container_; }

  void recreate_swapchain() {
    swapchain_.reset();
    swapchain_ = std::make_unique<Swapchain>(*surface_, *device_);
  }
}
