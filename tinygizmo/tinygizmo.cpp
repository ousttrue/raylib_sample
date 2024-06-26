#include "tinygizmo.h"
#include "tinygizmo_rotation.h"
#include "tinygizmo_scaling.h"
#include "tinygizmo_translation.h"

namespace tinygizmo {

void TranslationGizmo::draw(const std::shared_ptr<GizmoComponent> &component,
                            const AddTriangleFunc &add_triangle,
                            const Float4x4 &modelMatrix) {
  position_draw(add_triangle, component, modelMatrix);
}

std::tuple<std::shared_ptr<GizmoComponent>, float>
TranslationGizmo::intersect(const Ray &local_ray) {
  return position_intersect(local_ray);
};

Transform TranslationGizmo::drag(
    DragState *state, const std::shared_ptr<GizmoComponent> &component,
    const FrameState &frame, bool local_toggle, const Transform &src) {
  auto dst = src;
  dst.position = position_drag(state, frame, local_toggle, component, src);
  return dst;
}

void RotationGizmo::draw(const std::shared_ptr<GizmoComponent> &component,
                         const AddTriangleFunc &add_triangle,
                         const Float4x4 &modelMatrix) {
  rotation_draw(add_triangle, component, modelMatrix);
}

std::tuple<std::shared_ptr<GizmoComponent>, float>
RotationGizmo::intersect(const Ray &local_ray) {
  return rotation_intersect(local_ray);
};

Transform RotationGizmo::drag(DragState *state,
                              const std::shared_ptr<GizmoComponent> &component,
                              const FrameState &frame, bool local_toggle,
                              const Transform &src) {
  auto dst = src;
  dst.orientation = rotation_drag(state, frame, local_toggle, component, src);
  return dst;
}

void ScalingGizmo::draw(const std::shared_ptr<GizmoComponent> &component,
                        const AddTriangleFunc &add_triangle,
                        const Float4x4 &modelMatrix) {
  scaling_draw(add_triangle, component, modelMatrix);
}

std::tuple<std::shared_ptr<GizmoComponent>, float>
ScalingGizmo::intersect(const Ray &local_ray) {
  return scaling_intersect(local_ray);
};

Transform ScalingGizmo::drag(DragState *state,
                             const std::shared_ptr<GizmoComponent> &component,
                             const FrameState &frame, bool local_toggle,
                             const Transform &src,
                             bool uniform) {
  auto dst = src;
  dst.scale = scaling_drag(state, frame, local_toggle, component, src, uniform);
  return dst;
}

} // namespace tinygizmo
