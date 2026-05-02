//
// Created by kinami on 4/5/26.
//

#include "TransformComponent.h"

#include "systems/ecs/meta.h"
#include "utils/logging.h"
#include "utils/xml.h"

#include <entt/entity/registry.hpp>
#include <entt/meta/factory.hpp>
#include <glm/gtx/matrix_decompose.hpp>

namespace lisa::components {
  namespace {
    const bool registered = [] {
      using namespace entt::literals;

      systems::ecs::reflect_component<TransformComponent>("transform"_hs)
        .func<&TransformComponent::parse>("parse"_hs);

      return true;
    }();
  }

  void TransformComponent::parse(
    const pugi::xml_node& node, const path& base_path
  ) {
    mat4 mat{1.0f};

    for (const auto& op : node.children())
      if (std::strcmp(op.name(), "translate") == 0)
        mat = glm::translate(
          mat, utils::xml::parse<vec3>(op.attribute("value").value())
        );
      else if (std::strcmp(op.name(), "rotate") == 0) {
        mat *= glm::mat4_cast(
          glm::angleAxis(
            glm::radians(
              utils::xml::parse<float>(op.attribute("angle").value())
            ),
            glm::normalize(
              utils::xml::parse<vec3>(op.attribute("axis").value())
            )
          )
        );
      } else if (std::strcmp(op.name(), "scale") == 0) {
        mat = glm::scale(
          mat, utils::xml::parse<vec3>(op.attribute("value").value())
        );
      } else if (std::strcmp(op.name(), "lookat") == 0) {
        auto origin = utils::xml::parse<vec3>(op.attribute("origin").value());
        auto target = utils::xml::parse<vec3>(op.attribute("target").value());
        auto up = utils::xml::parse<vec3>(op.attribute("up").value());

        mat *= glm::inverse(glm::lookAt(origin, target, up));
      } else if (std::strcmp(op.name(), "matrix") == 0)
        mat *= utils::xml::parse<mat4>(op.attribute("value").value());
      else
        logging::warning(
          "'{}' node of <transform> component not supported, skipping it",
          op.name()
        );

    vec3 skew;
    vec4 perspective;
    glm::decompose(mat, scale_, rotation_, position_, skew, perspective);
  }

  mat4 TransformComponent::matrix() const {
    if (dirty_) {
      matrix_ = glm::translate(mat4(1.0f), position_) *
                glm::mat4_cast(rotation_) *
                glm::scale(mat4(1.0f), scale_);
      dirty_ = false;
    }
    return matrix_;
  }
}
