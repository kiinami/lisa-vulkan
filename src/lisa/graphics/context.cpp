//
// Created by kinami on 3/24/26.
//

#include "context.h"

#include "device/Instance.h"
#include "device/LogicalDevice.h"
#include "swapchain/Surface.h"
#include "swapchain/Swapchain.h"
#include "utils/logging.h"
#include "window/context.h"

#include <memory>

namespace lisa::graphics::context {
  namespace {
    std::unique_ptr<vk::raii::Context> context_;
    std::unique_ptr<Instance> instance_;
    std::unique_ptr<PhysicalDevice> physical_device_;
    std::unique_ptr<LogicalDevice> device_;
    std::unique_ptr<MemoryAllocator> allocator_;
    std::unique_ptr<Surface> surface_;
    std::unique_ptr<Swapchain> swapchain_;
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

    logging::debug("Graphics context initiated");
  }

  void destroy() {
    swapchain_.reset();
    surface_.reset();
    allocator_.reset();
    device_.reset();
    physical_device_.reset();
    instance_.reset();
    context_.reset();
  }

  const Instance& instance() { return *instance_; }

  const PhysicalDevice& physical_device() { return *physical_device_; }

  const LogicalDevice& device() { return *device_; }

  const MemoryAllocator& allocator() { return *allocator_; }
}
