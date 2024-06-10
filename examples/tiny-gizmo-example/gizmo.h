#pragma once
#include "tiny-gizmo.hpp"
#include <linalg.h>
#include <string_view>

#include "drawable.h"

// tinygizmo::rigid_transform xform_a;
// xform_a.position = {-2, 0, 0};
// tinygizmo::rigid_transform xform_b;
// xform_b.position = {+2, 0, 0};

// struct ray {
//   linalg::aliases::float3 origin;
//   linalg::aliases::float3 direction;
// };
//
// struct rect {
//   int x0, y0, x1, y1;
//   int width() const { return x1 - x0; }
//   int height() const { return y1 - y0; }
//   linalg::aliases::int2 dims() const { return {width(), height()}; }
//   float aspect_ratio() const { return (float)width() / height(); }
// };

// // Returns a world-space ray through the given pixel, originating at the
// camera ray get_ray_from_pixel(const linalg::aliases::float2 &pixel,
//                        const rect &viewport, const camera &cam) {
//   const float x = 2 * (pixel.x - viewport.x0) / viewport.width() - 1,
//               y = 1 - 2 * (pixel.y - viewport.y0) / viewport.height();
//   const linalg::aliases::float4x4 inv_view_proj =
//       inverse(cam.get_viewproj_matrix(viewport.aspect_ratio()));
//   const linalg::aliases::float4 p0 = mul(inv_view_proj,
//                                          linalg::aliases::float4(x, y, -1,
//                                          1)),
//                                 p1 = mul(inv_view_proj,
//                                          linalg::aliases::float4(x, y, +1,
//                                          1));
//   return {cam.position, p1.xyz() * p0.w - p0.xyz() * p1.w};
// }

struct Gizmo {
  tinygizmo::gizmo_application_state gizmo_state;
  tinygizmo::gizmo_context gizmo_ctx;
  Drawable gizmo;

  Gizmo() {
    gizmo_ctx.render = [this](const tinygizmo::geometry_mesh &r) {
      // upload_mesh(r, gizmoEditorMesh);
      if(r.vertices.empty()){
        return;
      }
      this->gizmo.load<tinygizmo::geometry_vertex, uint32_t>(
          r.vertices, {&r.triangles[0].x, r.triangles.size() * 3}, true);
      gizmo.draw(MatrixIdentity());
    };
  }

  // auto gizmo = Drawable{};

  // minalg::float2 lastCursor;

  // tinygizmo::rigid_transform xform_a_last;

  void new_frame() {
    // auto position = GetMousePosition();

    // auto cameraOrientation = cam.rotation();
    // gizmo_state.mouse_left = IsMouseButtonDown(MOUSE_BUTTON_LEFT);
    //
    // // keyboard
    // gizmo_state.hotkey_ctrl = IsKeyDown(KEY_LEFT_CONTROL);
    // gizmo_state.hotkey_local = IsKeyDown(KEY_L);
    // gizmo_state.hotkey_translate = IsKeyDown(KEY_T);
    // gizmo_state.hotkey_rotate = IsKeyDown(KEY_R);
    // gizmo_state.hotkey_scale = IsKeyDown(KEY_S);
    //
    // const auto rayDir =
    //     get_ray_from_pixel({lastCursor.x, lastCursor.y}, {0, 0, w, h}, cam)
    //         .direction;
    // // Gizmo input interaction state populated via win->on_input(...)
    // callback
    // // above. Update app parameters:
    // gizmo_state.viewport_size =
    //     minalg::float2(static_cast<float>(w), static_cast<float>(h));
    // gizmo_state.cam.near_clip = cam.near_clip;
    // gizmo_state.cam.far_clip = cam.far_clip;
    // gizmo_state.cam.yfov = cam.yfov;
    // gizmo_state.cam.position =
    //     minalg::float3(cam.position.x, cam.position.y, cam.position.z);
    // gizmo_state.cam.orientation =
    //     minalg::float4(cameraOrientation.x, cameraOrientation.y,
    //                    cameraOrientation.z, cameraOrientation.w);
    // gizmo_state.ray_origin =
    //     minalg::float3(cam.position.x, cam.position.y, cam.position.z);
    // gizmo_state.ray_direction = minalg::float3(rayDir.x, rayDir.y, rayDir.z);
    // // gizmo_state.screenspace_scale = 80.f; // optional flag to draw the
    // gizmos
    // // at a constant screen-space scale
    //
    // gizmo_ctx.update(gizmo_state);
  }

  tinygizmo::rigid_transform
  transform(const std::string_view name,
            const tinygizmo::rigid_transform &transform) {
    return transform;
  }

  void draw() {
    glClear(GL_DEPTH_BUFFER_BIT);
    gizmo_ctx.draw();
  }
};
