#include "tinygizmo.h"
#include "tinygizmo_rotation.h"
#include "tinygizmo_scaling.h"
#include "tinygizmo_translation.h"

namespace tinygizmo {

void TranslationGizmo::draw(const Float4x4 &modelMatrix,
                            const AddTriangleFunc &add_triangle,
                            GizmoComponentType active_component) {
  position_draw(modelMatrix, add_triangle, active_component);
}

std::tuple<GizmoComponentType, float>
TranslationGizmo::intersect(const Ray &local_ray) {
  return position_intersect(local_ray);
};

Transform TranslationGizmo::drag(GizmoComponentType active_component,
                                 const FrameState &frame, bool local_toggle,
                                 const Transform &src, DragState *state) {
  auto dst = src;
  dst.position =
      position_drag(active_component, frame, local_toggle, src, state);
  return dst;
}

void RotationGizmo::draw(const Float4x4 &modelMatrix,
                         const AddTriangleFunc &add_triangle,
                         GizmoComponentType active_component) {
  rotation_draw(modelMatrix, add_triangle, active_component);
}

std::tuple<GizmoComponentType, float>
RotationGizmo::intersect(const Ray &local_ray) {
  return rotation_intersect(local_ray);
};

Transform RotationGizmo::drag(GizmoComponentType active_component,
                              const FrameState &frame, bool local_toggle,
                              const Transform &src, DragState *state) {
  auto dst = src;
  dst.orientation =
      rotation_drag(active_component, frame, local_toggle, src, state);
  return dst;
}

void ScalingGizmo::draw(const Float4x4 &modelMatrix,
                        const AddTriangleFunc &add_triangle,
                        GizmoComponentType active_component) {
  scaling_draw(modelMatrix, add_triangle, active_component);
}

std::tuple<GizmoComponentType, float>
ScalingGizmo::intersect(const Ray &local_ray) {
  return scaling_intersect(local_ray);
};

Transform ScalingGizmo::drag(GizmoComponentType active_component,
                             const FrameState &frame, bool local_toggle,
                             const Transform &src, bool uniform,
                             DragState *state) {
  auto dst = src;
  dst.scale =
      scaling_drag(active_component, frame, local_toggle, src, uniform, state);
  return dst;
}

} // namespace tinygizmo
