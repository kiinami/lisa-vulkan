//
// Created by kinami on 3/24/26.
//

#include "context.h"

#include "graphics/constants.h"
#include "utils/logging.h"
#include "window/context.h"
#include "descriptors/DescriptorContainer.h"

#include <memory>
#include <pugixml.hpp>

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

  namespace {
    VpProfileProperties read_rendergraph_profile(const path& filepath) {
      pugi::xml_document doc;
      if (!doc.load_file(filepath.c_str()))
        logging::abort("Failed to open rendergraph '{}'", filepath.string());
      const str name = doc.document_element().attribute("profile").as_string();
      if (name.empty())
        logging::abort(
          "Rendergraph '{}' missing required 'profile' attribute",
          filepath.string()
        );
      auto profile = constants::resolve_profile(name);
      if (!profile)
        logging::abort(
          "Unknown profile '{}' in rendergraph '{}'", name, filepath.string()
        );
      return *profile;
    }
  }

  void init(const path& rendergraph_path) {
    constants::set_active_profile(read_rendergraph_profile(rendergraph_path));
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
