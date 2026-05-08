//
// Created by kinami on 5/8/26.
//

#ifndef LISA_VULKAN_SHADERPASS_H
#define LISA_VULKAN_SHADERPASS_H

#include "resources/Shader.h"
#include "resources/context.h"
#include "systems/render/RenderPass.h"
#include "utils/common.h"

namespace lisa::render {

  class ShaderPass : public systems::render::RenderPass {
  public:
    using RenderPass::RenderPass;

    ~ShaderPass() override = default;

  protected:
    str shader_id() const { return shader_id_; }

    void set_shader(const path& shader_path) {
      auto p = resources::Shader::SHADERS_PATH / shader_path;
      shader_id_ = resources::context::manager()
                     .add<resources::Shader, resources::ShaderSpec>(
                       shader_path.string(), p
                     );
      resources::context::manager().load<resources::Shader>(shader_id_);
    }

    resources::Shader* shader() const {
      if (shader_id_.empty())
        throw std::runtime_error("Shader not set for pass " + id());
      return resources::context::manager().get<resources::Shader>(shader_id_);
    }

  private:
    str shader_id_;
  };

}

#endif // LISA_VULKAN_SHADERPASS_H
