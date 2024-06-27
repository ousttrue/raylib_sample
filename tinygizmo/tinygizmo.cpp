#include "tinygizmo.h"
#include "tinygizmo_rotation.h"
#include "tinygizmo_scaling.h"
#include "tinygizmo_translation.h"

namespace tinygizmo {

void TranslationGizmo::draw(
    const Float4x4 &modelMatrix, const AddTriangleFunc &add_triangle,
    std::optional<GizmoComponentType> active_component) {
  position_draw(modelMatrix, add_triangle, active_component);
}

std::tuple<std::optional<TranslationGizmo::GizmoComponentType>, float>
TranslationGizmo::intersect(const Ray &local_ray) {
  return position_intersect(local_ray);
};

Transform TranslationGizmo::drag(GizmoComponentType active_component,
                                 const FrameState &frame, bool local_toggle,
                                 const Transform &src, const RayState &state) {
  if (auto position =
          position_drag(active_component, frame, local_toggle, src, state)) {
    auto dst = src;
    dst.position = *position;
    return dst;
  }
  return src;
}

void RotationGizmo::draw(const Float4x4 &modelMatrix,
                         const AddTriangleFunc &add_triangle,
                         std::optional<GizmoComponentType> active_component) {
  rotation_draw(modelMatrix, add_triangle, active_component);
}

std::tuple<std::optional<RotationGizmo::GizmoComponentType>, float>
RotationGizmo::intersect(const Ray &local_ray) {
  return rotation_intersect(local_ray);
};

Transform RotationGizmo::drag(GizmoComponentType active_component,
                              const FrameState &frame, bool local_toggle,
                              const Transform &src, const RayState &state) {
  if (auto rotation =
          rotation_drag(active_component, frame, local_toggle, src, state)) {
    auto dst = src;
    dst.orientation = *rotation;
    return dst;
  }

  return src;
}

void ScalingGizmo::draw(const Float4x4 &modelMatrix,
                        const AddTriangleFunc &add_triangle,
                        std::optional<GizmoComponentType> active_component) {
  scaling_draw(modelMatrix, add_triangle, active_component);
}

std::tuple<std::optional<ScalingGizmo::GizmoComponentType>, float>
ScalingGizmo::intersect(const Ray &local_ray) {
  return scaling_intersect(local_ray);
};

Transform ScalingGizmo::drag(GizmoComponentType active_component,
                             const FrameState &frame, bool local_toggle,
                             const Transform &src, bool uniform,
                             const RayState &state) {
  if (auto scale = scaling_drag(active_component, frame, local_toggle, src,
                                uniform, state)) {
    auto dst = src;
    dst.scale = *scale;
    return dst;
  }
  return src;
}

} // namespace tinygizmo
