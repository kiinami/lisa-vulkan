//
// Created by kinami on 3/24/26.
//

#include "context.h"

#include "device/Instance.h"
#include "device/LogicalDevice.h"

#include <memory>

namespace lisa::graphics::context {
  namespace {
    std::unique_ptr<vk::raii::Context> context_;
    std::unique_ptr<Instance> instance_;
    std::unique_ptr<PhysicalDevice> physical_device_;
    std::unique_ptr<LogicalDevice> device_;
  }

  vk::raii::Context& context() { return *context_; }
  Instance& instance() { return *instance_; }
  const vk::raii::Instance& vk_instance() { return instance_->vk_instance(); }
  const vk::raii::PhysicalDevice& vk_physical_device() { return physical_device_->vk_physical_device(); }

  void init() {
    context_ = std::make_unique<vk::raii::Context>();
    instance_ = std::make_unique<Instance>(*context_);
    physical_device_ =
      std::make_unique<PhysicalDevice>(instance_->pick_physical_device());
    device_ = std::make_unique<LogicalDevice>(vk_physical_device());
  }

  void destroy() {
    device_.reset();
    physical_device_.reset();
    instance_.reset();
    context_.reset();
  }
}
