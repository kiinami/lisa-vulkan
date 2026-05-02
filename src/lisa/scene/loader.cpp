//
// Created by kinami on 5/2/26.
//

#include "loader.h"

#include "components/CameraComponent.h"
#include "components/DirectionalLightComponent.h"
#include "components/PointLightComponent.h"
#include "components/TransformComponent.h"
#include "components/context.h"
#include "fastgltf/core.hpp"
#include "fastgltf/tools.hpp"
#include "utils/xml.h"

namespace lisa::scene {
  namespace {

  }

  void load_xml(const path& filepath) {
    const auto doc = utils::xml::read(filepath, "scene");

    const auto scene = doc.document_element();
    const path base_path = filepath.parent_path();

    auto& reg = components::context::registry();
    for (const auto node : scene.children("entity")) {
      const auto entity = reg.create();
      for (auto child : node.children()) {
        
      }
    }

    logging::info("Scene loaded successfully with {} entities", reg->size());
  }

  void load_gltf(const path& filepath) {
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
}
