//
// Created by kinami on 4/1/26.
//

#include "Surface.h"

#include "SDL3pp/SDL3pp_vulkan.h"
#include "graphics/context.h"
#include "utils/chk.h"
#include "window/Window.h"

namespace lisa::graphics {
  Surface::Surface(
    const window::Window& window,
    const Instance& instance,
    const PhysicalDevice& physical_device
  ) {
    VkSurfaceKHR c_surface;
    SDL::Vulkan_CreateSurface(window, instance, nullptr, &c_surface);
    surface_ = vk::raii::SurfaceKHR(instance, c_surface);
    capabilities_ = physical_device.surface_capabilities(surface_);
  }

  std::pair<ImageFormat, vk::ColorSpaceKHR>
    Surface::image_format(const vk::raii::PhysicalDevice& device) const {
    for (const auto [format, colorSpace] : device.getSurfaceFormatsKHR(surface_))
      if (format == vk::Format::eB8G8R8A8Srgb) return {ImageFormat(format), colorSpace};

    logging::abort("No surface format supported");
  }
}
