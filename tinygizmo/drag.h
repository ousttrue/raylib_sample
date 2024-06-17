#pragma once
#include "minalg.hpp"
#include "tiny-gizmo.h"

namespace tinygizmo {

rigid_transform
plane_translation_dragger(drag_state *drag,
                          const gizmo_application_state &active_state,
                          bool local_toggle, const minalg::float3 &plane_normal,
                          const rigid_transform &t, bool uniform);

rigid_transform
axis_translation_dragger(drag_state *drag,
                         const gizmo_application_state &active_state,
                         bool local_toggle, const minalg::float3 &axis,
                         const rigid_transform &t, bool uniform);

rigid_transform
axis_rotation_dragger(drag_state *drag,
                      const gizmo_application_state &active_state,
                      bool local_toggle, const minalg::float3 &axis,
                      const rigid_transform &t, bool uniform);

rigid_transform axis_scale_dragger(drag_state *drag,
                                   const gizmo_application_state &active_state,
                                   bool local_toggle,
                                   const minalg::float3 &axis,
                                   const rigid_transform &t, bool uniform);

} // namespace tinygizmo
