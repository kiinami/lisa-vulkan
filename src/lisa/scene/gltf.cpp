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
#include "utils/path.h"

#include <fastgltf/core.hpp>
#include <fastgltf/tools.hpp>
#include <fstream>

namespace lisa::scene::gltf {

  namespace {
    fastgltf::Parser parser_{fastgltf::Extensions::KHR_lights_punctual};
    fastgltf::Asset asset_;
    path base_path_;
    str id_;

    void load_asset(const path& filepath, const str& parent_id = "") {
      auto data = fastgltf::GltfDataBuffer::FromPath(filepath);
      if (data.error() != fastgltf::Error::None)
        logging::abort(
          "The scene file at '{}' couldn't be loaded, or the buffer could not "
          "be allocated",
          utils::pstr(filepath)
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
      id_ = parent_id == "" ? "scene" : logging::genid(parent_id, filepath);
    }

    size select_scene(const fastgltf::Asset& asset) {
      const auto scene = asset.defaultScene.value_or(0);
      if (const auto n_scenes = asset.scenes.size(); n_scenes > 1)
        logging::warning(
          "Only glTF files with one scene are officially supported, and this "
          "file has {} scenes. Only scene {} will be rendered.",
          n_scenes,
          scene
        );
      return scene;
    }

    optional<str> load_texture(
      const fastgltf::Image& image,
      const str& id,
      const vector<fastgltf::Buffer>& buffers,
      const vector<fastgltf::BufferView>& buffer_views,
      int channel = -1
    ) {
      if (resources::context::manager().is_added<resources::Texture>(id))
        return id;

      optional<str> result = nullopt;

      std::visit(
        fastgltf::visitor{
          [&](const fastgltf::sources::URI& filepath) {
            result =
              optional{resources::context::manager()
                         .add<resources::Texture, resources::TextureSpec>(
                           id, base_path_ / filepath.uri.string(), channel
                         )};
          },
          [&](const fastgltf::sources::Vector& v) {
            result = resources::context::manager()
                       .add<resources::Texture, resources::TextureSpec>(
                         id, v.mimeType, v.bytes, channel
                       );
          },
          [&](const fastgltf::sources::Array& a) {
            if (a.mimeType != fastgltf::MimeType::None) {
              const auto data = std::vector(a.bytes.begin(), a.bytes.end());
              result = resources::context::manager()
                         .add<resources::Texture, resources::TextureSpec>(
                           id, a.mimeType, data, channel
                         );
            }
          },
          [&](const fastgltf::sources::ByteView& v) {
            const auto data =
              std::vector(v.bytes.data(), v.bytes.data() + v.bytes.size());
            result = resources::context::manager()
                       .add<resources::Texture, resources::TextureSpec>(
                         id, v.mimeType, data, channel
                       );
          },
          [&](const fastgltf::sources::BufferView& bv) {
            const auto& buffer_view = buffer_views[bv.bufferViewIndex];
            const auto& buffer = buffers[buffer_view.bufferIndex];
            std::visit(
              fastgltf::visitor{
                [&](const fastgltf::sources::Vector& v) {
                  const auto* start = v.bytes.data() + buffer_view.byteOffset;
                  const auto data =
                    std::vector(start, start + buffer_view.byteLength);
                  result = resources::context::manager()
                             .add<resources::Texture, resources::TextureSpec>(
                               id, bv.mimeType, data, channel
                             );
                },
                [&](const fastgltf::sources::Array& a) {
                  if (a.mimeType != fastgltf::MimeType::None) {
                    const auto* start = a.bytes.data() + buffer_view.byteOffset;
                    const auto data =
                      std::vector(start, start + buffer_view.byteLength);
                    result = resources::context::manager()
                               .add<resources::Texture, resources::TextureSpec>(
                                 id, bv.mimeType, data, channel
                               );
                  }
                },
                [](auto&) {
                  logging::warning(
                    "Unsupported glTF buffer view texture source"
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

      return result;
    }
  }

  void node_iterator(fastgltf::Node& node, fastgltf::math::fmat4x4 matrix) {
    auto& reg = components::context::registry();
    const auto entity = reg.create();
    const auto entity_id = logging::genid(id_, "nodes", node.name);

    auto& transform = reg->emplace<components::TransformComponent>(entity);
    transform.set_matrix(glm::make_mat4(matrix.data()));

    if (node.cameraIndex.has_value()) {
      auto& [camera, name] = asset_.cameras[node.cameraIndex.value()];
      if (
        const auto* persp = std::get_if<fastgltf::Camera::Perspective>(&camera)
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
      const auto mesh_id =
        logging::genid(id_, "meshes", node.meshIndex.value());

      for (size i = 0; i < mesh.primitives.size(); i++) {
        const auto& p = mesh.primitives[i];
        const auto p_id = logging::genid(mesh_id, "primitives", i);

        const auto p_entity = reg.create();
        auto& p_transform =
          reg->emplace<components::TransformComponent>(p_entity);
        p_transform.set_matrix(glm::make_mat4(matrix.data()));

        const auto mesh_token =
          resources::context::manager()
            .add<resources::Mesh, resources::MeshSpec>(p_id, p, asset_);
        reg->emplace<components::MeshComponent>(p_entity, mesh_token);

        if (p.materialIndex.has_value()) {
          auto& material = asset_.materials[p.materialIndex.value()];
          auto material_id =
            logging::genid(id_, "materials", p.materialIndex.value());

          optional<str> albedo_texture = nullopt;
          optional<str> normal_texture = nullopt;
          optional<str> roughness_texture = nullopt;
          optional<str> metallic_texture = nullopt;

          if (material.pbrData.baseColorTexture.has_value()) {
            auto& tex =
              asset_.textures[material.pbrData.baseColorTexture.value()
                                .textureIndex];
            if (tex.imageIndex.has_value()) {
              auto& img = asset_.images[tex.imageIndex.value()];
              auto img_id =
                logging::genid(id_, "images", tex.imageIndex.value());
              albedo_texture =
                load_texture(img, img_id, asset_.buffers, asset_.bufferViews);
            }
          }

          if (material.pbrData.metallicRoughnessTexture.has_value()) {
            auto& tex =
              asset_.textures[material.pbrData.metallicRoughnessTexture.value()
                                .textureIndex];
            if (tex.imageIndex.has_value()) {
              auto& img = asset_.images[tex.imageIndex.value()];
              auto base_id =
                logging::genid(id_, "images", tex.imageIndex.value());

              roughness_texture = load_texture(
                img,
                logging::genid(base_id, "roughness"),
                asset_.buffers,
                asset_.bufferViews,
                1
              );
              metallic_texture = load_texture(
                img,
                logging::genid(base_id, "metallic"),
                asset_.buffers,
                asset_.bufferViews,
                2
              );
            }
          }

          if (material.normalTexture.has_value()) {
            auto& tex =
              asset_.textures[material.normalTexture.value().textureIndex];
            if (tex.imageIndex.has_value()) {
              auto& img = asset_.images[tex.imageIndex.value()];
              auto img_id =
                logging::genid(id_, "images", tex.imageIndex.value());
              normal_texture =
                load_texture(img, img_id, asset_.buffers, asset_.bufferViews);
            }
          }

          reg->emplace<components::MaterialComponent>(
            p_entity,
            glm::make_vec3(material.pbrData.baseColorFactor.data()),
            material.pbrData.roughnessFactor,
            material.pbrData.metallicFactor,
            albedo_texture,
            normal_texture,
            roughness_texture,
            metallic_texture
          );
        }
      }
    }
  }

  void load(const path& filepath, mat4 transform, const str& parent_id) {
    base_path_ = filepath.parent_path();

    load_asset(filepath, parent_id);

    const auto scene_id = select_scene(asset_);

    fastgltf::math::fmat4x4 fastgltf_transform;
    std::memcpy(
      fastgltf_transform.data(), &transform[0][0], sizeof(float) * 16
    );

    fastgltf::iterateSceneNodes(
      asset_, scene_id, fastgltf_transform, node_iterator
    );

    resources::context::manager().load_all();

    logging::info(
      "Scene loaded successfully"
    );
  }
}
