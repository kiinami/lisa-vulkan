//
// Created by kinami on 4/3/26.
//

#ifndef LISA_VULKAN_SHADER_H
#define LISA_VULKAN_SHADER_H
#include "systems/resources/Resource.h"
#include "utils/common.h"

#include <slang/slang-com-helper.h>
#include <slang/slang-com-ptr.h>
#include <vulkan/vulkan_raii.hpp>

namespace lisa::resources {

  class Shader : public systems::resources::Resource {
  public:
    inline static const path SHADERS_PATH = "src/lisa/shaders";

    struct ShaderStage {
      vk::ShaderStageFlagBits stage;
      const char* entry_point;
    };

    explicit Shader(const path& filepath) : Resource(filepath) {}

    const vk::raii::ShaderModule& module() const { return module_; }

    const vector<ShaderStage>& stages() const { return stages_; }

  protected:
    bool load_function() override;
    bool unload_function() override;

    str type_name() override { return "Shader"; }

  private:
    vk::raii::ShaderModule module_ = nullptr;
    vector<ShaderStage> stages_;

    static const Slang::ComPtr<slang::IGlobalSession>& get_global_session();
    static Slang::ComPtr<slang::ISession> create_session();
  };
}

#endif // LISA_VULKAN_SHADER_H
