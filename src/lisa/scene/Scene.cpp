//
// Created by kinami on 4/5/26.
//

#include "Scene.h"

#include "components/TransformComponent.h"
#include "components/context.h"
#include "utils/logging.h"
#include "utils/parser.h"

namespace lisa::scene {

  Scene::Scene(const path& filepath) {
    const auto doc = read_xml_file(filepath);

    const auto scene = doc.document_element();
    if (std::strcmp(scene.name(), "scene") != 0)
      logging::abort(
        "Scene definition must contain exactly one <scene></scene> node in the "
        "top-level"
      );

    auto& reg = components::context::registry();
    for (const auto node : scene.children("entity")) {
      const auto entity = reg.create();
      for (auto child : node.children())
        parse_component(entity, child);
    }

    logging::debug("Scene loaded successfully with {} entities", reg->size());
  }

  pugi::xml_document Scene::read_xml_file(const path& filepath) {
    pugi::xml_document doc;

    if (const auto result = doc.load_file(filepath.c_str())) {
      logging::debug("XML scene parsed correctly");
    } else {
      logging::abort(
        "Error parsing XML scene file\n\tDescription: {}\n\tAt: {}",
        result.description(),
        filepath.c_str() + result.offset
      );
    }

    return doc;
  }

  void Scene::parse_component(entt::entity e, const pugi::xml_node& node) {
    auto type_id = entt::hashed_string(node.name());
    auto meta_type = entt::resolve(type_id);

    if (!meta_type)
      logging::warning(
        "Component of type {} not recognized, ignoring", node.name()
      );

    auto instance = meta_type.construct();

    if (auto custom_parse_func = meta_type.func(entt::hashed_string("parse")))
      custom_parse_func.invoke(instance, entt::forward_as_meta(node));
    else {
      for (const auto& attr : node.attributes()) {
        auto prop_id = entt::hashed_string(attr.name());
        if (auto metadata = meta_type.data(prop_id)) {
          if (auto value =
                utils::parse_value_string(metadata.type(), attr.value()))
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
}
