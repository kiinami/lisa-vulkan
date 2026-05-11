//
// Created by kinami on 5/11/26.
//

#include "ShaderModule.h"

namespace lisa::graphics {
  ShaderModule::ShaderModule(
    const str& id,
    const size code_size,
    const uint32* code,
    vector<ShaderStage> stages
  ) :
    NamedVkObject(id),
    stages_(std::move(stages)) {
    const vk::ShaderModuleCreateInfo module_ci{
      .codeSize = code_size, .pCode = code
    };
    set(context::device()->createShaderModule(module_ci));
  }
}
