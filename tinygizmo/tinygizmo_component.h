#pragma once
#include "geometry_mesh.h"
#include "tinygizmo.h"
#include <optional>

namespace tinygizmo {

struct gizmo_component {
  geometry_mesh mesh;
  minalg::float4 base_color;
  minalg::float4 highlight_color;

  gizmo_component(const geometry_mesh &mesh, const minalg::float4 &base,
                  const minalg::float4 &high)
      : mesh(mesh), base_color(base), highlight_color(high) {}

  virtual ~gizmo_component() {}

  virtual std::optional<minalg::rigid_transform>
  drag(drag_state *drag, const gizmo_application_state &active_state,
       bool local_toggle, const minalg::rigid_transform &src) const {
    return {};
  }
};

} // namespace tinygizmo
