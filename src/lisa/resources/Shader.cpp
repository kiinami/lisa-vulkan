//
// Created by kinami on 4/3/26.
//

#include "Shader.h"

#include "graphics/context.h"
#include "utils/logging.h"
#include "utils/path.h"

namespace lisa::resources {
  const Slang::ComPtr<slang::IGlobalSession>& Shader::get_global_session() {
    static auto s = []() {
      Slang::ComPtr<slang::IGlobalSession> global_session;
      const SlangResult result =
        slang::createGlobalSession(global_session.writeRef());
      if (SLANG_FAILED(result) || !global_session)
        logging::abort("Failed to create Slang global session");
      return global_session;
    }();
    return s;
  }

  Slang::ComPtr<slang::ISession> Shader::create_session() {
    const auto& global_session = get_global_session();

    static const str search_path_str = build::shaders_path.string();
    static const char* search_paths[] = {search_path_str.c_str()};

    static const auto targets = []() {
      return std::to_array<slang::TargetDesc>(
        {{.format = SLANG_SPIRV,
          .profile = get_global_session()->findProfile("spirv_1_5")}}
      );
    }();

    static auto options = std::to_array<slang::CompilerOptionEntry>(
      {{slang::CompilerOptionName::EmitSpirvDirectly,
        {slang::CompilerOptionValueKind::Int, 1}}}
    );

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
    const SlangResult result =
      global_session->createSession(session_desc, session.writeRef());
    if (SLANG_FAILED(result) || !session)
      logging::abort("Failed to create Slang session");

    return session;
  }

  Shader::Shader(const str& id, const path& filepath) : Resource(id) {
    if (!exists(filepath)) {
      logging::abort("Shader file '{}' does not exist", filepath.string());
      return;
    }

    const auto session = create_session();
    if (!session) {
      logging::abort("Slang session creation returned null");
      return;
    }
    Slang::ComPtr<ISlangBlob> load_diagnostics;
    const auto module = session->loadModule(
      utils::pstr(filepath).c_str(), load_diagnostics.writeRef()
    );
    if (!module) {
      if (load_diagnostics) {
        logging::abort(
          "Failed to load shader module at {}\n{}",
          utils::pstr(filepath),
          static_cast<const char*>(load_diagnostics->getBufferPointer())
        );
      }
      logging::abort(
        "Failed to load shader module at {}", utils::pstr(filepath)
      );
      return;
    }

    vector<slang::IComponentType*> components;
    components.push_back(module);
    const SlangInt defined_entry_point_count =
      module->getDefinedEntryPointCount();
    vector<Slang::ComPtr<slang::IEntryPoint>> entry_points;
    for (SlangInt i = 0; i < defined_entry_point_count; i++) {
      Slang::ComPtr<slang::IEntryPoint> entry_point;
      module->getDefinedEntryPoint(i, entry_point.writeRef());
      entry_points.push_back(entry_point);
      components.push_back(entry_point.get());
    }

    Slang::ComPtr<slang::IComponentType> program;
    {
      Slang::ComPtr<ISlangBlob> compose_diagnostics;
      const SlangResult result = session->createCompositeComponentType(
        components.data(),
        static_cast<SlangInt>(components.size()),
        program.writeRef(),
        compose_diagnostics.writeRef()
      );
      if (SLANG_FAILED(result) || !program) {
        if (compose_diagnostics) {
          logging::abort(
            "Failed to compose shader program for {}\n{}",
            utils::pstr(filepath),
            static_cast<const char*>(compose_diagnostics->getBufferPointer())
          );
        }
        logging::abort("Failed to compose shader program for {}", utils::pstr(filepath));
        return;
      }
    }

    Slang::ComPtr<ISlangBlob> spirv;
    Slang::ComPtr<ISlangBlob> diagnostics;
    {
      const SlangResult result =
        program->getTargetCode(0, spirv.writeRef(), diagnostics.writeRef());
      if (SLANG_FAILED(result) || !spirv) {
        if (diagnostics) {
          logging::abort(
            "Failed to compile shader {}\n{}",
            utils::pstr(filepath),
            static_cast<const char*>(diagnostics->getBufferPointer())
          );
        }
        logging::abort("Failed to compile shader {}", utils::pstr(filepath));
        return;
      }
    }

    slang::ShaderReflection* reflection = program->getLayout();
    if (!reflection) {
      logging::abort("Failed to reflect shader layout for {}", utils::pstr(filepath));
      return;
    }
    const uint32 entry_point_count = reflection->getEntryPointCount();

    vector<graphics::ShaderStage> stages;
    stages.reserve(entry_point_count);

    for (uint32 i = 0; i < entry_point_count; i++) {
      slang::EntryPointReflection* entry_point =
        reflection->getEntryPointByIndex(i);

      const SlangStage slang_stage = entry_point->getStage();
      str entry_name = entry_point->getNameOverride();
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

      stages.push_back({.stage = vk_stage, .entry_point = entry_name});
    }

    module_ = graphics::ShaderModule(
      id, spirv->getBufferSize(), (uint32*) spirv->getBufferPointer(), stages
    );
  }
}
