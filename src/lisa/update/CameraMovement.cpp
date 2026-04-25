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
  void update_camera(const float dt) {
    auto& registry = components::context::registry();
    const auto view =
      registry
        ->view<components::CameraComponent, components::TransformComponent>();
    if (view.begin() == view.end()) return;

    auto& transform = view.get<components::TransformComponent>(*view.begin());

    const auto keys = window::Window::keyboard_state();

    if (keys[SDL::SCANCODE_W])
      transform.translate_local({0.0f, 0.0f, -SPEED * dt});
    if (keys[SDL::SCANCODE_S])
      transform.translate_local({0.0f, 0.0f, SPEED * dt});
    if (keys[SDL::SCANCODE_D])
      transform.translate_local({SPEED * dt, 0.0f, 0.0f});
    if (keys[SDL::SCANCODE_A])
      transform.translate_local({-SPEED * dt, 0.0f, 0.0f});
    if (keys[SDL::SCANCODE_E]) transform.translate({0.0f, SPEED * dt, 0.0f});
    if (keys[SDL::SCANCODE_Q]) transform.translate({0.0f, -SPEED * dt, 0.0f});
  }

  REGISTER_UPDATE(update_camera);
}
