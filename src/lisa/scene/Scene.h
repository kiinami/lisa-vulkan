//
// Created by kinami on 4/5/26.
//

#ifndef LISA_VULKAN_SCENE_H
#define LISA_VULKAN_SCENE_H
#include "utils/common.h"

#include <entt/entity/entity.hpp>
#include <entt/meta/meta.hpp>
#include <pugixml.hpp>

namespace lisa::scene {
  class Scene {
  public:
    explicit Scene(const path& filepath);
    ~Scene() = default;

  private:
    static void parse_component(
      entt::entity e, const pugi::xml_node& node, const path& base_path
    );
    static entt::meta_any parse_value_string(
      const entt::meta_type& type, const str& value, const path& base_path
    );
  };

}

#endif // LISA_VULKAN_SCENE_H
