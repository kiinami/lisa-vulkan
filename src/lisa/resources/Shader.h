//
// Created by kinami on 4/3/26.
//

#ifndef LISA_VULKAN_SHADER_H
#define LISA_VULKAN_SHADER_H
#include "graphics/pipeline/ShaderModule.h"
#include "systems/resources/Resource.h"
#include "utils/common.h"

#include <slang/slang-com-helper.h>
#include <slang/slang-com-ptr.h>

namespace lisa::resources {

  class Shader : public systems::resources::Resource {
  public:
    explicit Shader(const str& id, const path& filepath);

    const graphics::ShaderModule& module() const { return module_; }

    const vector<graphics::ShaderStage>& stages() const {
      return module_.stages();
    }

    void unload() override {}

  private:
    graphics::ShaderModule module_;

    static const Slang::ComPtr<slang::IGlobalSession>& get_global_session();
    static Slang::ComPtr<slang::ISession> create_session();
  };

  struct ShaderSpec : systems::resources::ResourceSpec<Shader> {
    path filepath;

    explicit ShaderSpec(const str& id, const path& filepath) :
      ResourceSpec(id),
      filepath(filepath) {}

    Shader load_resource(const graphics::CommandBuffer& cmdb) override {
      return Shader(id, filepath);
    }
  };
}

#endif // LISA_VULKAN_SHADER_H
