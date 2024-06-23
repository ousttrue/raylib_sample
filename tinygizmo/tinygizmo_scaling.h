#include "tinygizmo.h"
#include "tinygizmo_component.h"

namespace tinygizmo {

std::tuple<std::shared_ptr<gizmo_component>, float>
scaling_intersect(const Ray &ray);

Float3 scaling_drag(drag_state *drag, const gizmo_application_state &state,
                    bool local_toggle,
                    const std::shared_ptr<gizmo_component> &active,
                    const RigidTransform &p, bool uniform);

void scaling_draw(const AddTriangleFunc &add_world_triangle,
                  const std::shared_ptr<gizmo_component> &active,
                  const Float4x4 &modelMatrix);

} // namespace tinygizmo
