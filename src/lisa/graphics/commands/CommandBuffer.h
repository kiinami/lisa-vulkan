//
// Created by kinami on 4/2/26.
//

#ifndef LISA_VULKAN_COMMANDBUFFER_H
#define LISA_VULKAN_COMMANDBUFFER_H

#include "graphics/vk/VkObject.h"
#include "utils/common.h"

#include <any>
#include <vulkan/vulkan_raii.hpp>

namespace lisa::graphics {

  class CommandBuffer : public VkObject<vk::raii::CommandBuffer> {
  public:
    explicit CommandBuffer(vk::raii::CommandBuffer&& buf) :
      VkObject(std::move(buf)) {}

    template<typename T> void keep_alive(T&& object) const {
      dependencies_.emplace_back(
        std::make_shared<std::decay_t<T>>(std::forward<T>(object))
      );
    }

    void reset() const;
    void begin_onetime() const;

    void begin_region(
      const str& name, const rgba& color = {1.0, 1.0, 1.0, 1.0}
    ) const;
    void end_region() const;

  private:
    mutable vector<std::shared_ptr<void>> dependencies_;
  };

}

#endif // LISA_VULKAN_COMMANDBUFFER_H
