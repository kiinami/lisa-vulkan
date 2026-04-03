//
// Created by kinami on 4/3/26.
//

#include "Shader.h"

#include "constants.h"
#include "graphics/context.h"

namespace lisa::resources {
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
      {{.format = SLANG_SPIRV,
        .profile = global_session->findProfile("spirv_1_4")}}
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

  bool Shader::load_function() {
    const path filepath = constants::SHADERS_PATH / (id_ + ".slang");

    const auto session = create_session();
    const auto module = session->loadModuleFromSource(
      id_.c_str(), filepath.c_str(), nullptr, nullptr
    );

    Slang::ComPtr<ISlangBlob> spirv;
    module->getTargetCode(0, spirv.writeRef());

    const vk::ShaderModuleCreateInfo module_ci{
      .codeSize = spirv->getBufferSize(),
      .pCode = (uint32*) spirv->getBufferPointer()
    };
    module_ = graphics::context::device()->createShaderModule(module_ci);

    return true;
  }

  bool Shader::unload_function() { return true; }
}
