// This is free and unencumbered software released into the public domain.
// For more information, please refer to <http://unlicense.org>
#include <raylib/external/glad.h>

#include "gizmo.h"

#include "camera.h"
#include "drawable.h"
#include "teapot.h"

#include <raymath.h>
#include <rlgl.h>

#include <chrono>
#include <span>

struct TeapotVertex {
  Vector3 position;
  Vector3 color;
};

int main(int argc, char *argv[]) {
  camera cam = {};
  cam.yfov = 1.0f;
  cam.near_clip = 0.01f;
  cam.far_clip = 32.0f;
  cam.position = {0, 1.5f, 4};

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

  auto t0 = std::chrono::high_resolution_clock::now();
  while (!WindowShouldClose()) {
    auto w = GetScreenWidth();
    auto h = GetScreenHeight();
    auto aspect = static_cast<float>(w) / static_cast<float>(h);
    auto t1 = std::chrono::high_resolution_clock::now();
    float timestep = std::chrono::duration<float>(t1 - t0).count();
    t0 = t1;

    // mouse
    auto mr = IsMouseButtonDown(MOUSE_BUTTON_RIGHT);
    auto deltaCursorMotion = GetMouseDelta();
    if (mr) {
      cam.yaw -= deltaCursorMotion.x * 0.01f;
      cam.pitch -= deltaCursorMotion.y * 0.01f;
    }
    if (mr) {
      cam.shift(timestep);
    }

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

    {
      auto proj = cam.get_projection_matrix(aspect);
      rlMatrixMode(RL_PROJECTION); // Switch to projection matrix
      rlLoadIdentity();            // Reset current matrix (projection)
      rlMultMatrixf(&proj.x.x);

      rlMatrixMode(RL_MODELVIEW); // Switch to projection matrix
      rlLoadIdentity();           // Reset current matrix (projection)
      auto view = cam.get_view_matrix();
      rlMultMatrixf(&view.x.x);

      // teapot
      teapot.draw(MatrixTranspose(*(Matrix *)&ma));
      teapot.draw(MatrixTranspose(*(Matrix *)&mb));

      DrawGrid(10, 1.0);

      gizmo.draw();

      EndDrawing();
    }
  }

  CloseWindow();
  return EXIT_SUCCESS;
}
