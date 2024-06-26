#include "tinygizmo.h"

namespace tinygizmo {

void position_draw(const Float4x4 &modelMatrix,
                   const AddTriangleFunc &add_world_triangle,
                   GizmoComponentType active_component);

std::tuple<GizmoComponentType, float> position_intersect(const Ray &ray);

Float3 position_drag(GizmoComponentType active_component,
                     const FrameState &state, bool local_toggle,
                     const Transform &p, DragState *drag);

} // namespace tinygizmo
