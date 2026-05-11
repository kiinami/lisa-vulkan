//
// Created by kinami on 4/1/26.
//

#ifndef LISA_VULKAN_SURFACE_H
#define LISA_VULKAN_SURFACE_H
#include "graphics/device/Instance.h"
#include "graphics/device/PhysicalDevice.h"
#include "graphics/images/ImageFormat.h"
#include "window/Window.h"

#include <vulkan/vulkan_raii.hpp>

namespace lisa::graphics {

  class Surface : public VkObject<vk::raii::SurfaceKHR> {
  public:
    Surface(
      const window::Window& window,
      const Instance& instance,
      const PhysicalDevice& physical_device
    );

    const vk::SurfaceCapabilitiesKHR& capabilities() const {
      return capabilities_;
    }

    pair<ImageFormat, vk::ColorSpaceKHR>
      image_format(const vk::raii::PhysicalDevice& device) const;

  private:
    vk::SurfaceCapabilitiesKHR capabilities_;
  };

}

#endif // LISA_VULKAN_SURFACE_H
