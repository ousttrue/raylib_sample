// This is free and unencumbered software released into the public domain.
// For more information, please refer to <http://unlicense.org>

#include <chrono>
#include <iostream>

#include "gl-api.hpp"
#include <raylib.h>
#include "teapot.h"
#include "tiny-gizmo.hpp"

static inline uint64_t get_local_time_ns() {
  return std::chrono::duration_cast<std::chrono::nanoseconds>(
             std::chrono::high_resolution_clock::now().time_since_epoch())
      .count();
}

const linalg::aliases::float4x4 identity4x4 = {
    {1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}};

constexpr const char gizmo_vert[] = R"(#version 330
    layout(location = 0) in vec3 vertex;
    layout(location = 1) in vec3 normal;
    layout(location = 2) in vec4 color;
    out vec4 v_color;
    out vec3 v_world, v_normal;
    uniform mat4 u_mvp;
    void main()
    {
        gl_Position = u_mvp * vec4(vertex.xyz, 1);
        v_color = color;
        v_world = vertex;
        v_normal = normal;
    }
)";

constexpr const char gizmo_frag[] = R"(#version 330
    in vec4 v_color;
    in vec3 v_world, v_normal;
    out vec4 f_color;
    uniform vec3 u_eye;
    void main()
    {
        vec3 light = vec3(1) * max(dot(v_normal, normalize(u_eye - v_world)), 0.50) + 0.25;
        f_color = v_color * vec4(light, 1);
    }
)";

constexpr const char lit_vert[] = R"(#version 330
    uniform mat4 u_modelMatrix;
    uniform mat4 u_viewProj;

    layout(location = 0) in vec3 inPosition;
    layout(location = 1) in vec3 inNormal;

    out vec3 v_position, v_normal;

    void main()
    {
        vec4 worldPos = u_modelMatrix * vec4(inPosition, 1);
        v_position = worldPos.xyz;
        v_normal = normalize((u_modelMatrix * vec4(inNormal,0)).xyz);
        gl_Position = u_viewProj * worldPos;
    }
)";

constexpr const char lit_frag[] = R"(#version 330
    uniform vec3 u_diffuse = vec3(1, 1, 1);
    uniform vec3 u_eye;

    in vec3 v_position;
    in vec3 v_normal;

    out vec4 f_color;
    
    vec3 compute_lighting(vec3 eyeDir, vec3 position, vec3 color)
    {
        vec3 light = vec3(0, 0, 0);
        vec3 lightDir = normalize(position - v_position);
        light += color * u_diffuse * max(dot(v_normal, lightDir), 0);
        vec3 halfDir = normalize(lightDir + eyeDir);
        light += color * u_diffuse * pow(max(dot(v_normal, halfDir), 0), 128);
        return light;
    }

    void main()
    {
        vec3 eyeDir = vec3(0, 1, -2);
        vec3 light = vec3(0, 0, 0);
        light += compute_lighting(eyeDir, vec3(+3, 1, 0), vec3(235.0/255.0, 43.0/255.0, 211.0/255.0));
        light += compute_lighting(eyeDir, vec3(-3, 1, 0), vec3(43.0/255.0, 236.0/255.0, 234.0/255.0));
        f_color = vec4(light + vec3(0.5, 0.5, 0.5), 1.0);
    }
)";

//////////////////////////
//   Main Application   //
//////////////////////////

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

void draw_mesh(GlShader &shader, GlMesh &mesh,
               const linalg::aliases::float3 eye,
               const linalg::aliases::float4x4 &viewProj,
               const linalg::aliases::float4x4 &model) {
  linalg::aliases::float4x4 modelViewProjectionMatrix = mul(viewProj, model);
  shader.bind();
  shader.uniform("u_mvp", modelViewProjectionMatrix);
  shader.uniform("u_eye", eye);
  mesh.draw_elements();
  shader.unbind();
}

void draw_lit_mesh(GlShader &shader, GlMesh &mesh,
                   const linalg::aliases::float3 eye,
                   const linalg::aliases::float4x4 &viewProj,
                   const linalg::aliases::float4x4 &model) {
  shader.bind();
  shader.uniform("u_viewProj", viewProj);
  shader.uniform("u_modelMatrix", model);
  shader.uniform("u_eye", eye);
  mesh.draw_elements();
  shader.unbind();
}

void upload_mesh(const tinygizmo::geometry_mesh &cpu, GlMesh &gpu) {
  const auto &verts =
      reinterpret_cast<const std::vector<linalg::aliases::float3> &>(
          cpu.vertices);
  const auto &tris =
      reinterpret_cast<const std::vector<linalg::aliases::uint3> &>(
          cpu.triangles);
  gpu.set_vertices(verts, GL_DYNAMIC_DRAW);
  gpu.set_attribute(0, 3, GL_FLOAT, GL_FALSE,
                    sizeof(tinygizmo::geometry_vertex),
                    (GLvoid *)offsetof(tinygizmo::geometry_vertex, position));
  gpu.set_attribute(1, 3, GL_FLOAT, GL_FALSE,
                    sizeof(tinygizmo::geometry_vertex),
                    (GLvoid *)offsetof(tinygizmo::geometry_vertex, normal));
  gpu.set_attribute(2, 4, GL_FLOAT, GL_FALSE,
                    sizeof(tinygizmo::geometry_vertex),
                    (GLvoid *)offsetof(tinygizmo::geometry_vertex, color));
  gpu.set_elements(tris, GL_DYNAMIC_DRAW);
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
  GLenum err = glewInit();
  if (GLEW_OK != err) {
    /* Problem: glewInit failed, something is seriously wrong. */
    fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
    return 1;
  }

  GlMesh gizmoEditorMesh, teapotMesh;

  auto wireframeShader = GlShader(gizmo_vert, gizmo_frag);
  auto litShader = GlShader(lit_vert, lit_frag);

  tinygizmo::geometry_mesh teapot = make_teapot();
  upload_mesh(teapot, teapotMesh);

  gizmo_ctx.render = [&](const tinygizmo::geometry_mesh &r) {
    upload_mesh(r, gizmoEditorMesh);
    draw_mesh(wireframeShader, gizmoEditorMesh, cam.position,
              cam.get_viewproj_matrix((float)GetScreenWidth() /
                                      (float)GetScreenHeight()),
              identity4x4);
  };

  minalg::float2 lastCursor;

  tinygizmo::rigid_transform xform_a;
  xform_a.position = {-2, 0, 0};

  tinygizmo::rigid_transform xform_a_last;

  tinygizmo::rigid_transform xform_b;
  xform_b.position = {+2, 0, 0};

  auto t0 = std::chrono::high_resolution_clock::now();
  while (!WindowShouldClose()) {
    BeginDrawing();

    gizmo_state.hotkey_ctrl = IsKeyDown(KEY_LEFT_CONTROL);
    gizmo_state.hotkey_local = IsKeyDown(KEY_L);
    gizmo_state.hotkey_translate = IsKeyDown(KEY_T);
    gizmo_state.hotkey_rotate = IsKeyDown(KEY_R);
    gizmo_state.hotkey_scale = IsKeyDown(KEY_S);
    bf = IsKeyDown(KEY_W);
    bl = IsKeyDown(KEY_A);
    bb = IsKeyDown(KEY_S);
    br = IsKeyDown(KEY_D);

    gizmo_state.mouse_left = IsMouseButtonDown(MOUSE_BUTTON_LEFT);
    ml = IsMouseButtonDown(MOUSE_BUTTON_LEFT);
    mr = IsMouseButtonDown(MOUSE_BUTTON_RIGHT);

    auto position = GetMousePosition();
    auto deltaCursorMotion =
        minalg::float2(position.x, position.y) - lastCursor;
    if (mr) {
      cam.yaw -= deltaCursorMotion.x * 0.01f;
      cam.pitch -= deltaCursorMotion.y * 0.01f;
    }
    lastCursor = minalg::float2(position.x, position.y);

    auto t1 = std::chrono::high_resolution_clock::now();
    float timestep = std::chrono::duration<float>(t1 - t0).count();
    t0 = t1;

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

    auto w = GetScreenWidth();
    auto h = GetScreenHeight();

    glViewport(0, 0, w, h);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glClearColor(0.725f, 0.725f, 0.725f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

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

    glDisable(GL_CULL_FACE);
    auto teapotModelMatrix_a_tmp = xform_a.matrix();
    auto teapotModelMatrix_a =
        reinterpret_cast<const linalg::aliases::float4x4 &>(
            teapotModelMatrix_a_tmp);
    draw_lit_mesh(litShader, teapotMesh, cam.position,
                  cam.get_viewproj_matrix((float)w / (float)h),
                  teapotModelMatrix_a);

    auto teapotModelMatrix_b_tmp = xform_b.matrix();
    auto teapotModelMatrix_b =
        reinterpret_cast<const linalg::aliases::float4x4 &>(
            teapotModelMatrix_b_tmp);
    draw_lit_mesh(litShader, teapotMesh, cam.position,
                  cam.get_viewproj_matrix((float)w / (float)h),
                  teapotModelMatrix_b);

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

    gl_check_error(__FILE__, __LINE__);

    EndDrawing();
  }
  return EXIT_SUCCESS;
}
