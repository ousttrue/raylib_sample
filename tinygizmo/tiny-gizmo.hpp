// This is free and unencumbered software released into the public domain.
// For more information, please refer to <http://unlicense.org>
#pragma once
#include <memory>
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

enum class transform_mode { translate, rotate, scale };

struct gizmo_application_state {
  bool mouse_left{false};
  bool hotkey_ctrl{false};
  float screenspace_scale{
      0.f}; // If > 0.f, the gizmos are drawn scale-invariant with a screenspace
            // value defined here
  float snap_translation{
      0.f};                 // World-scale units used for snapping translation
  float snap_scale{0.f};    // World-scale units used for snapping scale
  float snap_rotation{0.f}; // Radians used for snapping rotation quaternions
                            // (i.e. PI/8 or PI/16)
  float2 viewport_size;     // 3d viewport used to render the view
  float3 ray_origin;        // world-space ray origin (i.e. the camera position)
  float3 ray_direction;     // world-space ray direction
  float cam_yfov;
  float4 cam_orientation;
};

struct gizmo_context_impl;
struct gizmo_context {
  std::unique_ptr<gizmo_context_impl> impl;

  gizmo_context();
  ~gizmo_context();

  void update(const gizmo_application_state
                  &state); // Clear geometry buffer and update internal
                           // `gizmo_application_state` data
  std::tuple<std::span<draw_vertex>, std::span<uint32_t>> drawlist();

  bool position_gizmo(bool local_toggle, const std::string &name,
                      float *position, float *orientation, float *scale);
  bool orientation_gizmo(bool local_toggle, const std::string &name,
                         float *position, float *orientation, float *scale);
  bool scale_gizmo(bool local_toggle, const std::string &name, float *position,
                   float *orientation, float *scale);
};

} // namespace tinygizmo
