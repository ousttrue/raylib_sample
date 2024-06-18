#pragma once
#include "geometry_mesh.h"

namespace tinygizmo {

struct gizmo_component {
  geometry_mesh mesh;
  minalg::float4 base_color;
  minalg::float4 highlight_color;

  gizmo_component(const geometry_mesh &mesh, const minalg::float4 &base,
                  const minalg::float4 &high)
      : mesh(mesh), base_color(base), highlight_color(high) {}

  virtual ~gizmo_component() {}
};

} // namespace tinygizmo
