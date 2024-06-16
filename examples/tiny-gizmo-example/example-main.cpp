// This is free and unencumbered software released into the public domain.
// For more information, please refer to <http://unlicense.org>
#include <assert.h>
#include <raylib/external/glad.h>

#include "drawable.h"
#include "gizmo_dragger.h"
#include "orbit_camera.h"
#include "rdrag.h"
#include "teapot.h"
#include <rlgl.h>

int main(int argc, char *argv[]) {

  InitWindow(1280, 800, "tiny-gizmo-example-app");

  auto a = std::make_shared<Drawable>();
  a->name = "first-example-gizmo";
  a->position = {-2, 0, 0};
  a->load({(Vertex *)teapot_vertices, _countof(teapot_vertices) / 6},
          teapot_triangles, false);

  auto b = std::make_shared<Drawable>();
  b->name = "second-example-gizmo";
  b->position = {2, 0, 0};
  b->load({(Vertex *)teapot_vertices, _countof(teapot_vertices) / 6},
          teapot_triangles, false);

  std::list<std::shared_ptr<Drawable>> scene{
      a,
      b,
  };

  OrbitCamera orbit;
  Camera3D camera{
      .position = {0, 1.5f, 10},
      .target = {0, 0, 0},
      .up = {0, 1, 0},
      .fovy = 45.0f,
      .projection = CAMERA_PERSPECTIVE,
  };
  orbit.update_view(&camera);

  Drag right_drag{
      .draggable = std::make_shared<CameraYawPitchDragger>(&camera, &orbit),
  };

  Drag middle_drag{
      .draggable = std::make_shared<CameraShiftDragger>(&camera, &orbit),
  };

  Drawable gizmo_mesh;
  auto gizmo = std::make_shared<TranslationGizmoDragger>(&camera, scene);
  Drag left_drag{
      .draggable = gizmo,
  };

  while (!WindowShouldClose()) {
    auto w = GetScreenWidth();
    auto h = GetScreenHeight();
    auto cursor = GetMousePosition();

    hotkey active_hotkey = {
        .hotkey_ctrl = IsKeyDown(KEY_LEFT_CONTROL),
        .hotkey_translate = IsKeyDown(KEY_T),
        .hotkey_rotate = IsKeyDown(KEY_R),
        .hotkey_scale = IsKeyDown(KEY_S),
        .hotkey_local = IsKeyDown(KEY_L),
    };
    gizmo->hotkey(w, h, cursor, active_hotkey);

    // camera
    dolly(&camera);

    // render
    {
      BeginDrawing();
      glEnable(GL_DEPTH_TEST);
      glEnable(GL_BLEND);
      glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
      glDisable(GL_CULL_FACE);
      ClearBackground(RAYWHITE);

      left_drag.process(w, h, cursor, IsMouseButtonDown(MOUSE_BUTTON_LEFT));
      right_drag.process(w, h, cursor, IsMouseButtonDown(MOUSE_BUTTON_RIGHT));
      middle_drag.process(w, h, cursor, IsMouseButtonDown(MOUSE_BUTTON_MIDDLE));

      {
        BeginMode3D(camera);
        // teapot
        a->draw();
        b->draw();
        DrawGrid(10, 1.0);

        // draw gizmo
        glClear(GL_DEPTH_BUFFER_BIT);
        gizmo->load(&gizmo_mesh);
        gizmo_mesh.draw();
        EndMode3D();
      }

      EndDrawing();
    }
  }

  CloseWindow();
  return EXIT_SUCCESS;
}
