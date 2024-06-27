#include "tinygizmo.h"
#include <optional>

namespace tinygizmo {

void position_draw(
    const Float4x4 &modelMatrix, const AddTriangleFunc &add_world_triangle,
    std::optional<TranslationGizmo::GizmoComponentType> active_component);

std::tuple<std::optional<TranslationGizmo::GizmoComponentType>, float>
position_intersect(const Ray &ray);

std::optional<Float3>
position_drag(TranslationGizmo::GizmoComponentType active_component,
              const FrameState &state, bool local_toggle, const Transform &p,
              const RayState &drag);

} // namespace tinygizmo
