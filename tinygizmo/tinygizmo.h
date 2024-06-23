// This is free and unencumbered software released into the public domain.
// For more information, please refer to <http://unlicense.org>
#pragma once
#include "tinygizmo_alg.h"
#include <functional>
#include <string>

namespace tinygizmo {

inline Float3 snap(const Float3 &value, const float snap);

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
  Float2 viewport_size;
  Ray ray;
  float cam_yfov;
  Quaternion cam_orientation;

  // This will calculate a scale constant based on the number of screenspace
  // pixels passed as pixel_scale.
  float calc_scale_screenspace(const Float3 position) const {
    float dist = (position - this->ray.origin).length();
    return std::tan(this->cam_yfov) * dist *
           (_screenspace_scale / this->viewport_size.y);
  }
  float scale_screenspace(const Float3 &position) const {
    return (_screenspace_scale > 0.f) ? calc_scale_screenspace(position) : 1.f;
  }
};

inline Float3 transform_coord(const Float4x4 &m, const Float3 &coord) {
  auto r = m.transform(Float4::make(coord, 1));
  return (r.xyz() * (1.0 / r.w));
}

using AddTriangleFunc = std::function<void(const Float4 &rgba, const Float3 &p0,
                                           const Float3 &p1, const Float3 &p2)>;
inline void add_triangles(const AddTriangleFunc &add_triangle,
                          const Float4x4 &modelMatrix, const GeometryMesh &mesh,
                          const Float4 &color) {
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

inline Ray transform(const RigidTransform &p, const Ray &r) {
  return {p.transform_point(r.origin), p.transform_vector(r.direction)};
}

inline Ray detransform(const RigidTransform &p, const Ray &r) {
  return {p.detransform_point(r.origin), p.detransform_vector(r.direction)};
}

inline Ray scaling(const float scale, const Ray &r) {
  return {
      .origin = r.origin * scale,
      .direction = r.direction * scale,
  };
}

inline Ray descale(const float scale, const Ray &r) {
  return {
      .origin = r.origin * (1.0f / scale),
      .direction = r.direction * (1.0f / scale),
  };
}

inline std::tuple<float, RigidTransform, Ray>
gizmo_transform(const gizmo_application_state &active_state, bool local_toggle,
                const RigidTransform &src) {
  auto draw_scale = active_state.scale_screenspace(src.position);
  auto p = RigidTransform{
      .orientation = local_toggle ? src.orientation : Quaternion{0, 0, 0, 1},
      .position = src.position,
  };
  auto ray = detransform(p, active_state.ray);
  ray = descale(draw_scale, ray);
  return {draw_scale, p, ray};
};

struct drag_state {
  // Original position of an object being manipulated with a gizmo
  Float3 original_position;
  // Offset from position of grabbed object to coordinates of clicked point
  Float3 click_offset;
  // Original scale of an object being manipulated with a gizmo
  Float3 original_scale;
  // Original orientation of an object being manipulated with a gizmo
  Quaternion original_orientation;
};

} // namespace tinygizmo
