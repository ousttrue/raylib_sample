#include "tinygizmo.h"

namespace tinygizmo {

void rotation_draw(const Float4x4 &modelMatrix,
                   const AddTriangleFunc &add_world_triangle,
                   GizmoComponentType active_component);

std::tuple<GizmoComponentType, float> rotation_intersect(const Ray &ray);

Quaternion rotation_drag(GizmoComponentType active_component,
                         const FrameState &state, bool local_toggle,
                         const Transform &p, DragState *drag);

} // namespace tinygizmo
