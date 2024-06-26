#include "tinygizmo.h"

namespace tinygizmo {

void scaling_draw(const Float4x4 &modelMatrix,
                  const AddTriangleFunc &add_world_triangle,
                  GizmoComponentType active_component);

std::tuple<GizmoComponentType, float> scaling_intersect(const Ray &ray);

Float3 scaling_drag(GizmoComponentType active_component,
                    const FrameState &state, bool local_toggle,
                    const Transform &p, bool uniform, DragState *drag);

} // namespace tinygizmo
