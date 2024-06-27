// This is free and unencumbered software released into the public domain.
// For more information, please refer to <http://unlicense.org>
#pragma once
#include "tinygizmo_alg.h"

namespace tinygizmo {

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

struct RayState {
  bool local_toggle;
  Transform transform;
  float draw_scale;
  Transform gizmo_transform;
  Ray local_ray;
  float t;
};

namespace TranslationGizmo {
enum class GizmoComponentType {
  TranslationX,
  TranslationY,
  TranslationZ,
  TranslationXY,
  TranslationYZ,
  TranslationZX,
  TranslationView,
};

void draw(const Float4x4 &modelMatrix, const AddTriangleFunc &add_triangle,
          std::optional<GizmoComponentType> active_component);

std::tuple<std::optional<GizmoComponentType>, float>
intersect(const Ray &local_ray);

Transform drag(GizmoComponentType active_component, const FrameState &frame,
               bool local_toggle, const Transform &src, const RayState &state);
}; // namespace TranslationGizmo

namespace RotationGizmo {
enum class GizmoComponentType {
  RotationX,
  RotationY,
  RotationZ,
};

void draw(const Float4x4 &modelMatrix, const AddTriangleFunc &add_triangle,
          std::optional<GizmoComponentType> active_component);

std::tuple<std::optional<GizmoComponentType>, float>
intersect(const Ray &local_ray);

Transform drag(GizmoComponentType active_component, const FrameState &frame,
               bool local_toggle, const Transform &src, const RayState &state);
} // namespace RotationGizmo

namespace ScalingGizmo {
enum class GizmoComponentType {
  ScalingX,
  ScalingY,
  ScalingZ,
};

void draw(const Float4x4 &modelMatrix, const AddTriangleFunc &add_triangle,
          std::optional<GizmoComponentType> active_component);

std::tuple<std::optional<GizmoComponentType>, float>
intersect(const Ray &local_ray);

Transform drag(GizmoComponentType active_component, const FrameState &frame,
               bool local_toggle, const Transform &src, bool uniform,
               const RayState &state);
}; // namespace ScalingGizmo

} // namespace tinygizmo
