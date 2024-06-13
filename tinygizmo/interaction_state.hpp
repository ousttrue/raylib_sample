#pragma once
#include "minalg.hpp"
#include "tiny-gizmo.hpp"
#include <assert.h>
#include <map>

namespace tinygizmo {

enum class interact {
  none,
  translate_x,
  translate_y,
  translate_z,
  translate_yz,
  translate_zx,
  translate_xy,
  translate_xyz,
  rotate_x,
  rotate_y,
  rotate_z,
  scale_x,
  scale_y,
  scale_z,
  scale_xyz,
};

struct interaction_state {
  // Flag to indicate if the gizmo is being actively manipulated
  bool active = false;
  // Flag to indicate if the gizmo is being hovered
  bool hover = false;
  // Original position of an object being manipulated with a gizmo
  minalg::float3 original_position;
  // Original orientation of an object being manipulated with a gizmo
  minalg::float4 original_orientation;
  // Original scale of an object being manipulated with a gizmo
  minalg::float3 original_scale;
  // Offset from position of grabbed object to coordinates of clicked point
  minalg::float3 click_offset;
  // Currently active component
  interact interaction_mode;

  std::map<interact, gizmo_mesh_component> mesh_components;

public:
  interaction_state();

private:
  // The only purpose of this is readability: to reduce the total column width
  // of the intersect(...) statements in every gizmo
  bool intersect(const ray &r, interact i, float &t, const float best_t);

  // This will calculate a scale constant based on the number of screenspace
  // pixels passed as pixel_scale.
  float scale_screenspace(const gizmo_state &state,
                          const minalg::float3 position,
                          const float pixel_scale);

  void plane_translation_dragger(const gizmo_state &state,
                                 const minalg::float3 &plane_normal,
                                 minalg::float3 &point);

  void axis_translation_dragger(const gizmo_state &state,
                                const minalg::float3 &axis,
                                minalg::float3 &point);

  void axis_rotation_dragger(const gizmo_state &state,
                             const minalg::float3 &axis,
                             const minalg::float3 &center,
                             const minalg::float4 &start_orientation,
                             minalg::float4 &orientation);

  void axis_scale_dragger(const gizmo_state &state, const minalg::float3 &axis,
                          const minalg::float3 &center, minalg::float3 &scale,
                          const bool uniform);

public:
  gizmo_result position_gizmo(const gizmo_state &state, bool local_toggle,
                              const minalg::float4 &rotation,
                              const minalg::float3 &_position);

  gizmo_result rotation_gizmo(const gizmo_state &state, bool local_toggle,
                              const minalg::float3 &center,
                              const minalg::float4 &_orientation);

  gizmo_result scale_gizmo(const gizmo_state &state, bool local_toggle,
                           const minalg::float4 &orientation,
                           const minalg::float3 &center,
                           const minalg::float3 &_scale, bool uniform);
};

} // namespace tinygizmo
