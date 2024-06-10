// This is free and unencumbered software released into the public domain.
// For more information, please refer to <http://unlicense.org>
#include <raylib/external/glad.h>

#include "gizmo.h"

// #include "camera.h"
#include "drawable.h"
#include "teapot.h"

#include <raymath.h>
#include <rcamera.h>
#include <rlgl.h>

#include <chrono>
#include <span>

struct TeapotVertex {
  Vector3 position;
  Vector3 color;
};

auto dolly(Camera3D *camera) -> void {
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

int main(int argc, char *argv[]) {

  Camera3D camera{
      .position = {0, 1.5f, 10},
      .target = {0, 0, 0},
      .up = {0, 1, 0},
      .fovy = 45.0f,
      .projection = CAMERA_PERSPECTIVE,
  };

  InitWindow(1280, 800, "tiny-gizmo-example-app");

  auto teapot = Drawable{};
  {
    teapot.load<TeapotVertex, uint32_t>(
        {(TeapotVertex *)teapot_vertices.data(), teapot_vertices.size() / 6},
        teapot_triangles, false);
  }

  tinygizmo::rigid_transform xform_a;
  xform_a.position = {-2, 0, 0};
  tinygizmo::rigid_transform xform_b;
  xform_b.position = {2, 0, 0};

  Gizmo gizmo;
  OrbitCamera orbit;

  auto t0 = std::chrono::high_resolution_clock::now();
  while (!WindowShouldClose()) {
    // auto w = GetScreenWidth();
    // auto h = GetScreenHeight();
    // auto aspect = static_cast<float>(w) / static_cast<float>(h);
    // auto t1 = std::chrono::high_resolution_clock::now();
    // float timestep = std::chrono::duration<float>(t1 - t0).count();
    // t0 = t1;

    // mouse
    // auto mr = IsMouseButtonDown(MOUSE_BUTTON_RIGHT);
    // auto deltaCursorMotion = GetMouseDelta();
    // if (mr) {
    //   yaw -= deltaCursorMotion.x * 0.01f;
    //   pitch -= deltaCursorMotion.y * 0.01f;
    // }
    // if (mr) {
    //   cam.shift(timestep);
    // }
    dolly(&camera);
    auto distance = Vector3Distance(camera.target, camera.position);
    orbit.MouseUpdateCamera(distance, camera.fovy,
                            {
                                0,
                                0,
                                static_cast<float>(GetScreenWidth()),
                                static_cast<float>(GetScreenHeight()),
                            });
    // if (self.is_active) {
    orbit.update_view(&camera);
    // }

    // gizmo
    gizmo.new_frame();
    xform_a = gizmo.transform("first-example-gizmo", xform_a);
    auto ma = xform_a.matrix();
    xform_b = gizmo.transform("second-example-gizmo", xform_b);
    auto mb = xform_b.matrix();

    // render
    BeginDrawing();
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_CULL_FACE);
    ClearBackground(RAYWHITE);

    BeginMode3D(camera);
    {
      // teapot
      teapot.draw(MatrixTranspose(*(Matrix *)&ma));
      teapot.draw(MatrixTranspose(*(Matrix *)&mb));

      DrawGrid(10, 1.0);

      gizmo.draw();
    }
    EndMode3D();
    EndDrawing();
  }

  CloseWindow();
  return EXIT_SUCCESS;
}
