//
// Created by kinami on 4/3/26.
//

#ifndef LISA_VULKAN_SHADER_H
#define LISA_VULKAN_SHADER_H
#include "systems/resources/Resource.h"
#include "utils/common.h"

#include <slang.h>
#include <slang/slang-com-helper.h>
#include <slang/slang-com-ptr.h>
#include <vulkan/vulkan_raii.hpp>

namespace lisa::resources {

  class Shader : public systems::resources::Resource {
  public:
    explicit Shader(const str& id) : Resource(id) {}

  protected:
    bool load_function() override;
    bool unload_function() override;

    str type_name() override { return "Shader"; }

  private:
    vk::raii::ShaderModule module_ = nullptr;

    static const Slang::ComPtr<slang::IGlobalSession>& get_global_session();
    static Slang::ComPtr<slang::ISession> create_session();
  };

}

#endif // LISA_VULKAN_SHADER_H
