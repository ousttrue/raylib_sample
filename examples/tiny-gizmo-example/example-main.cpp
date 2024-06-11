// This is free and unencumbered software released into the public domain.
// For more information, please refer to <http://unlicense.org>
#include <raylib/external/glad.h>

#include <tiny-gizmo.hpp>

#include "teapot.h"

#include <raylib.h>
#include <raymath.h>
#include <rcamera.h>
#include <rlgl.h>
#include <span>
#include <string_view>

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

      mesh.colors[col++] = static_cast<unsigned char>(v.color.x * 255);
      mesh.colors[col++] = static_cast<unsigned char>(v.color.y * 255);
      mesh.colors[col++] = static_cast<unsigned char>(v.color.z * 255);
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

struct Gizmo {
  tinygizmo::gizmo_application_state gizmo_state;
  tinygizmo::gizmo_context gizmo_ctx;

  void new_frame(const Ray &ray, int width, int height, float fovy) {
    // mouse
    gizmo_state.mouse_left = IsMouseButtonDown(MOUSE_BUTTON_LEFT);
    // keyboard
    gizmo_state.hotkey_ctrl = IsKeyDown(KEY_LEFT_CONTROL);
    gizmo_state.hotkey_local = IsKeyDown(KEY_L);
    gizmo_state.hotkey_translate = IsKeyDown(KEY_T);
    gizmo_state.hotkey_rotate = IsKeyDown(KEY_R);
    gizmo_state.hotkey_scale = IsKeyDown(KEY_S);
    // camera projection
    gizmo_state.viewport_size =
        minalg::float2(static_cast<float>(width), static_cast<float>(height));
    gizmo_state.cam.near_clip = 0.01f;
    gizmo_state.cam.far_clip = 32.0f;
    gizmo_state.cam.yfov = 1.0f;
    // camer view
    gizmo_state.cam.position =
        minalg::float3(ray.position.x, ray.position.y, ray.position.z);
    auto rot =
        QuaternionFromEuler(ray.direction.x, ray.direction.y, ray.direction.z);
    gizmo_state.cam.orientation = minalg::float4(rot.x, rot.y, rot.z, rot.w);
    gizmo_state.ray_origin = gizmo_state.cam.position;
    gizmo_state.ray_direction =
        minalg::float3(ray.direction.x, ray.direction.y, ray.direction.z);
    // optional flag to draw the gizmos at a constant screen-space scale
    // gizmo_state.screenspace_scale = 80.f;
    gizmo_ctx.update(gizmo_state);
  }

  tinygizmo::rigid_transform
  transform(const std::string_view name,
            const tinygizmo::rigid_transform &transform) {
    auto t = transform;
    tinygizmo::transform_gizmo({name.begin(), name.end()}, gizmo_ctx, t);
    return t;
  }
};
struct Vertex {
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
    teapot.load<Vertex, uint32_t>(
        {(Vertex *)teapot_vertices, _countof(teapot_vertices) / 6},
        teapot_triangles, false);
  }

  tinygizmo::rigid_transform xform_a;
  xform_a.position = {-2, 0, 0};
  tinygizmo::rigid_transform xform_b;
  xform_b.position = {2, 0, 0};

  OrbitCamera orbit;

  Gizmo gizmo;
  std::vector<tinygizmo::geometry_vertex> gizmo_vertices;
  std::vector<uint32_t> gizmo_indices;
  Drawable gizmo_mesh;

  while (!WindowShouldClose()) {
    auto w = GetScreenWidth();
    auto h = GetScreenHeight();

    // mouse
    dolly(&camera);
    auto distance = Vector3Distance(camera.target, camera.position);
    if (orbit.MouseUpdateCamera(distance, camera.fovy,
                                {
                                    0,
                                    0,
                                    static_cast<float>(w),
                                    static_cast<float>(h),
                                })) {
      orbit.update_view(&camera);
    }

    // gizmo
    auto ray = GetMouseRay(GetMousePosition(), camera);
    gizmo.new_frame(ray, w, h, camera.fovy * DEG2RAD);
    xform_a = gizmo.transform("first-example-gizmo", xform_a);
    auto ma = xform_a.matrix();
    xform_b = gizmo.transform("second-example-gizmo", xform_b);
    auto mb = xform_b.matrix();

    // update gizmo mesh
    gizmo_vertices.clear();
    gizmo_indices.clear();
    auto drawlist = gizmo.gizmo_ctx.drawlist();
    for (auto &m : drawlist) {
      uint32_t numVerts = (uint32_t)gizmo_vertices.size();
      auto it = gizmo_vertices.insert(
          gizmo_vertices.end(), m.mesh.vertices.begin(), m.mesh.vertices.end());
      for (auto &t : m.mesh.triangles) {
        gizmo_indices.push_back(numVerts + t.x);
        gizmo_indices.push_back(numVerts + t.y);
        gizmo_indices.push_back(numVerts + t.z);
      }
      for (; it != gizmo_vertices.end(); ++it) {
        // Take the color and shove it into a per-vertex attribute
        it->color = m.color;
      }
    }
    if (gizmo_vertices.size()) {
      gizmo_mesh.load<tinygizmo::geometry_vertex, uint32_t>(
          gizmo_vertices, gizmo_indices, true);
    }

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
