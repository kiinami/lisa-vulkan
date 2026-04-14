//
// Created by kinami on 4/4/26.
//

#ifndef LISA_VULKAN_SEMAPHORE_H
#define LISA_VULKAN_SEMAPHORE_H

#include <vulkan/vulkan_raii.hpp>

namespace lisa::graphics {

  class Semaphore {
  public:
    explicit Semaphore(const vk::SemaphoreCreateFlags flags = {});

    ~Semaphore() = default;

    Semaphore(const Semaphore&) = delete;
    Semaphore& operator=(const Semaphore&) = delete;

    Semaphore(Semaphore&&) noexcept = default;
    Semaphore& operator=(Semaphore&&) noexcept = default;

    operator const vk::raii::Semaphore&() { return semaphore_; }

    operator vk::Semaphore() { return semaphore_; }

    vk::Semaphore operator*() const { return *semaphore_; }

  private:
    vk::raii::Semaphore semaphore_;
  };

}

#endif // LISA_VULKAN_SEMAPHORE_H
