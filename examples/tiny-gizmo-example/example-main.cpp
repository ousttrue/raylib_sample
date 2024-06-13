// This is free and unencumbered software released into the public domain.
// For more information, please refer to <http://unlicense.org>
#include <raylib/external/glad.h>

#include <algorithm>
#include <tiny-gizmo.hpp>

#include "orbit_camera.h"
#include "teapot.h"
#include <rlgl.h>

enum class transform_mode {
  translate,
  rotate,
  scale,
};
bool transform_gizmo(tinygizmo::gizmo_context *gizmo, transform_mode mode,
                     bool local_toggle, bool uniform, const std::string &name,
                     Vector3 &t, Quaternion &r, Vector3 &s) {

  auto id = tinygizmo::hash_fnv1a(name);

  switch (mode) {
  case transform_mode::translate: {
    auto result = gizmo->translation_gizmo(local_toggle, id, &t.x, &r.x);
    if (result.active) {
      t = *(Vector3 *)&result.t;
    }
    return result.hover || result.active;
  }
  case transform_mode::rotate: {
    auto result = gizmo->rotationn_gizmo(local_toggle, id, &t.x, &r.x);
    if (result.active) {
      r = *(Quaternion *)&result.r;
    }
    return result.hover || result.active;
  }
  case transform_mode::scale: {
    auto result =
        gizmo->scale_gizmo(local_toggle, uniform, id, &t.x, &r.x, &s.x);
    if (result.active) {
      s = *(Vector3 *)&result.s;
    }
    return result.hover || result.active;
  }
  }
}

static Matrix TRS(const Vector3 &t, const Quaternion &r, const Vector3 &s) {
  return MatrixMultiply(
      MatrixMultiply(MatrixScale(s.x, s.y, s.z), QuaternionToMatrix(r)),
      MatrixTranslate(t.x, t.y, t.z));
}

struct Drawable {
  Model model = {};

  // Generate a simple triangle mesh from code
  template <typename V, typename I>
  void load(std::span<const V> vertices, std::span<const I> indices,
            bool dynamic) {

    Mesh mesh = {0};

    // vertices
    mesh.vertexCount = vertices.size();
    mesh.vertices = (float *)MemAlloc(mesh.vertexCount * 3 * sizeof(float));
    // mesh.normals = (float *)MemAlloc(mesh.vertexCount * 3 * sizeof(float));
    mesh.colors =
        (unsigned char *)MemAlloc(mesh.vertexCount * 4 * sizeof(unsigned char));
    auto pos = 0;
    // auto nom = 0;
    auto col = 0;
    for (auto &v : vertices) {
      mesh.vertices[pos++] = v.position.x;
      mesh.vertices[pos++] = v.position.y;
      mesh.vertices[pos++] = v.position.z;

      // mesh.normals[nom++] = v.normal.x;
      // mesh.normals[nom++] = v.normal.y;
      // mesh.normals[nom++] = v.normal.z;

      mesh.colors[col++] =
          static_cast<unsigned char>(std::max(0.0f, v.color.x) * 255);
      mesh.colors[col++] =
          static_cast<unsigned char>(std::max(0.0f, v.color.y * 255));
      mesh.colors[col++] =
          static_cast<unsigned char>(std::max(0.0f, v.color.z * 255));
      mesh.colors[col++] = 255;
    }

    mesh.triangleCount = indices.size() / 3;
    mesh.indices = (unsigned short *)MemAlloc(mesh.triangleCount * 3 *
                                              sizeof(unsigned short));
    auto index = 0;
    for (auto &i : indices) {
      mesh.indices[index++] = i;
    }

    // Upload mesh data from CPU (RAM) to GPU (VRAM) memory
    UploadMesh(&mesh, dynamic);
    this->model = LoadModelFromMesh(mesh);
  }

  void draw(const Matrix &_m) {
    rlPushMatrix();
    auto m = MatrixTranspose(_m);
    rlMultMatrixf(&m.m0);
    DrawModel(this->model, {0, 0, 0}, 1.0f, WHITE);
    rlPopMatrix();
  }
};

struct Vertex {
  Vector3 position;
  Vector3 color;
};

struct hotkey {
  bool hotkey_ctrl{false};
  bool hotkey_translate{false};
  bool hotkey_rotate{false};
  bool hotkey_scale{false};
  bool hotkey_local{false};
};

int main(int argc, char *argv[]) {

  InitWindow(1280, 800, "tiny-gizmo-example-app");

  auto teapot = Drawable{};
  {
    teapot.load<Vertex, uint32_t>(
        {(Vertex *)teapot_vertices, _countof(teapot_vertices) / 6},
        teapot_triangles, false);
  }

  Vector3 a_t = {-2, 0, 0};
  Quaternion a_r = {0, 0, 0, 1};
  Vector3 a_s = {1, 1, 1};
  Vector3 b_t = {2, 0, 0};
  Quaternion b_r = {0, 0, 0, 1};
  Vector3 b_s = {1, 1, 1};

  OrbitCamera orbit;
  Camera3D camera{
      .position = {0, 1.5f, 10},
      .target = {0, 0, 0},
      .up = {0, 1, 0},
      .fovy = 45.0f,
      .projection = CAMERA_PERSPECTIVE,
  };

  tinygizmo::gizmo_context gizmo_ctx;
  auto mode = transform_mode::translate;
  // State to describe if the gizmo should use transform-local math
  bool local_toggle{true};
  Drawable gizmo_mesh;

  hotkey last_state{0};

  while (!WindowShouldClose()) {
    auto w = GetScreenWidth();
    auto h = GetScreenHeight();

    // mouse
    dolly(&camera);
    auto distance = Vector3Distance(camera.target, camera.position);
    orbit.MouseUpdateCamera(distance, camera.fovy,
                            {
                                0,
                                0,
                                static_cast<float>(w),
                                static_cast<float>(h),
                            });
    orbit.update_view(&camera);

    // gizmo
    auto ray = GetMouseRay(GetMousePosition(), camera);
    auto rot =
        QuaternionFromEuler(ray.direction.x, ray.direction.y, ray.direction.z);
    // mouse
    tinygizmo::gizmo_application_state active_state{
        .mouse_left = IsMouseButtonDown(MOUSE_BUTTON_LEFT),
        // optional flag to draw the gizmos at a constant screen-space scale
        // gizmo_state.screenspace_scale = 80.f;
        // camera projection
        .viewport_size = {static_cast<float>(w), static_cast<float>(h)},
        .ray_origin = {ray.position.x, ray.position.y, ray.position.z},
        .ray_direction = {ray.direction.x, ray.direction.y, ray.direction.z},
        .cam_yfov = 1.0f,
        .cam_orientation = {rot.x, rot.y, rot.z, rot.w},
    };

    hotkey active_hotkey = {
        .hotkey_ctrl = IsKeyDown(KEY_LEFT_CONTROL),
        .hotkey_translate = IsKeyDown(KEY_T),
        .hotkey_rotate = IsKeyDown(KEY_R),
        .hotkey_scale = IsKeyDown(KEY_S),
        .hotkey_local = IsKeyDown(KEY_L),
    };

    if (active_hotkey.hotkey_ctrl == true) {
      if (last_state.hotkey_translate == false &&
          active_hotkey.hotkey_translate == true)
        mode = transform_mode::translate;
      else if (last_state.hotkey_rotate == false &&
               active_hotkey.hotkey_rotate == true)
        mode = transform_mode::rotate;
      else if (last_state.hotkey_scale == false &&
               active_hotkey.hotkey_scale == true)
        mode = transform_mode::scale;

      local_toggle = (!last_state.hotkey_local && active_hotkey.hotkey_local)
                         ? !local_toggle
                         : local_toggle;
    }

    {
      gizmo_ctx.begin_frame(active_state);
      transform_gizmo(&gizmo_ctx, mode, local_toggle, active_hotkey.hotkey_ctrl,
                      "first-example-gizmo", a_t, a_r, a_s);
      transform_gizmo(&gizmo_ctx, mode, local_toggle, active_hotkey.hotkey_ctrl,
                      "second-example-gizmo", b_t, b_r, b_s);
      auto [vertices, indices] = gizmo_ctx.end_frame();
      if (vertices.size() && indices.size()) {
        // update gizmo mesh
        gizmo_mesh.load<tinygizmo::draw_vertex, uint32_t>(vertices, indices,
                                                          true);
      }
    }
    last_state = active_hotkey;

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
      teapot.draw(TRS(a_t, a_r, a_s));
      teapot.draw(TRS(b_t, b_r, b_s));
      DrawGrid(10, 1.0);

      // draw gizmo
      glClear(GL_DEPTH_BUFFER_BIT);
      gizmo_mesh.draw(MatrixIdentity());
    }
    EndMode3D();
    EndDrawing();
  }

  CloseWindow();
  return EXIT_SUCCESS;
}
