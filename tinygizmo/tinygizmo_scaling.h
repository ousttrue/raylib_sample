#include "tinygizmo.h"
#include <optional>

namespace tinygizmo {

void scaling_draw(
    const Float4x4 &modelMatrix, const AddTriangleFunc &add_world_triangle,
    std::optional<ScalingGizmo::GizmoComponentType> active_component);

std::tuple<std::optional<ScalingGizmo::GizmoComponentType>, float>
scaling_intersect(const Ray &ray);

std::optional<Float3>
scaling_drag(ScalingGizmo::GizmoComponentType active_component,
             const FrameState &state, bool local_toggle, const Transform &p,
             bool uniform, const RayState &drag);

} // namespace tinygizmo
