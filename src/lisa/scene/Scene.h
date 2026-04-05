//
// Created by kinami on 4/5/26.
//

#ifndef LISA_VULKAN_SCENE_H
#define LISA_VULKAN_SCENE_H
#include "resources/Mesh.h"
#include "resources/Texture.h"
#include "resources/context.h"
#include "systems/resources/ResourceManager.h"
#include "utils/common.h"

namespace lisa::scene {

  struct Entity {
    str mesh_id;
    str texture_id;
    mat4 model;
  };

  struct Camera {
    uint32 width;
    uint32 height;
    mat4 view;
    mat4 projection;
  };

  class Scene {
  public:
    Scene(const vector<Entity>& entities, const Camera& camera) :
      entities_(entities),
      camera_(camera) {
      for (const auto& e : entities_) {
        meshes_.push_back(
          resources::context::manager().load<resources::Mesh>(e.mesh_id)
        );
        textures_.push_back(
          resources::context::manager().load<resources::Texture>(e.texture_id)
        );
      }
    }

    ~Scene() = default;

    const vector<Entity>& entities() const { return entities_; }

    const Camera& camera() const { return camera_; }

  private:
    vector<Entity> entities_;
    Camera camera_;
    vector<systems::resources::ResourceHandle<resources::Mesh>> meshes_;
    vector<systems::resources::ResourceHandle<resources::Texture>> textures_;
  };

}

#endif // LISA_VULKAN_SCENE_H
