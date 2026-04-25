//
// Created by kinami on 4/25/26.
//

#ifndef LISA_VULKAN_CAMERAMOVEMENT_H
#define LISA_VULKAN_CAMERAMOVEMENT_H

namespace lisa::update {
  constexpr float SPEED = 5.0f;
  constexpr float SENSITIVITY = 0.001f;

  void update_camera_position(float dt);
  void update_camera_rotation(float dt);
}

#endif //LISA_VULKAN_CAMERAMOVEMENT_H
