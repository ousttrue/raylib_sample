#pragma once
#include "drag.h"
#include "minalg.hpp"
#include "tiny-gizmo.hpp"
#include <assert.h>
#include <memory>

namespace tinygizmo {

struct gizmo_mesh_component {
  geometry_mesh mesh;
  minalg::float4 base_color;
  minalg::float4 highlight_color;

  gizmo_mesh_component(const geometry_mesh &mesh, const minalg::float4 &base,
                       const minalg::float4 &high)
      : mesh(mesh), base_color(base), highlight_color(high) {}
};

struct interaction_state {
  // Currently active component
  std::shared_ptr<gizmo_mesh_component> active;

  drag_state drag = {};

  gizmo_result position_gizmo(const gizmo_state &state,
                              const AddTriangleFunc &add_triangle,
                              const minalg::float4 &rotation,
                              const minalg::float3 &_position);

  gizmo_result rotation_gizmo(const gizmo_state &state,
                              const AddTriangleFunc &add_triangle,
                              const minalg::float3 &center,
                              const minalg::float4 &_orientation);

  gizmo_result scale_gizmo(const gizmo_state &state,
                           const AddTriangleFunc &add_triangle,
                           const minalg::float4 &orientation,
                           const minalg::float3 &center,
                           const minalg::float3 &_scale, bool uniform);
};

} // namespace tinygizmo
