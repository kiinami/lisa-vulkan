//
// Created by kinami on 4/5/26.
//

#ifndef LISA_VULKAN_SCENE_H
#define LISA_VULKAN_SCENE_H
#include "entt/entity/entity.hpp"
#include "entt/meta/meta.hpp"
#include "utils/common.h"

#include <pugixml.hpp>

namespace lisa::scene {
  class Scene {
  public:
    explicit Scene(const path& filepath);
    ~Scene() = default;

  private:
    static pugi::xml_document read_xml_file(const path& filepath);
    static void parse_component(entt::entity e, const pugi::xml_node& node);
  };

}

#endif // LISA_VULKAN_SCENE_H
