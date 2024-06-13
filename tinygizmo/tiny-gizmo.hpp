// This is free and unencumbered software released into the public domain.
// For more information, please refer to <http://unlicense.org>
#pragma once
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
  float screenspace_scale = 0.f;
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
  gizmo_application_state active_state;
  gizmo_application_state last_state;

  // State to describe if the user has pressed the left mouse button during the
  // last frame
  bool has_clicked() const {
    return !last_state.mouse_left && active_state.mouse_left;
  }

  // State to describe if the user has released the left mouse button during the
  // last frame
  bool has_released() const {
    return last_state.mouse_left && !active_state.mouse_left;
  }

  AddTriangleFunc add_world_triangle;
};

struct interaction_state;
struct gizmo_context {
  std::unordered_map<uint32_t, std::shared_ptr<interaction_state>> gizmos;
  std::shared_ptr<interaction_state> get_or_create(uint32_t id);

  // Clear geometry buffer and update internal `gizmo_application_state` data
  gizmo_result translation_gizmo(const gizmo_state &state, bool local_toggle,
                                 uint32_t id, const float t[3],
                                 const float r[4]);
  gizmo_result rotationn_gizmo(const gizmo_state &state, bool local_toggle,
                               uint32_t id, const float t[3], const float r[4]);
  gizmo_result scale_gizmo(const gizmo_state &state, bool local_toggle,
                           bool uniform, uint32_t id, const float t[3],
                           const float r[4], const float s[3]);
};

} // namespace tinygizmo
