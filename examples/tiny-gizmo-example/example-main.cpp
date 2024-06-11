// This is free and unencumbered software released into the public domain.
// For more information, please refer to <http://unlicense.org>
#include <raylib/external/glad.h>

#include <tiny-gizmo.hpp>

#include "orbit_camera.h"
#include "teapot.h"
#include <rlgl.h>

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

struct Vertex {
  Vector3 position;
  Vector3 color;
};

int main(int argc, char *argv[]) {

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
  Camera3D camera{
      .position = {0, 1.5f, 10},
      .target = {0, 0, 0},
      .up = {0, 1, 0},
      .fovy = 45.0f,
      .projection = CAMERA_PERSPECTIVE,
  };

  tinygizmo::gizmo_context gizmo_ctx;
  std::vector<tinygizmo::geometry_vertex> gizmo_vertices;
  std::vector<uint32_t> gizmo_indices;
  Drawable gizmo_mesh;

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
    tinygizmo::gizmo_application_state gizmo_state{
        .mouse_left = IsMouseButtonDown(MOUSE_BUTTON_LEFT),
        // trs
        .hotkey_translate = IsKeyDown(KEY_T),
        .hotkey_rotate = IsKeyDown(KEY_R),
        .hotkey_scale = IsKeyDown(KEY_S),
        // keyboard
        .hotkey_local = IsKeyDown(KEY_L),
        .hotkey_ctrl = IsKeyDown(KEY_LEFT_CONTROL),
        // optional flag to draw the gizmos at a constant screen-space scale
        // gizmo_state.screenspace_scale = 80.f;
        // camera projection
        .viewport_size =
            minalg::float2(static_cast<float>(w), static_cast<float>(h)),
        .ray_origin =
            minalg::float3(ray.position.x, ray.position.y, ray.position.z),
        .ray_direction =
            minalg::float3(ray.direction.x, ray.direction.y, ray.direction.z),
        .cam =
            {
                .yfov = 1.0f,
                .near_clip = 0.01f,
                .far_clip = 32.0f,
                .position = minalg::float3(ray.position.x, ray.position.y,
                                           ray.position.z),
                .orientation = minalg::float4(rot.x, rot.y, rot.z, rot.w),
            },
    };
    gizmo_ctx.update(gizmo_state);
    tinygizmo::transform_gizmo("first-example-gizmo", gizmo_ctx, xform_a);
    auto ma = xform_a.matrix();
    tinygizmo::transform_gizmo("second-example-gizmo", gizmo_ctx, xform_b);
    auto mb = xform_b.matrix();
    auto drawlist = gizmo_ctx.drawlist();

    // update gizmo mesh
    gizmo_vertices.clear();
    gizmo_indices.clear();
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
