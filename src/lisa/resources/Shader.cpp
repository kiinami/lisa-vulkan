//
// Created by kinami on 4/3/26.
//

#include "Shader.h"

#include "constants.h"
#include "graphics/context.h"
#include "utils/logging.h"

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
    auto search_path_str = constants::SHADERS_PATH.string();
    const char* search_paths[] = {search_path_str.c_str()};

    const slang::SessionDesc session_desc{
      .targets = targets.data(),
      .targetCount = static_cast<SlangInt>(targets.size()),
      .defaultMatrixLayoutMode = SLANG_MATRIX_LAYOUT_COLUMN_MAJOR,
      .searchPaths = search_paths,
      .searchPathCount = 1,
      .compilerOptionEntries = options.data(),
      .compilerOptionEntryCount = static_cast<uint32>(options.size())
    };

    Slang::ComPtr<slang::ISession> session;
    global_session->createSession(session_desc, session.writeRef());

    return session;
  }

  bool Shader::load_function() {
    logging::info("Shader::load_function called for id: {}", id_);
    const path filepath = constants::SHADERS_PATH / (id_ + ".slang");
    logging::info("Shader filepath: {}", filepath.string());

    const auto session = create_session();
    logging::info("Shader session created");
    Slang::ComPtr<ISlangBlob> load_diagnostics;
    const auto module =
      session->loadModule(id_.c_str(), load_diagnostics.writeRef());
    if (load_diagnostics) {
      logging::info(
        "Slang load diagnostics: {}",
        (const char*) load_diagnostics->getBufferPointer()
      );
    }
    if (!module) {
      logging::error("Failed to load shader module: {}", id_);
      return false;
    }
    logging::info("Shader module loaded");

    vector<slang::IComponentType*> components;
    components.push_back(module);
    const SlangInt defined_entry_point_count =
      module->getDefinedEntryPointCount();
    logging::info("Defined entry point count: {}", defined_entry_point_count);
    vector<Slang::ComPtr<slang::IEntryPoint>> entry_points;
    for (SlangInt i = 0; i < defined_entry_point_count; i++) {
      Slang::ComPtr<slang::IEntryPoint> entry_point;
      module->getDefinedEntryPoint(i, entry_point.writeRef());
      entry_points.push_back(entry_point);
      components.push_back(entry_point.get());
    }

    Slang::ComPtr<slang::IComponentType> program;
    logging::info("Creating composite component type...");
    session->createCompositeComponentType(
      components.data(), components.size(), program.writeRef()
    );
    logging::info("Composite component type created");

    Slang::ComPtr<ISlangBlob> spirv;
    Slang::ComPtr<ISlangBlob> diagnostics;
    logging::info("Getting target code...");
    program->getTargetCode(0, spirv.writeRef(), diagnostics.writeRef());
    if (diagnostics) {
      logging::info(
        "Slang diagnostics: {}", (const char*) diagnostics->getBufferPointer()
      );
    }
    logging::info("Target code acquired");

    const vk::ShaderModuleCreateInfo module_ci{
      .codeSize = spirv->getBufferSize(),
      .pCode = (uint32*) spirv->getBufferPointer()
    };
    logging::info("Creating Vulkan shader module...");
    module_ = graphics::context::device()->createShaderModule(module_ci);
    logging::info("Vulkan shader module created");

    slang::ShaderReflection* reflection = program->getLayout();
    uint32 entry_point_count = reflection->getEntryPointCount();

    stages_.reserve(entry_point_count);

    for (uint32 i = 0; i < entry_point_count; i++) {
      slang::EntryPointReflection* entry_point =
        reflection->getEntryPointByIndex(i);

      SlangStage slang_stage = entry_point->getStage();
      const char* entry_name = entry_point->getNameOverride();
      vk::ShaderStageFlagBits vk_stage;
      switch (slang_stage) {
        case SLANG_STAGE_VERTEX:
          vk_stage = vk::ShaderStageFlagBits::eVertex;
          break;
        case SLANG_STAGE_FRAGMENT:
          vk_stage = vk::ShaderStageFlagBits::eFragment;
          break;
        case SLANG_STAGE_COMPUTE:
          vk_stage = vk::ShaderStageFlagBits::eCompute;
          break;
        case SLANG_STAGE_GEOMETRY:
          vk_stage = vk::ShaderStageFlagBits::eGeometry;
          break;
        case SLANG_STAGE_RAY_GENERATION:
          vk_stage = vk::ShaderStageFlagBits::eRaygenKHR;
          break;
        default:
          logging::abort(
            "Shader stage of entry point '{}' not supported", entry_name
          );
      }

      stages_.push_back({.stage = vk_stage, .entry_point = entry_name});
    }

    return true;
  }

  bool Shader::unload_function() { return true; }
}
