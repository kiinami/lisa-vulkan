//
// Created by kinami on 4/25/26.
//

#include "CameraMovement.h"

#include "components/CameraComponent.h"
#include "components/TransformComponent.h"
#include "components/context.h"
#include "context.h"
#include "window/Window.h"

namespace lisa::update {
  namespace {
    float yaw_ = 0.0f;
    float pitch_ = 0.0f;

    components::TransformComponent* find_camera_transform() {
      auto& registry = components::context::registry();
      const auto view =
        registry
          ->view<components::CameraComponent, components::TransformComponent>();
      if (view.begin() == view.end()) return nullptr;
      return &view.get<components::TransformComponent>(*view.begin());
    }
  }

  void update_camera_position(const float dt) {
    const auto transform = find_camera_transform();
    if (!transform) return;

    const auto keys = window::Window::keyboard_state();

    if (keys[SDL::SCANCODE_W])
      transform->translate_local({0.0f, 0.0f, -SPEED * dt});
    if (keys[SDL::SCANCODE_S])
      transform->translate_local({0.0f, 0.0f, SPEED * dt});
    if (keys[SDL::SCANCODE_D])
      transform->translate_local({SPEED * dt, 0.0f, 0.0f});
    if (keys[SDL::SCANCODE_A])
      transform->translate_local({-SPEED * dt, 0.0f, 0.0f});
    if (keys[SDL::SCANCODE_E]) transform->translate({0.0f, SPEED * dt, 0.0f});
    if (keys[SDL::SCANCODE_Q]) transform->translate({0.0f, -SPEED * dt, 0.0f});
  }

  REGISTER_UPDATE(update_camera_position);

  void update_camera_rotation(const float dt) {
    const auto transform = find_camera_transform();
    if (!transform) return;

    const auto state = window::Window::mouse_state();

    yaw_ -= state.x * SENSITIVITY;
    pitch_ -= state.y * SENSITIVITY;
    pitch_ = glm::clamp(pitch_, glm::radians(-89.0f), glm::radians(89.0f));

    const quat orientation = glm::angleAxis(yaw_, vec3(0.0f, 1.0f, 0.0f)) *
                             glm::angleAxis(pitch_, vec3(1.0f, 0.0f, 0.0f));
    transform->set_rotation(orientation);
  }

  REGISTER_UPDATE(update_camera_rotation);
}
