// This is free and unencumbered software released into the public domain.
// For more information, please refer to <http://unlicense.org>
#pragma once
#include "minalg.hpp"
#include <array>
#include <functional>
#include <memory>
#include <string>
#include <unordered_map>

namespace tinygizmo {

// 32 bit Fowler-Noll-Vo Hash
inline uint32_t hash_fnv1a(std::string_view str) {
  static const uint32_t fnv1aBase32 = 0x811C9DC5u;
  static const uint32_t fnv1aPrime32 = 0x01000193u;

  uint32_t result = fnv1aBase32;

  for (auto c : str) {
    result ^= static_cast<uint32_t>(c);
    result *= fnv1aPrime32;
  }
  return result;
}

struct gizmo_application_state {
  bool mouse_left = false;
  // If > 0.f, the gizmos are drawn scale-invariant with a screenspace value
  // defined here
  float _screenspace_scale = 0.f;
  // World-scale units used for snapping translation
  float snap_translation = 0.f;
  // World-scale units used for snapping scale
  float snap_scale = 0.f;
  // Radians used for snapping rotation quaternions (i.e. PI/8 or PI/16)
  float snap_rotation = 0.f;
  // 3d viewport used to render the view
  std::array<float, 2> viewport_size;
  // world-space ray origin (i.e. the camera position)
  std::array<float, 3> ray_origin;
  // world-space ray direction
  std::array<float, 3> ray_direction;
  float cam_yfov;
  std::array<float, 4> cam_orientation;

  // This will calculate a scale constant based on the number of screenspace
  // pixels passed as pixel_scale.
  float calc_scale_screenspace(const minalg::float3 position) const {
    float dist = length(position - to_minalg(this->ray_origin));
    return std::tan(this->cam_yfov) * dist *
           (_screenspace_scale / this->viewport_size[1]);
  }
  float scale_screenspace(const minalg::float3 &position) const {
    return (_screenspace_scale > 0.f) ? calc_scale_screenspace(position) : 1.f;
  }
};

struct gizmo_result {
  bool hover;
  bool active;
  std::array<float, 3> t;
  std::array<float, 4> r;
  std::array<float, 3> s;
};

using AddTriangleFunc = std::function<void(
    const std::array<float, 4> &rgba, const std::array<float, 3> &p0,
    const std::array<float, 3> &p1, const std::array<float, 3> &p2)>;

struct gizmo_state {
  bool local_toggle;
  gizmo_application_state active_state;
  // State to describe if the user has pressed the left mouse button during the
  // last frame
  bool has_clicked;
  // State to describe if the user has released the left mouse button during the
  // last frame
  bool has_released;

  gizmo_state(bool local_toggle, const gizmo_application_state &active_state,
              const gizmo_application_state &last_state)
      : local_toggle(local_toggle), active_state(active_state),
        has_clicked(!last_state.mouse_left && active_state.mouse_left),
        has_released(last_state.mouse_left && !active_state.mouse_left) {}

  std::tuple<float, rigid_transform, ray>
  gizmo_transform(const rigid_transform &src) const {
    auto draw_scale = active_state.scale_screenspace(src.position);
    auto p = rigid_transform(local_toggle ? src.orientation
                                          : minalg::float4(0, 0, 0, 1),
                             src.position);
    ray ray{
        .origin = *(minalg::float3 *)&active_state.ray_origin[0],
        .direction = *(minalg::float3 *)&active_state.ray_direction[0],
    };
    ray = detransform(p, ray);
    ray = ray.descale(draw_scale);
    return {draw_scale, p, ray};
  }
};

struct gizmo_mesh_component {
  geometry_mesh mesh;
  minalg::float4 base_color;
  minalg::float4 highlight_color;

  gizmo_mesh_component(const geometry_mesh &mesh, const minalg::float4 &base,
                       const minalg::float4 &high)
      : mesh(mesh), base_color(base), highlight_color(high) {}
};

struct drag_state {
  // Original position of an object being manipulated with a gizmo
  minalg::float3 original_position;
  // Offset from position of grabbed object to coordinates of clicked point
  minalg::float3 click_offset;
  // Original scale of an object being manipulated with a gizmo
  minalg::float3 original_scale;
  // Original orientation of an object being manipulated with a gizmo
  minalg::float4 original_orientation;
};

// struct interaction_state {
//   // Currently active component
//   std::shared_ptr<gizmo_mesh_component> active;
//
//   drag_state drag = {};
//
//   gizmo_result position_gizmo(const gizmo_state &state,
//                               const AddTriangleFunc &add_triangle,
//                               const rigid_transform &src);
//
//   gizmo_result rotation_gizmo(const gizmo_state &state,
//                               const AddTriangleFunc &add_triangle,
//                               const rigid_transform &src);
//
//   gizmo_result scale_gizmo(const gizmo_state &state,
//                            const AddTriangleFunc &add_triangle,
//                            const rigid_transform &src, bool uniform);
// };

// struct gizmo_context {
//   std::unordered_map<uint32_t, std::shared_ptr<interaction_state>> gizmos;
//   std::shared_ptr<interaction_state> get_or_create(uint32_t id);
// };

std::tuple<std::shared_ptr<gizmo_mesh_component>, float>
position_intersect(const ray &ray);

minalg::float3
position_drag(drag_state *drag, const gizmo_state &state,
              const std::shared_ptr<gizmo_mesh_component> &active,
              const rigid_transform &p);

void position_draw(const AddTriangleFunc &add_world_triangle,
                   const std::shared_ptr<gizmo_mesh_component> &active,
                   const minalg::float4x4 &modelMatrix);

} // namespace tinygizmo
