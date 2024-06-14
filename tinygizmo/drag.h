#pragma once
#include "minalg.hpp"
#include "tiny-gizmo.hpp"

namespace tinygizmo {

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

rigid_transform plane_translation_dragger(drag_state *drag,
                                          const gizmo_state &state,
                                          const minalg::float3 &plane_normal,
                                          const rigid_transform &t,
                                          bool uniform);

rigid_transform axis_translation_dragger(drag_state *drag,
                                         const gizmo_state &state,
                                         const minalg::float3 &axis,
                                         const rigid_transform &t,
                                         bool uniform);

rigid_transform axis_rotation_dragger(drag_state *drag,
                                      const gizmo_state &state,
                                      const minalg::float3 &axis,
                                      const rigid_transform &t, bool uniform);

rigid_transform axis_scale_dragger(drag_state *drag, const gizmo_state &state,
                                   const minalg::float3 &axis,
                                   const rigid_transform &t, bool uniform);

} // namespace tinygizmo
