//
// Created by kinami on 4/5/26.
//

#include "Scene.h"

#include "components/TransformComponent.h"
#include "components/context.h"
#include "utils/logging.h"
#include "utils/xml.h"

namespace lisa::scene {

  Scene::Scene(const path& filepath) {
    const auto doc = utils::xml::read(filepath, "scene");

    const auto scene = doc.document_element();

    auto& reg = components::context::registry();
    for (const auto node : scene.children("entity")) {
      const auto entity = reg.create();
      for (auto child : node.children())
        parse_component(entity, child);
    }

    logging::debug("Scene loaded successfully with {} entities", reg->size());
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
          if (auto value = parse_value_string(metadata.type(), attr.value()))
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

  entt::meta_any
    Scene::parse_value_string(const entt::meta_type& type, const str& value) {
    if (type == entt::resolve<float>()) return utils::xml::parse<float>(value);
    if (type == entt::resolve<int>()) return utils::xml::parse<int>(value);
    if (type == entt::resolve<bool>()) return utils::xml::parse<bool>(value);
    if (type == entt::resolve<str>()) return utils::xml::parse<str>(value);
    if (type == entt::resolve<path>()) return utils::xml::parse<path>(value);
    if (type == entt::resolve<vec3>()) return utils::xml::parse<vec3>(value);
    if (type == entt::resolve<rgb>()) return utils::xml::parse<rgb>(value);
    if (type == entt::resolve<vec4>()) return utils::xml::parse<vec4>(value);
    if (type == entt::resolve<rgba>()) return utils::xml::parse<rgba>(value);
    if (type == entt::resolve<mat3>()) return utils::xml::parse<mat3>(value);
    if (type == entt::resolve<mat4>()) return utils::xml::parse<mat4>(value);
    return {};
  }
}
