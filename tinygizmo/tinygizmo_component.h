#pragma once

#include "tinygizmo.h"

namespace tinygizmo {

struct gizmo_component {
  GeometryMesh mesh;
  Float4 base_color;
  Float4 highlight_color;

  gizmo_component(const GeometryMesh &mesh, const Float4 &base,
                  const Float4 &high)
      : mesh(mesh), base_color(base), highlight_color(high) {}

  virtual ~gizmo_component() {}

  virtual std::optional<RigidTransform>
  drag(DragState *drag, const FrameState &active_state,
       bool local_toggle, const RigidTransform &src) const {
    return {};
  }
};

} // namespace tinygizmo
