// This is free and unencumbered software released into the public domain.
// For more information, please refer to <http://unlicense.org>
#include <algorithm>
#include <assert.h>
#include <raylib/external/glad.h>
#include <span>
#include <tiny-gizmo.h>

#include "orbit_camera.h"
#include "rdrag.h"
#include "teapot.h"
#include <rlgl.h>

enum class transform_mode {
  translate,
  rotate,
  scale,
};
void transform_gizmo(tinygizmo::gizmo_context *gizmo_ctx,
                     const tinygizmo::gizmo_state &state,
                     const tinygizmo::AddTriangleFunc &add_world_triangle,
                     transform_mode mode, bool uniform, const std::string &name,
                     Vector3 &t, Quaternion &r, Vector3 &s) {

  auto id = tinygizmo::hash_fnv1a(name);
  auto gizmo = gizmo_ctx->get_or_create(id);
  // interaction_mode will only change on clicked
  if (state.has_clicked) {
    gizmo->active = nullptr;
  } else if (state.has_released) {
    gizmo->active = nullptr;
  }

  rigid_transform p(*(minalg::float4 *)&r.x, *(minalg::float3 *)&t.x,
                    *(minalg::float3 *)&s.x);
  switch (mode) {
  case transform_mode::translate: {
    auto result = gizmo->position_gizmo(state, add_world_triangle, p);
    if (result.active) {
      t = *(Vector3 *)&result.t;
    }
    break;
  }
  case transform_mode::rotate: {
    auto result = gizmo->rotation_gizmo(state, add_world_triangle, p);
    if (result.active) {
      r = *(Quaternion *)&result.r;
    }
    break;
  }
  case transform_mode::scale: {
    auto result = gizmo->scale_gizmo(state, add_world_triangle, p, uniform);
    if (result.active) {
      s = *(Vector3 *)&result.s;
    }
    break;
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

    // vertices
    auto mesh_vertices = (Vector3 *)MemAlloc(vertices.size() * sizeof(Vector3));
    // mesh.normals = (float *)MemAlloc(mesh.vertexCount * 3 * sizeof(float));
    auto mesh_colors = (Color *)MemAlloc(vertices.size() * sizeof(Color));
    auto pos = 0;
    // auto nom = 0;
    auto col = 0;
    for (auto &v : vertices) {
      mesh_vertices[pos++] = {
          v.position.x,
          v.position.y,
          v.position.z,
      };

      // mesh.normals[nom++] = v.normal.x;
      // mesh.normals[nom++] = v.normal.y;
      // mesh.normals[nom++] = v.normal.z;

      mesh_colors[col++] = {
          static_cast<unsigned char>(std::max(0.0f, v.color.x) * 255),
          static_cast<unsigned char>(std::max(0.0f, v.color.y) * 255),
          static_cast<unsigned char>(std::max(0.0f, v.color.z) * 255),
          255,
      };
    }

    auto mesh_indices =
        (unsigned short *)MemAlloc(indices.size() * sizeof(unsigned short));
    auto index = 0;
    for (auto &i : indices) {
      mesh_indices[index++] = i;
    }

    load(vertices.size(), mesh_vertices, mesh_colors, indices.size(),
         mesh_indices, dynamic);
  }

  void load(size_t vertexCount, const Vector3 *vertices, const Color *colors,
            size_t indexCount, const unsigned short *indices, bool dynamic) {
    // Upload mesh data from CPU (RAM) to GPU (VRAM) memory
    Mesh mesh = {
        .vertexCount = static_cast<int>(vertexCount),
        .triangleCount = static_cast<int>(indexCount / 3),
        .vertices = const_cast<float *>(&vertices[0].x),
        .colors = const_cast<unsigned char *>(&colors[0].r),
        .indices = const_cast<unsigned short *>(indices),
    };
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
  bool hotkey_ctrl = false;
  bool hotkey_translate = false;
  bool hotkey_rotate = false;
  bool hotkey_scale = false;
  bool hotkey_local = false;
};

class CameraYawPitchDragger : public Dragger {
  Camera *_camera;
  OrbitCamera *_orbit;
  Vector2 _last;

public:
  CameraYawPitchDragger(Camera *camera, OrbitCamera *orbit)
      : _camera(camera), _orbit(orbit) {}

  void begin(const Vector2 &cursor) override { _last = cursor; }

  void end(const Vector2 &end) override {}

  void drag(const DragState &state, const Vector2 &cursor, int w,
            int h) override {
    auto delta = Vector2Subtract(cursor, _last);
    _last = cursor;
    auto distance = Vector3Distance(_camera->target, _camera->position);
    _orbit->YawPitch(delta, distance, _camera->fovy,
                     {
                         0,
                         0,
                         static_cast<float>(w),
                         static_cast<float>(h),
                     });
    _orbit->update_view(_camera);
  }
};

class CameraShiftDragger : public Dragger {
  Camera *_camera;
  OrbitCamera *_orbit;
  Vector2 _last;

public:
  CameraShiftDragger(Camera *camera, OrbitCamera *orbit)
      : _camera(camera), _orbit(orbit) {}

  void begin(const Vector2 &cursor) override { _last = cursor; }

  void end(const Vector2 &end) override {}

  void drag(const DragState &state, const Vector2 &cursor, int w,
            int h) override {
    auto delta = Vector2Subtract(cursor, _last);
    _last = cursor;
    auto distance = Vector3Distance(_camera->target, _camera->position);
    _orbit->Shift(delta, distance, _camera->fovy,
                  {
                      0,
                      0,
                      static_cast<float>(w),
                      static_cast<float>(h),
                  });
    _orbit->update_view(_camera);
  }
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
  orbit.update_view(&camera);

  tinygizmo::gizmo_context gizmo_ctx;
  auto mode = transform_mode::translate;
  // State to describe if the gizmo should use transform-local math
  bool local_toggle{true};
  Drawable gizmo_mesh;

  tinygizmo::gizmo_application_state last_state;
  hotkey last_hotkey{0};
  std::vector<Vector3> positions;
  std::vector<Color> colors;
  std::vector<unsigned short> indices;

  Drag left_drag;
  Drag right_drag{
      .draggable = std::make_shared<CameraYawPitchDragger>(&camera, &orbit),
  };
  Drag middle_drag{
      .draggable = std::make_shared<CameraShiftDragger>(&camera, &orbit),
  };

  while (!WindowShouldClose()) {
    auto w = GetScreenWidth();
    auto h = GetScreenHeight();

    // camera
    dolly(&camera);

    // gizmo
    auto cursor = GetMousePosition();
    auto ray = GetMouseRay(cursor, camera);
    auto rot =
        QuaternionFromEuler(ray.direction.x, ray.direction.y, ray.direction.z);

    hotkey active_hotkey = {
        .hotkey_ctrl = IsKeyDown(KEY_LEFT_CONTROL),
        .hotkey_translate = IsKeyDown(KEY_T),
        .hotkey_rotate = IsKeyDown(KEY_R),
        .hotkey_scale = IsKeyDown(KEY_S),
        .hotkey_local = IsKeyDown(KEY_L),
    };

    if (active_hotkey.hotkey_ctrl == true) {
      if (last_hotkey.hotkey_translate == false &&
          active_hotkey.hotkey_translate == true)
        mode = transform_mode::translate;
      else if (last_hotkey.hotkey_rotate == false &&
               active_hotkey.hotkey_rotate == true)
        mode = transform_mode::rotate;
      else if (last_hotkey.hotkey_scale == false &&
               active_hotkey.hotkey_scale == true)
        mode = transform_mode::scale;

      local_toggle = (!last_hotkey.hotkey_local && active_hotkey.hotkey_local)
                         ? !local_toggle
                         : local_toggle;
    }

    tinygizmo::gizmo_state state(
        local_toggle,
        {
            .mouse_left = IsMouseButtonDown(MOUSE_BUTTON_LEFT),
            // optional flag to draw the gizmos at a constant screen-space
            // scale gizmo_state.screenspace_scale = 80.f; camera projection
            .viewport_size = {static_cast<float>(w), static_cast<float>(h)},
            .ray_origin = {ray.position.x, ray.position.y, ray.position.z},
            .ray_direction = {ray.direction.x, ray.direction.y,
                              ray.direction.z},
            .cam_yfov = 1.0f,
            .cam_orientation = {rot.x, rot.y, rot.z, rot.w},
        },
        last_state);

    {
      positions.clear();
      colors.clear();
      indices.clear();
      tinygizmo::AddTriangleFunc add_world_triangle =
          [&positions, &colors, &indices](
              const std::array<float, 4> &rgba, const std::array<float, 3> &p0,
              const std::array<float, 3> &p1, const std::array<float, 3> &p2) {
            //
            auto offset = positions.size();
            Color color{
                static_cast<unsigned char>(std::max(0.0f, rgba[0]) * 255),
                static_cast<unsigned char>(std::max(0.0f, rgba[1]) * 255),
                static_cast<unsigned char>(std::max(0.0f, rgba[2]) * 255),
                static_cast<unsigned char>(std::max(0.0f, rgba[3]) * 255),
            };
            positions.push_back({p0[0], p0[1], p0[2]});
            positions.push_back({p1[0], p1[1], p1[2]});
            positions.push_back({p2[0], p2[1], p2[2]});
            colors.push_back(color);
            colors.push_back(color);
            colors.push_back(color);
            indices.push_back(offset + 0);
            indices.push_back(offset + 1);
            indices.push_back(offset + 2);
          };

      // gizmo_ctx.begin_frame(active_state);
      transform_gizmo(&gizmo_ctx, state, add_world_triangle, mode,
                      active_hotkey.hotkey_ctrl, "first-example-gizmo", a_t,
                      a_r, a_s);
      transform_gizmo(&gizmo_ctx, state, add_world_triangle, mode,
                      active_hotkey.hotkey_ctrl, "second-example-gizmo", b_t,
                      b_r, b_s);

      // end_frame(state.drawlist, vertices, indices);
      if (positions.size() && indices.size()) {
        assert(positions.size() == colors.size());
        // update gizmo mesh
        gizmo_mesh.load(positions.size(), positions.data(), colors.data(),
                        indices.size(), indices.data(), true);
      }
    }
    last_hotkey = active_hotkey;
    last_state = state.active_state;

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

    left_drag.process(cursor, w, h, IsMouseButtonDown(MOUSE_BUTTON_LEFT));
    right_drag.process(cursor, w, h, IsMouseButtonDown(MOUSE_BUTTON_RIGHT));
    middle_drag.process(cursor, w, h, IsMouseButtonDown(MOUSE_BUTTON_MIDDLE));
    EndDrawing();
  }

  CloseWindow();
  return EXIT_SUCCESS;
}
