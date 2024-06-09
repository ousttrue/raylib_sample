// This is free and unencumbered software released into the public domain.
// For more information, please refer to <http://unlicense.org>
#include "tiny-gizmo.hpp"
#include <raylib.h>
#include <raylib/external/glad.h>
#include <rlgl.h>

#include "teapot.h"
#include <chrono>
#include <iostream>
#include <linalg.h>

static inline uint64_t get_local_time_ns() {
  return std::chrono::duration_cast<std::chrono::nanoseconds>(
             std::chrono::high_resolution_clock::now().time_since_epoch())
      .count();
}

tinygizmo::geometry_mesh make_teapot() {
  tinygizmo::geometry_mesh mesh;
  for (int i = 0; i < 4974; i += 6) {
    tinygizmo::geometry_vertex v;
    v.position = minalg::float3(teapot_vertices[i + 0], teapot_vertices[i + 1],
                                teapot_vertices[i + 2]);
    v.normal = minalg::float3(teapot_vertices[i + 3], teapot_vertices[i + 4],
                              teapot_vertices[i + 5]);
    mesh.vertices.push_back(v);
  }
  for (int i = 0; i < 4680; i += 3)
    mesh.triangles.push_back(minalg::uint3(teapot_triangles[i + 0],
                                           teapot_triangles[i + 1],
                                           teapot_triangles[i + 2]));
  return mesh;
}

struct camera {
  float yfov, near_clip, far_clip;
  linalg::aliases::float3 position;
  float pitch, yaw;
  linalg::aliases::float4 get_orientation() const {
    return qmul(rotation_quat(linalg::aliases::float3(0, 1, 0), yaw),
                rotation_quat(linalg::aliases::float3(1, 0, 0), pitch));
  }
  linalg::aliases::float4x4 get_view_matrix() const {
    return mul(rotation_matrix(qconj(get_orientation())),
               translation_matrix(-position));
  }
  linalg::aliases::float4x4
  get_projection_matrix(const float aspectRatio) const {
    return linalg::perspective_matrix(yfov, aspectRatio, near_clip, far_clip);
  }
  linalg::aliases::float4x4 get_viewproj_matrix(const float aspectRatio) const {
    return mul(get_projection_matrix(aspectRatio), get_view_matrix());
  }
};

struct ray {
  linalg::aliases::float3 origin;
  linalg::aliases::float3 direction;
};

struct rect {
  int x0, y0, x1, y1;
  int width() const { return x1 - x0; }
  int height() const { return y1 - y0; }
  linalg::aliases::int2 dims() const { return {width(), height()}; }
  float aspect_ratio() const { return (float)width() / height(); }
};

// Returns a world-space ray through the given pixel, originating at the camera
ray get_ray_from_pixel(const linalg::aliases::float2 &pixel,
                       const rect &viewport, const camera &cam) {
  const float x = 2 * (pixel.x - viewport.x0) / viewport.width() - 1,
              y = 1 - 2 * (pixel.y - viewport.y0) / viewport.height();
  const linalg::aliases::float4x4 inv_view_proj =
      inverse(cam.get_viewproj_matrix(viewport.aspect_ratio()));
  const linalg::aliases::float4 p0 = mul(inv_view_proj,
                                         linalg::aliases::float4(x, y, -1, 1)),
                                p1 = mul(inv_view_proj,
                                         linalg::aliases::float4(x, y, +1, 1));
  return {cam.position, p1.xyz() * p0.w - p0.xyz() * p1.w};
}

struct Drawable {
  Model model = {};

  // Generate a simple triangle mesh from code
  void load(const tinygizmo::geometry_mesh &geometry, bool dynamic) {

    Mesh mesh = {0};
    mesh.triangleCount = geometry.triangles.size();
    mesh.vertexCount = geometry.vertices.size();
    mesh.vertices = (float *)MemAlloc(mesh.vertexCount * 3 * sizeof(float));
    mesh.normals = (float *)MemAlloc(mesh.vertexCount * 3 * sizeof(float));
    mesh.colors =
        (unsigned char *)MemAlloc(mesh.vertexCount * 4 * sizeof(unsigned char));

    auto pos = 0;
    auto nom = 0;
    auto col = 0;
    for (auto &v : geometry.vertices) {
      mesh.vertices[pos++] = v.position.x;
      mesh.vertices[pos++] = v.position.y;
      mesh.vertices[pos++] = v.position.z;

      mesh.normals[nom++] = v.normal.x;
      mesh.normals[nom++] = v.normal.y;
      mesh.normals[nom++] = v.normal.z;

      mesh.colors[col++] = static_cast<unsigned char>(v.color.x * 255);
      mesh.colors[col++] = static_cast<unsigned char>(v.color.y * 255);
      mesh.colors[col++] = static_cast<unsigned char>(v.color.z * 255);
      mesh.colors[col++] = 255;
    }

    mesh.indices = (unsigned short *)MemAlloc(mesh.triangleCount * 3 *
                                              sizeof(unsigned short));
    auto index = 0;
    for (auto &t : geometry.triangles) {
      mesh.indices[index++] = t.x;
      mesh.indices[index++] = t.y;
      mesh.indices[index++] = t.z;
    }

    // Upload mesh data from CPU (RAM) to GPU (VRAM) memory
    UploadMesh(&mesh, dynamic);
    this->model = LoadModelFromMesh(mesh);
  }

  void draw(const float m[16]) {
    rlPushMatrix();
    // c.rlTranslatef(m.m12, m.m13, m.m14);
    rlMultMatrixf(m);
    // DrawCube({}, 0.5, 0.5, 0.5, YELLOW);
    DrawModel(this->model, {0, 0, 0}, 1.0f, WHITE);
    // c.DrawCylinderWires(.{}, 0, 2.0, 2, 4, c.DARKBLUE);
    rlPopMatrix();
  }
};

int main(int argc, char *argv[]) {
  bool ml = {};
  bool mr = {};
  bool bf = {};
  bool bl = {};
  bool bb = {};
  bool br = {};

  camera cam = {};
  cam.yfov = 1.0f;
  cam.near_clip = 0.01f;
  cam.far_clip = 32.0f;
  cam.position = {0, 1.5f, 4};

  tinygizmo::gizmo_application_state gizmo_state;
  tinygizmo::gizmo_context gizmo_ctx;

  InitWindow(1280, 800, "tiny-gizmo-example-app");
  // GLenum err = glewInit();
  // if (GLEW_OK != err) {
  //   /* Problem: glewInit failed, something is seriously wrong. */
  //   fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
  //   return 1;
  // }

  auto teapot = Drawable{};
  {
    auto teapot_mesh = make_teapot();
    teapot.load(teapot_mesh, false);
  }

  auto gizmo = Drawable{};

  gizmo_ctx.render = [&](const tinygizmo::geometry_mesh &r) {
    // upload_mesh(r, gizmoEditorMesh);
    gizmo.load(r, true);
    // draw_mesh(wireframeShader, gizmoEditorMesh, cam.position,
    //           cam.get_viewproj_matrix((float)GetScreenWidth() /
    //                                   (float)GetScreenHeight()),
    // identity4x4);
    float identity[] = {
        1, 0, 0, 0, //
        0, 1, 0, 0, //
        0, 0, 1, 0, //
        0, 0, 0, 1, //
    };
    gizmo.draw(identity);
  };

  minalg::float2 lastCursor;

  tinygizmo::rigid_transform xform_a;
  xform_a.position = {-2, 0, 0};

  tinygizmo::rigid_transform xform_a_last;

  tinygizmo::rigid_transform xform_b;
  xform_b.position = {+2, 0, 0};

  auto t0 = std::chrono::high_resolution_clock::now();
  while (!WindowShouldClose()) {
    auto w = GetScreenWidth();
    auto h = GetScreenHeight();
    auto aspect = static_cast<float>(w) / static_cast<float>(h);
    auto t1 = std::chrono::high_resolution_clock::now();
    float timestep = std::chrono::duration<float>(t1 - t0).count();
    t0 = t1;

    BeginDrawing();
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_CULL_FACE);
    ClearBackground(RAYWHITE);

    // keyboard
    gizmo_state.hotkey_ctrl = IsKeyDown(KEY_LEFT_CONTROL);
    gizmo_state.hotkey_local = IsKeyDown(KEY_L);
    gizmo_state.hotkey_translate = IsKeyDown(KEY_T);
    gizmo_state.hotkey_rotate = IsKeyDown(KEY_R);
    gizmo_state.hotkey_scale = IsKeyDown(KEY_S);
    bf = IsKeyDown(KEY_W);
    bl = IsKeyDown(KEY_A);
    bb = IsKeyDown(KEY_S);
    br = IsKeyDown(KEY_D);
    // mouse
    gizmo_state.mouse_left = IsMouseButtonDown(MOUSE_BUTTON_LEFT);
    ml = IsMouseButtonDown(MOUSE_BUTTON_LEFT);
    mr = IsMouseButtonDown(MOUSE_BUTTON_RIGHT);
    auto position = GetMousePosition();
    auto deltaCursorMotion =
        minalg::float2(position.x, position.y) - lastCursor;
    lastCursor = minalg::float2(position.x, position.y);
    if (mr) {
      cam.yaw -= deltaCursorMotion.x * 0.01f;
      cam.pitch -= deltaCursorMotion.y * 0.01f;
    }
    if (mr) {
      const linalg::aliases::float4 orientation = cam.get_orientation();
      linalg::aliases::float3 move;
      if (bf)
        move -= qzdir(orientation);
      if (bl)
        move -= qxdir(orientation);
      if (bb)
        move += qzdir(orientation);
      if (br)
        move += qxdir(orientation);
      if (length2(move) > 0)
        cam.position += normalize(move) * (timestep * 10);
    }

    // gizmo
    auto cameraOrientation = cam.get_orientation();
    const auto rayDir =
        get_ray_from_pixel({lastCursor.x, lastCursor.y}, {0, 0, w, h}, cam)
            .direction;
    // Gizmo input interaction state populated via win->on_input(...) callback
    // above. Update app parameters:
    gizmo_state.viewport_size =
        minalg::float2(static_cast<float>(w), static_cast<float>(h));
    gizmo_state.cam.near_clip = cam.near_clip;
    gizmo_state.cam.far_clip = cam.far_clip;
    gizmo_state.cam.yfov = cam.yfov;
    gizmo_state.cam.position =
        minalg::float3(cam.position.x, cam.position.y, cam.position.z);
    gizmo_state.cam.orientation =
        minalg::float4(cameraOrientation.x, cameraOrientation.y,
                       cameraOrientation.z, cameraOrientation.w);
    gizmo_state.ray_origin =
        minalg::float3(cam.position.x, cam.position.y, cam.position.z);
    gizmo_state.ray_direction = minalg::float3(rayDir.x, rayDir.y, rayDir.z);
    // gizmo_state.screenspace_scale = 80.f; // optional flag to draw the gizmos
    // at a constant screen-space scale

    // render
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
      auto ma = xform_a.matrix();
      teapot.draw(&ma.x.x);
      auto mb = xform_b.matrix();
      teapot.draw(&mb.x.x);
      DrawGrid(10, 1.0);

      // gizmo
      glClear(GL_DEPTH_BUFFER_BIT);
      gizmo_ctx.update(gizmo_state);

      if (transform_gizmo("first-example-gizmo", gizmo_ctx, xform_a)) {
        std::cout << get_local_time_ns() << " - " << "First Gizmo Hovered..."
                  << std::endl;
        if (xform_a != xform_a_last)
          std::cout << get_local_time_ns() << " - " << "First Gizmo Changed..."
                    << std::endl;
        xform_a_last = xform_a;
      }

      transform_gizmo("second-example-gizmo", gizmo_ctx, xform_b);
      gizmo_ctx.draw();

      EndDrawing();
    }
  }
  return EXIT_SUCCESS;
}
