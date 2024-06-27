#include "tinygizmo.h"
#include <optional>

namespace tinygizmo {

void rotation_draw(
    const Float4x4 &modelMatrix, const AddTriangleFunc &add_world_triangle,
    std::optional<RotationGizmo::GizmoComponentType> active_component);

std::tuple<std::optional<RotationGizmo::GizmoComponentType>, float>
rotation_intersect(const Ray &ray);

std::optional<Quaternion>
rotation_drag(RotationGizmo::GizmoComponentType active_component,
              const FrameState &state, bool local_toggle, const Transform &p,
              const RayState &drag);

} // namespace tinygizmo
