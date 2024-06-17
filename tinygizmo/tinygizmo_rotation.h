#include "tinygizmo.h"

namespace tinygizmo {

std::tuple<std::shared_ptr<gizmo_mesh_component>, float>
rotation_intersect(const ray &ray);

minalg::float4
rotation_drag(drag_state *drag, const gizmo_application_state &state,
              bool local_toggle,
              const std::shared_ptr<gizmo_mesh_component> &active,
              const rigid_transform &p);

void rotation_draw(const AddTriangleFunc &add_world_triangle,
                   const std::shared_ptr<gizmo_mesh_component> &active,
                   const minalg::float4x4 &modelMatrix);

} // namespace tinygizmo
