//
// Created by kinami on 5/11/26.
//

#ifndef LISA_VULKAN_SHADERMODULE_H
#define LISA_VULKAN_SHADERMODULE_H
#include "graphics/vk/NamedVkObject.h"

namespace lisa::graphics {

  struct ShaderStage {
    vk::ShaderStageFlagBits stage;
    str entry_point;
  };

  class ShaderModule : public NamedVkObject<vk::raii::ShaderModule> {
  public:
    ShaderModule() = default;

    explicit ShaderModule(
      const str& id,
      size code_size,
      const uint32* code,
      vector<ShaderStage> stages
    );

    const vector<ShaderStage>& stages() const { return stages_; }

  private:
    vector<ShaderStage> stages_;
  };

}

#endif // LISA_VULKAN_SHADERMODULE_H
