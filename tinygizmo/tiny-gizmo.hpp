// This is free and unencumbered software released into the public domain.
// For more information, please refer to <http://unlicense.org>
#pragma once
#include <span>
#include <string>

namespace tinygizmo {

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

class gizmo_context_impl;
struct gizmo_context {
  gizmo_context_impl *impl;
  gizmo_context();
  ~gizmo_context();
  // Clear geometry buffer and update internal `gizmo_application_state` data
  void begin_frame(const gizmo_application_state &state);
  gizmo_result translation_gizmo(bool local_toggle, const std::string &name,
                                 const float t[3], const float r[4]);
  gizmo_result rotationn_gizmo(bool local_toggle, const std::string &name,
                               const float t[3], const float r[4]);
  gizmo_result scale_gizmo(bool local_toggle, bool uniform,
                           const std::string &name, const float t[3],
                           const float r[4], const float s[3]);
  std::tuple<std::span<draw_vertex>, std::span<uint32_t>> end_frame();
};

} // namespace tinygizmo
