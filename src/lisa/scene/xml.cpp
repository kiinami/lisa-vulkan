//
// Created by kinami on 5/2/26.
//

#include "xml.h"

#include "components/AmbientLightComponent.h"
#include "components/CameraComponent.h"
#include "components/DirectionalLightComponent.h"
#include "components/MaterialComponent.h"
#include "components/MeshComponent.h"
#include "components/PointLightComponent.h"
#include "components/TransformComponent.h"
#include "components/context.h"
#include "gltf.h"
#include "resources/Mesh.h"
#include "resources/Texture.h"
#include "resources/context.h"
#include "utils/logging.h"

#include <rfl.hpp>
#include <rfl/xml.hpp>

namespace {
  std::vector<float> parse_floats(const std::string& str) {
    std::stringstream ss(str);
    std::string token;
    std::vector<float> vals;
    while (std::getline(ss, token, ',')) {
      try {
        vals.push_back(std::stof(token));
      } catch (...) { return {}; }
    }
    return vals;
  }
}

namespace rfl {
  template<> struct Reflector<glm::vec3> {
    using ReflType = std::string;

    static glm::vec3 to(const ReflType& str) {
      auto vals = ::parse_floats(str);
      if (vals.size() == 3) return glm::vec3(vals[0], vals[1], vals[2]);
      if (vals.size() == 1) return glm::vec3(vals[0]);
      throw std::runtime_error(
        "Expected 1 or 3 values for vec3, got: '" + str + "'"
      );
    }

    static ReflType from(const glm::vec3& v) {
      return std::to_string(v.x) +
             ", " +
             std::to_string(v.y) +
             ", " +
             std::to_string(v.z);
    }
  };

  template<> struct Reflector<glm::vec4> {
    using ReflType = std::string;

    static glm::vec4 to(const ReflType& str) {
      auto vals = ::parse_floats(str);
      if (vals.size() == 4)
        return glm::vec4(vals[0], vals[1], vals[2], vals[3]);
      if (vals.size() == 1) return glm::vec4(vals[0]);
      throw std::runtime_error(
        "Expected 1 or 4 values for vec4, got: '" + str + "'"
      );
    }

    static ReflType from(const glm::vec4& v) {
      return std::to_string(v.x) +
             ", " +
             std::to_string(v.y) +
             ", " +
             std::to_string(v.z) +
             ", " +
             std::to_string(v.w);
    }
  };

  template<> struct Reflector<glm::mat4> {
    using ReflType = std::string;

    static glm::mat4 to(const ReflType& str) {
      auto vals = ::parse_floats(str);
      if (vals.size() == 16) return glm::make_mat4(vals.data());
      throw std::runtime_error(
        "Expected 16 values for mat4, got: '" + str + "'"
      );
    }

    static ReflType from(const glm::mat4& v) { return ""; }
  };
}

namespace lisa::scene::xml {
  namespace {
    template<typename T>
    using ZeroToOne = rfl::Validator<T, rfl::Minimum<0>, rfl::Maximum<1>>;

    template<typename T>
    using Positive = rfl::Validator<T, rfl::ExclusiveMinimum<0>>;

    template<typename T>
    using FovRange =
      rfl::Validator<T, rfl::ExclusiveMinimum<0>, rfl::ExclusiveMaximum<180>>;

    template<typename T>
    using AspectRatio = rfl::Validator<T, rfl::ExclusiveMinimum<0>>;

    struct Translate {
      vec3 value;
    };

    struct Rotate {
      vec3 axis;
      float angle;
    };

    struct Scale {
      vec3 value;
    };

    struct LookAt {
      vec3 origin;
      vec3 target;
      vec3 up;
    };

    struct Matrix {
      mat4 value;
    };

    struct Transform {
      vector<Translate> translate;
      vector<Rotate> rotate;
      vector<Scale> scale;
      vector<LookAt> lookat;
      vector<Matrix> matrix;

      components::TransformComponent
        to_component(mat4 model = mat4(1.0f)) const {
        for (const auto& [value] : translate)
          model = glm::translate(model, value);

        for (const auto& [axis, angle] : rotate)
          model = glm::rotate(model, glm::radians(angle), axis);

        for (const auto& [value] : scale)
          model = glm::scale(model, value);

        for (const auto& [value] : matrix)
          model *= value;

        for (const auto& [origin, target, up] : lookat) {
          glm::mat4 view = glm::lookAt(origin, target, up);
          model *= glm::inverse(view);
        }

        components::TransformComponent component;
        component.set_matrix(model);
        return component;
      }
    };

    struct Mesh {
      str path;

      components::MeshComponent
        to_component(const std::filesystem::path& parent) const {
        return components::MeshComponent(
          resources::context::manager()
            .add<resources::Mesh, resources::MeshSpec>(path, parent / path)
        );
      }
    };

    struct Material {
      optional<vec3> albedo;
      optional<ZeroToOne<float>> roughness;
      optional<ZeroToOne<float>> metallic;

      optional<str> albedo_texture;
      optional<str> roughness_texture;
      optional<str> metallic_texture;
      optional<str> normal_texture;

      components::MaterialComponent
        to_component(const std::filesystem::path& parent) const {
        optional<str> albedo_tx = nullopt;
        optional<str> normal_tx = nullopt;
        optional<str> roughness_tx = nullopt;
        optional<str> metallic_tx = nullopt;

        if (albedo_texture.has_value())
          albedo_tx =
            resources::context::manager()
              .add<resources::Texture, resources::TextureSpec>(
                albedo_texture.value(), parent / albedo_texture.value()
              );
        if (normal_texture.has_value())
          normal_tx =
            resources::context::manager()
              .add<resources::Texture, resources::TextureSpec>(
                normal_texture.value(), parent / normal_texture.value()
              );
        if (roughness_texture.has_value())
          roughness_tx =
            resources::context::manager()
              .add<resources::Texture, resources::TextureSpec>(
                roughness_texture.value(), parent / roughness_texture.value()
              );
        if (metallic_texture.has_value())
          metallic_tx =
            resources::context::manager()
              .add<resources::Texture, resources::TextureSpec>(
                metallic_texture.value(), parent / metallic_texture.value()
              );

        return components::MaterialComponent(
          albedo.value_or(vec3(1.0f)),
          roughness.value_or(1.0f).get(),
          metallic.value_or(0.0f).get(),
          albedo_tx,
          normal_tx,
          roughness_tx,
          metallic_tx
        );
      }
    };

    struct Camera {
      optional<FovRange<float>> fov;
      optional<AspectRatio<float>> aspect_ratio;
      optional<Positive<float>> near_plane;
      optional<Positive<float>> far_plane;

      components::CameraComponent to_component() {
        components::CameraComponent component;
        if (fov) component.fov = glm::radians(fov.value().get());
        if (aspect_ratio) component.aspect_ratio = aspect_ratio.value().get();
        if (near_plane) component.near_plane = near_plane.value().get();
        if (far_plane) component.far_plane = far_plane.value().get();
        return component;
      }
    };

    struct DirectionalLight {
      vec3 color;
      Positive<float> intensity;

      components::DirectionalLightComponent to_component() {
        components::DirectionalLightComponent component;
        component.color = color;
        component.intensity = intensity.get();
        return component;
      }
    };

    struct PointLight {
      vec3 color;
      Positive<float> intensity;
      Positive<float> attenuation;
      Positive<float> radius;

      components::PointLightComponent to_component() {
        components::PointLightComponent component;
        component.color = color;
        component.intensity = intensity.get();
        component.attenuation = attenuation.get();
        component.radius = radius.get();
        return component;
      }
    };

    struct AmbientLight {
      vec3 color;
      Positive<float> intensity;

      components::AmbientLightComponent to_component() {
        components::AmbientLightComponent component;
        component.color = color;
        component.intensity = intensity.get();
        return component;
      }
    };

    struct Entity {
      optional<str> name;

      optional<Transform> transform;
      optional<Mesh> mesh;
      optional<Material> material;
      optional<Camera> camera;
      optional<DirectionalLight> directional_light;
      optional<PointLight> point_light;
      optional<AmbientLight> ambient_light;
    };

    struct External {
      path file;

      optional<Transform> transform;
    };

    struct Scene {
      vector<Entity> entity;
      vector<External> external;
    };

    str read_file(const path& path) {
      std::ifstream file(path, std::ios::binary);
      if (!file) throw std::runtime_error("Failed to open file");

      file.seekg(0, std::ios::end);
      str content(file.tellg(), '\0');
      file.seekg(0, std::ios::beg);
      file.read(content.data(), static_cast<std::streamsize>(content.size()));

      return content;
    }

  }

  void load(const path& filepath, const mat4& transform, const str& parent_id) {
    const auto result = rfl::xml::read<Scene>(read_file(filepath));
    const auto id =
      parent_id == "" ? "scene" : logging::genid(parent_id, filepath.string());
    if (!result)
      logging::abort("Failed to parse XML scene: {}", result.error().what());

    auto scene = result.value();

    if (scene.entity.empty() && scene.external.empty()) {
      logging::warning("Scene '{}' has no entities", filepath.string());
      return;
    }

    auto& reg = components::context::registry();
    auto scene_parent = filepath.parent_path();

    for (auto& xml_entity : scene.entity) {
      const auto entity = reg.create();

      if (xml_entity.transform.has_value()) {
        reg->emplace<components::TransformComponent>(
          entity, xml_entity.transform.value().to_component()
        );
      }

      if (xml_entity.mesh.has_value()) {
        reg->emplace<components::MeshComponent>(
          entity, xml_entity.mesh.value().to_component(scene_parent)
        );
      }

      if (xml_entity.material.has_value()) {
        reg->emplace<components::MaterialComponent>(
          entity, xml_entity.material.value().to_component(scene_parent)
        );
      }

      if (xml_entity.camera.has_value())
        reg->emplace<components::CameraComponent>(
          entity, xml_entity.camera.value().to_component()
        );

      if (xml_entity.directional_light.has_value())
        reg->emplace<components::DirectionalLightComponent>(
          entity, xml_entity.directional_light.value().to_component()
        );

      if (xml_entity.point_light.has_value())
        reg->emplace<components::PointLightComponent>(
          entity, xml_entity.point_light.value().to_component()
        );

      if (xml_entity.ambient_light.has_value())
        reg->emplace<components::AmbientLightComponent>(
          entity, xml_entity.ambient_light.value().to_component()
        );
    }

    for (const auto& [file, t] : scene.external) {
      mat4 external_transform(1.0f);
      if (t.has_value()) {
        auto component = t.value().to_component(transform);
        external_transform = component.matrix();
      }

      const auto ext = file.extension().string();
      if (ext == ".xml") {
        load(scene_parent / file, external_transform, id);
      } else if (ext == ".gltf" || ext == ".glb") {
        gltf::load(scene_parent / file, external_transform, id);
      } else {
        logging::error(
          "Unsupported external scene format: '{}', skipping", ext
        );
      }
    }

    resources::context::manager().load_all();

    logging::info(
      "Scene '{}' loaded successfully with {} entities",
      filepath.string(),
      reg->size()
    );
  }
}
