#include "tinygizmo.h"
#include "tinygizmo_component.h"
#include <memory>

namespace tinygizmo {

std::tuple<std::shared_ptr<gizmo_component>, float>
rotation_intersect(const Ray &ray);

Quaternion rotation_drag(DragState *drag, const FrameState &state,
                     bool local_toggle,
                     const std::shared_ptr<gizmo_component> &active,
                     const Transform &p);

void rotation_draw(const AddTriangleFunc &add_world_triangle,
                   const std::shared_ptr<gizmo_component> &active,
                   const Float4x4 &modelMatrix);

} // namespace tinygizmo
