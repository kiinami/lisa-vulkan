//
// Created by kinami on 3/24/26.
//

#ifndef LISA_INSTANCE_H
#define LISA_INSTANCE_H
#include <vulkan/vulkan_core.h>

namespace lisa {
  class Instance {
  public:
    Instance();
    ~Instance();

    [[nodiscard]]
    VkInstance vk_instance() const {
      return instance_;
    }

  private:
    VkInstance instance_ = VK_NULL_HANDLE;
  };
} // namespace lisa

#endif // LISA_INSTANCE_H
