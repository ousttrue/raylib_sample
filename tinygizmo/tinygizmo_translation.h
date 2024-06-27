#include "tinygizmo.h"
#include <optional>

namespace tinygizmo {

void position_mesh(
    const Float4x4 &modelMatrix, const AddTriangleFunc &add_world_triangle,
    std::optional<TranslationGizmo::GizmoComponentType> active_component);

std::tuple<std::optional<TranslationGizmo::GizmoComponentType>, float>
position_intersect(const Ray &ray);

std::optional<Float3>
position_drag(TranslationGizmo::GizmoComponentType active_component,
              const FrameState &frame, const RayState &drag,
              const Transform &src);

} // namespace tinygizmo
