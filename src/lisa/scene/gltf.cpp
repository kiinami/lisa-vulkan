//
// Created by kinami on 5/2/26.
//

#include "gltf.h"

#include "components/CameraComponent.h"
#include "components/DirectionalLightComponent.h"
#include "components/MaterialComponent.h"
#include "components/MeshComponent.h"
#include "components/PointLightComponent.h"
#include "components/TransformComponent.h"
#include "components/context.h"
#include "resources/context.h"
#include "utils/logging.h"

#include <fastgltf/core.hpp>
#include <fastgltf/tools.hpp>

namespace lisa::scene::gltf {

  namespace {
    fastgltf::Parser parser_{fastgltf::Extensions::KHR_lights_punctual};
    fastgltf::Asset asset_;
    path base_path_;

    void load_asset(const path& filepath) {
      auto data = fastgltf::GltfDataBuffer::FromPath(filepath);
      if (data.error() != fastgltf::Error::None)
        logging::abort(
          "The scene file at '{}' couldn't be loaded, or the buffer could not "
          "be allocated",
          filepath.c_str()
        );

      auto asset = parser_.loadGltf(
        data.get(),
        filepath.parent_path(),
        fastgltf::Options::LoadExternalBuffers
      );
      if (const auto error = asset.error(); error != fastgltf::Error::None)
        logging::abort(
          "Some error occurred while reading the buffer, parsing the JSON, or "
          "validating the data"
        );

      asset_ = *std::move(asset);
    }

    size select_scene(const fastgltf::Asset& asset) {
      if (const auto n_scenes = asset.scenes.size(); n_scenes > 1)
        logging::warning(
          "Only glTF files with one scene are officially supported, and this "
          "file has {} scenes. Only the default scene or first scene will be "
          "rendered.",
          n_scenes
        );
      return asset.defaultScene.value_or(0);
    }

    resources::Texture* load_texture(
      const fastgltf::Image& image,
      const str& id,
      const vector<fastgltf::Buffer>& buffers,
      const vector<fastgltf::BufferView>& buffer_views,
      int channel = -1
    ) {
      resources::Texture* texture =
        resources::context::manager().get<resources::Texture>(id);

      if (texture) return texture;

      std::visit(
        fastgltf::visitor{
          [&](const fastgltf::sources::URI& filepath) {
            texture = resources::context::manager().load<resources::Texture>(
              static_cast<str>(filepath.uri.string()),
              base_path_ / filepath.uri.string()
            );
          },
          [&](const fastgltf::sources::Vector& file) {
            texture = resources::context::manager().load<resources::Texture>(
              id, file.bytes.data(), file.bytes.size()
            );
          },
          [&](const fastgltf::sources::ByteView& view) {
            texture = resources::context::manager().load<resources::Texture>(
              id, view.bytes.data(), view.bytes.size()
            );
          },

          [&](const fastgltf::sources::BufferView& bv) {
            const auto& buffer_view = buffer_views[bv.bufferViewIndex];
            auto& buffer = buffers[buffer_view.bufferIndex];

            std::visit(
              fastgltf::visitor{
                [](auto& arg) {},
                [&](const fastgltf::sources::Vector& vec) {
                  texture =
                    resources::context::manager().load<resources::Texture>(
                      id, vec.bytes.data(), vec.bytes.size(), channel
                    );
                }
              },
              buffer.data
            );
          },
          [&](auto&) {
            logging::warning("Unsupported glTF texture source");
          }
        },
        image.data
      );
      return texture;
    }

    void node_iterator(fastgltf::Node& node, fastgltf::math::fmat4x4 matrix) {
      auto& reg = components::context::registry();
      const auto entity = reg.create();

      auto& transform = reg->emplace<components::TransformComponent>(entity);
      transform.set_matrix(glm::make_mat4(matrix.data()));

      if (node.cameraIndex.has_value()) {
        auto& [camera, name] = asset_.cameras[node.cameraIndex.value()];
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
        auto& light = asset_.lights[node.lightIndex.value()];

        if (light.type == fastgltf::LightType::Point) {
          auto& light_component =
            reg->emplace<components::PointLightComponent>(entity);
          light_component.color = glm::make_vec3(light.color.data());
          light_component.radius = light.range.value_or(10000.0f);
          light_component.intensity = light.intensity;
          light_component.attenuation = 1.0;
        } else if (light.type == fastgltf::LightType::Directional) {
          auto& light_component =
            reg->emplace<components::DirectionalLightComponent>(entity);
          light_component.color = glm::make_vec3(light.color.data());
          light_component.intensity =
            light.intensity > 1e4f ? light.intensity : 1.0f;
        } else if (light.type == fastgltf::LightType::Spot) {
          logging::warning("Lights of type Spot are not supported, ignoring");
        }
      }

      if (node.meshIndex.has_value()) {
        const auto& mesh = asset_.meshes[node.meshIndex.value()];
        const auto mesh_id = "mesh::" + std::to_string(node.meshIndex.value());

        for (auto i = 0; i < mesh.primitives.size(); i++) {
          const auto& p = mesh.primitives[i];
          const auto p_id = mesh_id + "::p::" + std::to_string(i);

          const auto p_entity = reg.create();
          auto& p_transform =
            reg->emplace<components::TransformComponent>(p_entity);
          p_transform.set_matrix(glm::make_mat4(matrix.data()));

          const auto* mesh_res =
            resources::context::manager().load<resources::Mesh>(
              p_id, p, asset_
            );
          auto& mesh_component =
            reg->emplace<components::MeshComponent>(p_entity);
          mesh_component.mesh = mesh_res;

          if (p.materialIndex.has_value()) {
            auto& material = asset_.materials[p.materialIndex.value()];

            auto& material_component =
              reg->emplace<components::MaterialComponent>(p_entity);

            material_component.albedo =
              glm::make_vec3(material.pbrData.baseColorFactor.data());
            material_component.roughness = material.pbrData.roughnessFactor;
            material_component.metallic = material.pbrData.metallicFactor;

            if (material.pbrData.baseColorTexture.has_value()) {
              auto& tex =
                asset_.textures[material.pbrData.baseColorTexture.value()
                                  .textureIndex];
              if (tex.imageIndex.has_value()) {
                auto& img = asset_.images[tex.imageIndex.value()];
                auto img_id = "img::" + std::to_string(tex.imageIndex.value());
                material_component.albedo_texture =
                  load_texture(img, img_id, asset_.buffers, asset_.bufferViews);
              }
            }
            if (material.pbrData.metallicRoughnessTexture.has_value()) {
              auto& tex =
                asset_
                  .textures[material.pbrData.metallicRoughnessTexture.value()
                              .textureIndex];
              if (tex.imageIndex.has_value()) {
                auto& img = asset_.images[tex.imageIndex.value()];
                const auto base_id = "img::" + std::to_string(tex.imageIndex.value());

                material_component.roughness_texture = load_texture(
                    img, base_id + "::roughness", asset_.buffers, asset_.bufferViews, 1
                );
                material_component.metallic_texture = load_texture(
                    img, base_id + "::metallic", asset_.buffers, asset_.bufferViews, 2
                );
              }
            }
            if (material.normalTexture.has_value()) {
              auto& tex =
                asset_.textures[material.normalTexture.value().textureIndex];
              if (tex.imageIndex.has_value()) {
                auto& img = asset_.images[tex.imageIndex.value()];
                auto img_id = "img::" + std::to_string(tex.imageIndex.value());
                material_component.normal_texture =
                  load_texture(img, img_id, asset_.buffers, asset_.bufferViews);
              }
            }
          }
        }
      }
    }
  }

  void load(const path& filepath) {
    base_path_ = filepath.parent_path();

    load_asset(filepath);

    const auto scene_id = select_scene(asset_);

    fastgltf::iterateSceneNodes(
      asset_, scene_id, fastgltf::math::fmat4x4(), node_iterator
    );

    auto& reg = components::context::registry();
    logging::info("Scene loaded successfully with {} entities", reg->size());
  }
}
