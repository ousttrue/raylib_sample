#pragma once
#include "tinygizmo_alg.h"
#include <optional>

namespace tinygizmo {

enum class GizmoComonentType {
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
  ScalingUniform,
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

struct GizmoComponent {
  GeometryMesh mesh;
  Float4 base_color;
  Float4 highlight_color;

  GizmoComponent(const GeometryMesh &mesh, const Float4 &base,
                  const Float4 &high)
      : mesh(mesh), base_color(base), highlight_color(high) {}

  virtual ~GizmoComponent() {}

  virtual std::optional<Transform> drag(DragState *drag,
                                        const FrameState &active_state,
                                        bool local_toggle,
                                        const Transform &src) const {
    return {};
  }
};

} // namespace tinygizmo
