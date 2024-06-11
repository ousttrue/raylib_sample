#pragma once
#include <raylib.h>
#include <raymath.h>
#include <rcamera.h>

inline auto dolly(Camera3D *camera) -> void {
  auto wheel = GetMouseWheelMoveV();
  if (wheel.y > 0) {
    auto distance = Vector3Distance(camera->target, camera->position);
    CameraMoveToTarget(camera, distance * 0.9 - distance);
  } else if (wheel.y < 0) {
    auto distance = Vector3Distance(camera->target, camera->position);
    CameraMoveToTarget(camera, distance * 1.1 - distance);
  }
}

struct OrbitCamera {
  int yawDegree = 0;
  int pitchDegree = 40;
  float shiftX = 0;
  float shiftY = 0;

  bool MouseUpdateCamera(float distance, float fovy, const Rectangle &rect) {
    auto delta = GetMouseDelta();
    auto active = false;

    // camera shift
    if (IsMouseButtonDown(MOUSE_BUTTON_MIDDLE)) {
      auto speed = distance * tan(fovy * 0.5) * 2.0 / rect.height;
      shiftX += delta.x * speed;
      shiftY += delta.y * speed;
      active = true;
    }

    // yaw pitch
    if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT)) {
      yawDegree -= static_cast<int>(delta.x);
      pitchDegree += static_cast<int>(delta.y);
      if (pitchDegree > 89) {
        pitchDegree = 89;
      } else if (pitchDegree < -89) {
        pitchDegree = -89;
      }
      active = true;
    }

    return active;
  }

  void update_view(Camera3D *camera) {
    auto distance = Vector3Distance(camera->target, camera->position);
    auto pitch = MatrixRotateX(static_cast<float>(pitchDegree) * DEG2RAD);
    auto yaw = MatrixRotateY(static_cast<float>(yawDegree) * DEG2RAD);
    auto translation = MatrixTranslate(shiftX, shiftY, -distance);
    auto camera_transform =
        MatrixMultiply(translation, MatrixMultiply(pitch, yaw));

    camera->position = {
        .x = camera_transform.m12,
        .y = camera_transform.m13,
        .z = camera_transform.m14,
    };
    auto forward = Vector3{
        .x = camera_transform.m8,
        .y = camera_transform.m9,
        .z = camera_transform.m10,
    };
    camera->target =
        Vector3Add(camera->position, Vector3Scale(forward, distance));
  }
};
