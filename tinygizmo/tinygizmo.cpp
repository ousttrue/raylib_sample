#include "tinygizmo.h"
#include "tinygizmo_rotation.h"
#include "tinygizmo_scaling.h"
#include "tinygizmo_translation.h"

namespace tinygizmo {

void TranslationGizmo::mesh(
    const Float4x4 &modelMatrix, const AddTriangleFunc &add_triangle,
    std::optional<GizmoComponentType> active_component) {
  position_mesh(modelMatrix, add_triangle, active_component);
}

std::optional<std::tuple<RayState, TranslationGizmo::GizmoComponentType>>
TranslationGizmo::intersect(const FrameState &frame, bool local_toggle,
                            const Transform &p) {
  auto [draw_scale, gizmo_transform, local_ray] =
      frame.gizmo_transform_and_local_ray(local_toggle, p);

  auto [active_component, t] = position_intersect(local_ray);
  if (!active_component) {
    return {};
  }

  RayState state{
      .local_toggle = local_toggle,
      .uniform = false,
      .transform = p,
      .draw_scale = draw_scale,
      .gizmo_transform = gizmo_transform,
      .local_ray = local_ray,
      .t = t,
  };
  return std::make_pair(state, *active_component);
}

Transform TranslationGizmo::drag(GizmoComponentType active_component,
                                 const FrameState &frame, const RayState &drag,
                                 const Transform &src) {
  if (auto position = position_drag(active_component, frame, drag, src)) {
    auto dst = src;
    dst.position = *position;
    return dst;
  }
  return src;
}

void RotationGizmo::mesh(const Float4x4 &modelMatrix,
                         const AddTriangleFunc &add_triangle,
                         std::optional<GizmoComponentType> active_component) {
  rotation_mesh(modelMatrix, add_triangle, active_component);
}

std::optional<std::tuple<RayState, RotationGizmo::GizmoComponentType>>
RotationGizmo::intersect(const FrameState &frame, bool local_toggle,
                         const Transform &p) {
  auto [draw_scale, gizmo_transform, local_ray] =
      frame.gizmo_transform_and_local_ray(local_toggle, p);

  auto [active_component, t] = rotation_intersect(local_ray);
  if (!active_component) {
    return {};
  }

  RayState state{
      .local_toggle = local_toggle,
      .uniform = false,
      .transform = p,
      .draw_scale = draw_scale,
      .gizmo_transform = gizmo_transform,
      .local_ray = local_ray,
      .t = t,
  };
  return std::make_pair(state, *active_component);
}

Transform RotationGizmo::drag(GizmoComponentType active_component,
                              const FrameState &frame, const RayState &drag,
                              const Transform &src) {
  if (auto rotation = rotation_drag(active_component, frame, drag, src)) {
    auto dst = src;
    dst.orientation = *rotation;
    return dst;
  }

  return src;
}

void ScalingGizmo::mesh(const Float4x4 &modelMatrix,
                        const AddTriangleFunc &add_triangle,
                        std::optional<GizmoComponentType> active_component) {
  scaling_mesh(modelMatrix, add_triangle, active_component);
}

std::optional<std::tuple<RayState, ScalingGizmo::GizmoComponentType>>
ScalingGizmo::intersect(const FrameState &frame, bool local_toggle,
                        const Transform &p, bool uniform) {
  auto [draw_scale, gizmo_transform, local_ray] =
      frame.gizmo_transform_and_local_ray(local_toggle, p);

  auto [active_component, t] = scaling_intersect(local_ray);
  if (!active_component) {
    return {};
  }

  RayState state{
      .local_toggle = local_toggle,
      .uniform = uniform,
      .transform = p,
      .draw_scale = draw_scale,
      .gizmo_transform = gizmo_transform,
      .local_ray = local_ray,
      .t = t,
  };
  return std::make_pair(state, *active_component);
}

Transform ScalingGizmo::drag(GizmoComponentType active_component,
                             const FrameState &frame, const RayState &drag,
                             const Transform &src) {
  if (auto scale = scaling_drag(active_component, frame, drag, src)) {
    auto dst = src;
    dst.scale = *scale;
    return dst;
  }
  return src;
}

} // namespace tinygizmo
