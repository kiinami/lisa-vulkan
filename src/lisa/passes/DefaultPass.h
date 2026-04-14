//
// Created by kinami on 4/5/26.
//

#ifndef LISA_VULKAN_DEFAULTPASS_H
#define LISA_VULKAN_DEFAULTPASS_H
#include "graphics/descriptors/DescriptorContainer.h"
#include "systems/render/RenderPass.h"

namespace lisa::passes {

  class DefaultPass : public systems::render::RenderPass {
  public:
    explicit DefaultPass(const str& name);
    ~DefaultPass() override = default;

    void render(const RenderPassInput& in) override;
  };

}

#endif // LISA_VULKAN_DEFAULTPASS_H
