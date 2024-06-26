// This is free and unencumbered software released into the public domain.
// For more information, please refer to <http://unlicense.org>
#pragma once
#include "tinygizmo_alg.h"
#include <memory>

namespace tinygizmo {

enum class GizmoComponentType {
  None,
  TranslationX,
  TranslationY,
  TranslationZ,
  TranslationXY,
  TranslationYZ,
  TranslationZX,
  TranslationView,
  RotationX,
  RotationY,
  RotationZ,
  ScalingX,
  ScalingY,
  ScalingZ,
};

struct FrameState {
  bool mouse_down = false;
  // If > 0.f, the gizmos are drawn scale-invariant with a screenspace value
  // defined here
  float _screenspace_scale = 0.f;
  Float2 viewport_size;
  Ray ray;
  float cam_yfov;
  Quaternion cam_orientation;

  // This will calculate a scale constant based on the number of screenspace
  // pixels passed as pixel_scale.
  float calc_scale_screenspace(const Float3 position) const {
    float dist = (position - this->ray.origin).length();
    return std::tan(this->cam_yfov) * dist *
           (_screenspace_scale / this->viewport_size.y);
  }

  float scale_screenspace(const Float3 &position) const {
    return (_screenspace_scale > 0.f) ? calc_scale_screenspace(position) : 1.f;
  }

  std::tuple<float, Transform, Ray>
  gizmo_transform_and_local_ray(bool local_toggle, const Transform &src) const {
    auto draw_scale = this->scale_screenspace(src.position);
    auto p = Transform{
        .orientation = local_toggle ? src.orientation : Quaternion{0, 0, 0, 1},
        .position = src.position,
    };
    auto ray = this->ray.detransform(p);
    ray = ray.descale(draw_scale);
    return {draw_scale, p, ray};
  };
};

struct DragState {
  // Offset from position of grabbed object to coordinates of clicked point
  Float3 click_offset;
  // Original position of an object being manipulated with a gizmo
  Float3 original_position;
  // Original orientation of an object being manipulated with a gizmo
  Quaternion original_orientation;
  // Original scale of an object being manipulated with a gizmo
  Float3 original_scale;
};

struct RayState {
  Transform transform;
  float draw_scale;
  Transform gizmo_transform;
  Ray local_ray;
  float t;
};

struct TranslationGizmo {

  static void draw(const Float4x4 &modelMatrix,
                   const AddTriangleFunc &add_triangle,
                   GizmoComponentType active_component);

  static std::tuple<GizmoComponentType, float> intersect(const Ray &local_ray);

  static DragState begin_gizmo(const RayState &ray_state, bool local_toggle) {
    auto ray = ray_state.local_ray.scaling(ray_state.draw_scale);
    DragState drag_state = {
        .click_offset = ray.point(ray_state.t),
        .original_position = ray_state.transform.position,
    };
    if (local_toggle) {
      drag_state.click_offset =
          ray_state.gizmo_transform.transform_vector(drag_state.click_offset);
    }
    return drag_state;
  }

  static Transform drag(GizmoComponentType active_component,
                        const FrameState &frame, bool local_toggle,
                        const Transform &src, DragState *state);
};

struct RotationGizmo {
  static void draw(const Float4x4 &modelMatrix,
                   const AddTriangleFunc &add_triangle,
                   GizmoComponentType active_component);

  static std::tuple<GizmoComponentType, float> intersect(const Ray &local_ray);

  static DragState begin_gizmo(const RayState &ray_state, bool local_toggle) {
    auto ray = ray_state.local_ray.scaling(ray_state.draw_scale);
    DragState drag_state = {
        .click_offset = ray_state.transform.transform_point(
            ray.origin + ray.direction.scale(ray_state.t)),
        .original_orientation = ray_state.transform.orientation,
    };
    return drag_state;
  }

  static Transform drag(GizmoComponentType active_component,
                        const FrameState &frame, bool local_toggle,
                        const Transform &src, DragState *state);
};

struct ScalingGizmo {
  static void draw(const Float4x4 &modelMatrix,
                   const AddTriangleFunc &add_triangle,
                   GizmoComponentType active_component);

  static std::tuple<GizmoComponentType, float> intersect(const Ray &local_ray);

  static DragState begin_gizmo(const RayState &ray_state, bool local_toggle) {
    auto ray = ray_state.local_ray.scaling(ray_state.draw_scale);
    DragState drag_state = {
        .click_offset = ray_state.transform.transform_point(
            ray.origin + ray.direction.scale(ray_state.t)),
        .original_scale = ray_state.transform.scale,
    };
    return drag_state;
  }

  static Transform drag(GizmoComponentType active_component,
                        const FrameState &frame, bool local_toggle,
                        const Transform &src, bool uniform, DragState *state);
};

} // namespace tinygizmo
