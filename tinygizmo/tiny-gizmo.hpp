// This is free and unencumbered software released into the public domain.
// For more information, please refer to <http://unlicense.org>
#pragma once
#include <memory>
#include <span>
#include <string>
#include <unordered_map>
#include <vector>

#include "minalg.hpp"

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

struct float2 {
  float x;
  float y;
};
struct float3 {
  float x;
  float y;
  float z;
};
struct float4 {
  float x;
  float y;
  float z;
  float w;
};
struct uint3 {
  unsigned int x;
  unsigned int y;
  unsigned int z;
};
struct draw_vertex {
  float3 position;
  float3 normal;
  float4 color;
};

struct gizmo_application_state {
  bool mouse_left{false};
  // If > 0.f, the gizmos are drawn scale-invariant with a screenspace value
  // defined here
  float screenspace_scale{0.f};
  // World-scale units used for snapping translation
  float snap_translation{0.f};
  // World-scale units used for snapping scale
  float snap_scale{0.f};
  // Radians used for snapping rotation quaternions (i.e. PI/8 or PI/16)
  float snap_rotation{0.f};
  // 3d viewport used to render the view
  float2 viewport_size;
  // world-space ray origin (i.e. the camera position)
  float3 ray_origin;
  // world-space ray direction
  float3 ray_direction;
  float cam_yfov;
  float4 cam_orientation;
};

struct gizmo_result {
  bool hover;
  bool active;
  float3 t;
  float4 r;
  float3 s;
};

struct gizmo_state {
  gizmo_application_state active_state;
  gizmo_application_state last_state;

  // State to describe if the user has pressed the left mouse button during the
  // last frame
  bool has_clicked = false;
  // State to describe if the user has released the left mouse button during the
  // last frame
  bool has_released = false;

  mutable std::vector<gizmo_renderable> drawlist;
};

struct interaction_state;
struct gizmo_context {
  gizmo_state state;

  std::vector<draw_vertex> vertices;
  std::vector<uint32_t> indices;

  std::unordered_map<uint32_t, std::shared_ptr<interaction_state>> gizmos;
  std::shared_ptr<interaction_state> get_or_create(uint32_t id);

  // Clear geometry buffer and update internal `gizmo_application_state` data
  void begin_frame(const gizmo_application_state &state);
  gizmo_result translation_gizmo(bool local_toggle, uint32_t id,
                                 const float t[3], const float r[4]);
  gizmo_result rotationn_gizmo(bool local_toggle, uint32_t id, const float t[3],
                               const float r[4]);
  gizmo_result scale_gizmo(bool local_toggle, bool uniform, uint32_t id,
                           const float t[3], const float r[4],
                           const float s[3]);
  std::tuple<std::span<draw_vertex>, std::span<uint32_t>> end_frame();
};

} // namespace tinygizmo
