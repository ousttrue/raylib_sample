// This is free and unencumbered software released into the public domain.
// For more information, please refer to <http://unlicense.org>
#pragma once
#include "minalg.hpp"
#include "ray.h"
#include "rigid_transform.h"
#include <array>
#include <functional>
#include <string>

namespace tinygizmo {

inline minalg::float3 to_minalg(const std::array<float, 3> &v) {
  return {v[0], v[1], v[2]};
}
inline minalg::float4 to_minalg(const std::array<float, 4> &v) {
  return {v[0], v[1], v[2], v[3]};
}
inline minalg::float3 snap(const minalg::float3 &value, const float snap) {
  if (snap > 0.0f)
    return minalg::float3(floor(value / snap) * snap);
  return value;
}

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

inline minalg::float3 transform_coord(const minalg::float4x4 &transform,
                                      const minalg::float3 &coord) {
  auto r = mul(transform, minalg::float4(coord, 1));
  return (r.xyz() / r.w);
}

using AddTriangleFunc = std::function<void(
    const std::array<float, 4> &rgba, const std::array<float, 3> &p0,
    const std::array<float, 3> &p1, const std::array<float, 3> &p2)>;
inline void add_triangles(const AddTriangleFunc &add_triangle,
                          const minalg::float4x4 &modelMatrix,
                          const geometry_mesh &mesh,
                          const minalg::float4 &color) {
  for (auto &t : mesh.triangles) {
    auto v0 = mesh.vertices[t.x];
    auto v1 = mesh.vertices[t.y];
    auto v2 = mesh.vertices[t.z];
    auto p0 = transform_coord(
        modelMatrix,
        v0.position); // transform local coordinates into worldspace
    auto p1 = transform_coord(
        modelMatrix,
        v1.position); // transform local coordinates into worldspace
    auto p2 = transform_coord(
        modelMatrix,
        v2.position); // transform local coordinates into worldspace
    add_triangle({color.x, color.y, color.z, color.w}, {p0.x, p0.y, p0.z},
                 {p1.x, p1.y, p1.z}, {p2.x, p2.y, p2.z});
  }
}

inline ray transform(const minalg::rigid_transform &p, const ray &r) {
  return {p.transform_point(r.origin), p.transform_vector(r.direction)};
}

inline ray detransform(const minalg::rigid_transform &p, const ray &r) {
  return {p.detransform_point(r.origin), p.detransform_vector(r.direction)};
}

inline ray scaling(const float scale, const ray &r) {
  return {
      .origin = r.origin * scale,
      .direction = r.direction * scale,
  };
}

inline ray descale(const float scale, const ray &r) {
  return {
      .origin = r.origin / scale,
      .direction = r.direction / scale,
  };
}

inline std::tuple<float, minalg::rigid_transform, ray>
gizmo_transform(const gizmo_application_state &active_state, bool local_toggle,
                const minalg::rigid_transform &src) {
  auto draw_scale = active_state.scale_screenspace(src.position);
  auto p = minalg::rigid_transform(local_toggle ? src.orientation
                                                : minalg::float4(0, 0, 0, 1),
                                   src.position);
  ray ray{
      .origin = *(minalg::float3 *)&active_state.ray_origin[0],
      .direction = *(minalg::float3 *)&active_state.ray_direction[0],
  };
  ray = detransform(p, ray);
  ray = descale(draw_scale, ray);
  return {draw_scale, p, ray};
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

} // namespace tinygizmo
