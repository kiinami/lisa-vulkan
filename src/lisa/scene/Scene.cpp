//
// Created by kinami on 4/5/26.
//

#include "Scene.h"

#include "components/CameraComponent.h"
#include "components/DirectionalLightComponent.h"
#include "components/PointLightComponent.h"
#include "components/TransformComponent.h"
#include "components/context.h"
#include "utils/logging.h"
#include "utils/xml.h"

#include <fastgltf/core.hpp>
#include <fastgltf/tools.hpp>

namespace lisa::scene {

  Scene::Scene(const path& filepath) {
    const auto ext = filepath.extension().string();

    if (ext == ".xml")
      read_xml(filepath);
    else if (ext == ".gltf")
      read_gltf(filepath);
    else
      logging::abort("Scene format '{}' not supported", ext);
  }

  void Scene::read_xml(const path& filepath) {
    const auto doc = utils::xml::read(filepath, "scene");

    const auto scene = doc.document_element();
    const path base_path = filepath.parent_path();

    auto& reg = components::context::registry();
    for (const auto node : scene.children("entity")) {
      const auto entity = reg.create();
      for (auto child : node.children())
        parse_component(entity, child, base_path);
    }

    logging::info("Scene loaded successfully with {} entities", reg->size());
  }

  void Scene::read_gltf(const path& filepath) {
    static fastgltf::Parser parser;

    auto data = fastgltf::GltfDataBuffer::FromPath(filepath);
    if (data.error() != fastgltf::Error::None)
      logging::abort(
        "The scene file at '{}' couldn't be loaded, or the buffer could not be "
        "allocated",
        filepath.c_str()
      );

    auto asset = parser.loadGltf(
      data.get(), filepath.parent_path(), fastgltf::Options::None
    );
    if (const auto error = asset.error(); error != fastgltf::Error::None)
      logging::abort(
        "Some error occurred while reading the buffer, parsing the JSON, or "
        "validating the data"
      );

    if (const auto n_scenes = asset->scenes.size(); n_scenes > 1)
      logging::warning(
        "Only glTF files with one scene are officially supported, and this "
        "file has {} scenes. Only the default scene or first scene will be "
        "rendered.",
        n_scenes
      );
    auto scene_id = asset->defaultScene.value_or(0);

    auto& reg = components::context::registry();

    fastgltf::iterateSceneNodes(
      asset.get(),
      scene_id,
      fastgltf::math::fmat4x4(),
      [&](fastgltf::Node& node, fastgltf::math::fmat4x4 matrix) {
        const auto entity = reg.create();

        auto& transform = reg->emplace<components::TransformComponent>(entity);
        transform.set_matrix(glm::make_mat4(matrix.data()));

        if (node.cameraIndex.has_value()) {
          auto& [camera, name] = asset->cameras[node.cameraIndex.value()];
          if (
            const auto* persp =
              std::get_if<fastgltf::Camera::Perspective>(&camera)
          ) {
            auto& cam = reg->emplace<components::CameraComponent>(entity);
            cam.fov = persp->yfov;
            cam.aspect_ratio = persp->aspectRatio.value_or(16.0f / 9.0f);
            cam.near_plane = persp->znear;
            cam.far_plane = persp->zfar.value_or(100.0f);
          } else
            logging::abort("Orthographic camera not supported.");
        }

        if (node.lightIndex.has_value()) {
          auto& light = asset->lights[node.lightIndex.value()];

          if (light.type == fastgltf::LightType::Point) {
            auto& light_component =
              reg->emplace<components::PointLightComponent>(entity);
            light_component.color = glm::make_vec3(light.color.data());
            light_component.radius = light.range.value_or(100.0f);
            light_component.intensity = light.intensity;
            light_component.attenuation = 1.0;
          } else if (light.type == fastgltf::LightType::Directional) {
            auto& light_component =
              reg->emplace<components::DirectionalLightComponent>(entity);
            light_component.color = glm::make_vec3(light.color.data());
            light_component.intensity = light.intensity;
          } else if (light.type == fastgltf::LightType::Spot) {
            logging::warning("Lights of type Spot are not supported, ignoring");
          }
        }
      }
    );

    logging::info("Scene loaded successfully with {} entities", reg->size());
  }

  void Scene::parse_component(
    entt::entity e, const pugi::xml_node& node, const path& base_path
  ) {
    auto type_id = entt::hashed_string(node.name());
    auto meta_type = entt::resolve(type_id);

    if (!meta_type)
      logging::warning(
        "Component of type {} not recognized, ignoring", node.name()
      );

    auto instance = meta_type.construct();

    if (auto custom_parse_func = meta_type.func(entt::hashed_string("parse")))
      custom_parse_func.invoke(
        instance, entt::forward_as_meta(node), entt::forward_as_meta(base_path)
      );
    else {
      for (const auto& attr : node.attributes()) {
        auto prop_id = entt::hashed_string(attr.name());
        if (auto metadata = meta_type.data(prop_id)) {
          if (
            auto value =
              parse_value_string(metadata.type(), attr.value(), base_path)
          )
            metadata.set(instance, value);
        }
      }
    }

    if (auto emplace_func = meta_type.func(entt::hashed_string("emplace")))
      emplace_func.invoke(
        {},
        entt::forward_as_meta(*components::context::registry().operator->()),
        e,
        instance.as_ref()
      );
  }

  entt::meta_any Scene::parse_value_string(
    const entt::meta_type& type, const str& value, const path& base_path
  ) {
    if (type == entt::resolve<float>()) return utils::xml::parse<float>(value);
    if (type == entt::resolve<int>()) return utils::xml::parse<int>(value);
    if (type == entt::resolve<bool>()) return utils::xml::parse<bool>(value);
    if (type == entt::resolve<str>()) return utils::xml::parse<str>(value);
    if (type == entt::resolve<path>()) {
      auto p = utils::xml::parse<path>(value);
      if (p.is_relative()) p = (base_path / p).lexically_normal();
      return p;
    }
    if (type == entt::resolve<vec3>()) return utils::xml::parse<vec3>(value);
    if (type == entt::resolve<rgb>()) return utils::xml::parse<rgb>(value);
    if (type == entt::resolve<vec4>()) return utils::xml::parse<vec4>(value);
    if (type == entt::resolve<rgba>()) return utils::xml::parse<rgba>(value);
    if (type == entt::resolve<mat3>()) return utils::xml::parse<mat3>(value);
    if (type == entt::resolve<mat4>()) return utils::xml::parse<mat4>(value);
    return {};
  }
}
