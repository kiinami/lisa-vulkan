//
// Created by kinami on 4/3/26.
//

#include "Shader.h"

#include "graphics/context.h"
#include "utils/common.h"
#include "utils/logging.h"

#include <array>

namespace lisa::graphics {
  Shader::Shader(
    const std::filesystem::path& filepath, const str& module_name
  ) {
    const auto session = create_session();
    const auto slang_module = session->loadModuleFromSource(
      module_name.c_str(), filepath.c_str(), nullptr, nullptr
    );

    Slang::ComPtr<ISlangBlob> spirv;
    slang_module->getTargetCode(0, spirv.writeRef());

    const vk::ShaderModuleCreateInfo module_ci{
      .codeSize = spirv->getBufferSize(),
      .pCode = (uint32*) spirv->getBufferPointer()
    };
    module_ = context::device()->createShaderModule(module_ci);
    logging::trace("Loaded shader at path '{}'", filepath.c_str());
  }

  Shader::~Shader() {}

  const Slang::ComPtr<slang::IGlobalSession>& Shader::get_global_session() {
    static auto s = []() {
      Slang::ComPtr<slang::IGlobalSession> global_session;
      slang::createGlobalSession(global_session.writeRef());
      return global_session;
    }();
    return s;
  }

  Slang::ComPtr<slang::ISession> Shader::create_session() {
    const auto& global_session = get_global_session();

    auto targets = std::to_array<slang::TargetDesc>(
      {{.format = SLANG_SPIRV, .profile = global_session->findProfile("spirv_1_4")}}
    );
    auto options = std::to_array<slang::CompilerOptionEntry>(
      {{slang::CompilerOptionName::EmitSpirvDirectly,
        {slang::CompilerOptionValueKind::Int, 1}}}
    );
    const slang::SessionDesc session_desc{
      .targets = targets.data(),
      .targetCount = static_cast<SlangInt>(targets.size()),
      .defaultMatrixLayoutMode = SLANG_MATRIX_LAYOUT_COLUMN_MAJOR,
      .compilerOptionEntries = options.data(),
      .compilerOptionEntryCount = static_cast<uint32>(options.size())
    };

    Slang::ComPtr<slang::ISession> session;
    global_session->createSession(session_desc, session.writeRef());

    return session;
  }
}
