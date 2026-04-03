//
// Created by kinami on 4/3/26.
//

#include "Renderer.h"

#include "graphics/context.h"

namespace lisa::systems::render {
  void Renderer::render() {
    fence_.wait();
    fence_.reset();
    cmd_buffer_.reset();
  }
}
