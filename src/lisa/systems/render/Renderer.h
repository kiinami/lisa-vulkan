//
// Created by kinami on 4/3/26.
//

#ifndef LISA_VULKAN_RENDERER_H
#define LISA_VULKAN_RENDERER_H

#include "Rendergraph.h"
#include "graphics/buffer/Buffer.h"
#include "graphics/constants.h"
#include "graphics/swapchain/Swapchain.h"
#include "graphics/sync/Fence.h"
#include "graphics/sync/Semaphore.h"
#include "utils/common.h"

namespace lisa::systems::render {

  class Renderer {
  public:
    explicit Renderer(const path& graph_filepath);
    ~Renderer() = default;

    void render();

    uint32 current_frame() const { return current_frame_; }

    graphics::Buffer& global_view_buffer() {
      return global_data_buffers_[current_frame_];
    }

    graphics::Buffer& object_data_buffer() {
      return object_data_buffers_[current_frame_];
    }

  private:
    Rendergraph graph_;

    graphics::Fence fence_;
    graphics::CommandBuffer cmd_buffer_;
    graphics::Swapchain& swapchain_;

    array<graphics::Buffer, graphics::constants::MAX_FRAMES_IN_FLIGHT>
      global_data_buffers_;
    array<graphics::Buffer, graphics::constants::MAX_FRAMES_IN_FLIGHT>
      object_data_buffers_;
    array<graphics::Buffer, graphics::constants::MAX_FRAMES_IN_FLIGHT>
      point_lights_buffers_;
    array<graphics::Buffer, graphics::constants::MAX_FRAMES_IN_FLIGHT>
      dir_lights_buffers_;
    array<graphics::Buffer, graphics::constants::MAX_FRAMES_IN_FLIGHT>
      ambient_lights_buffers_;

    array<graphics::Semaphore, graphics::constants::MAX_FRAMES_IN_FLIGHT>
      available_s_;
    vector<graphics::Semaphore> finished_s_;

    uint32 current_frame_ = 0;
    float time_ = 0.0f;

    void reset() const;
    void submit_to_queue(uint32 swapchain_image_index) const;
  };

}

#endif // LISA_VULKAN_RENDERER_H
