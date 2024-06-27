#include "tinygizmo.h"
#include <optional>

namespace tinygizmo {

void rotation_mesh(
    const Float4x4 &modelMatrix, const AddTriangleFunc &add_world_triangle,
    std::optional<RotationGizmo::GizmoComponentType> active_component);

std::tuple<std::optional<RotationGizmo::GizmoComponentType>, float>
rotation_intersect(const Ray &ray);

std::optional<Quaternion>
rotation_drag(RotationGizmo::GizmoComponentType active_component,
              const FrameState &frame, const RayState &drag,
              const Transform &src);

} // namespace tinygizmo
