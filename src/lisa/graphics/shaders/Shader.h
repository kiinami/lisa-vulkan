//
// Created by kinami on 4/3/26.
//

#ifndef LISA_VULKAN_SHADER_H
#define LISA_VULKAN_SHADER_H

#include <filesystem>
#include <slang.h>
#include <slang/slang-com-helper.h>
#include <slang/slang-com-ptr.h>
#include <vulkan/vulkan_raii.hpp>

namespace lisa::graphics {

  class Shader {
  public:
    Shader(
      const std::filesystem::path& filepath, const std::string& module_name
    );
    ~Shader();

    static const Slang::ComPtr<slang::IGlobalSession>& get_global_session();
    static Slang::ComPtr<slang::ISession> create_session();

  private:
    vk::raii::ShaderModule module_ = nullptr;
  };

}

#endif // LISA_VULKAN_SHADER_H
