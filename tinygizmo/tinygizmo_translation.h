#include "tinygizmo.h"
#include "tinygizmo_component.h"
#include <memory>

namespace tinygizmo {

std::tuple<std::shared_ptr<gizmo_component>, float>
position_intersect(const Ray &ray);

Float3 position_drag(DragState *drag, const FrameState &state,
                     bool local_toggle,
                     const std::shared_ptr<gizmo_component> &active,
                     const Transform &p);

void position_draw(const AddTriangleFunc &add_world_triangle,
                   const std::shared_ptr<gizmo_component> &active,
                   const Float4x4 &modelMatrix);

} // namespace tinygizmo
