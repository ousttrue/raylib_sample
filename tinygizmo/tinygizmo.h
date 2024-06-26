// This is free and unencumbered software released into the public domain.
// For more information, please refer to <http://unlicense.org>
#pragma once
#include "tinygizmo_alg.h"
#include "tinygizmo_component.h"
#include <memory>

namespace tinygizmo {

struct RayState {
  tinygizmo::Transform transform;
  float draw_scale;
  tinygizmo::Transform gizmo_transform;
  tinygizmo::Ray local_ray;
  float t;
};

class Gizmo {
public:
  virtual ~Gizmo() {}
  virtual std::tuple<std::shared_ptr<gizmo_component>, float>
  intersect(const tinygizmo::Ray &local_ray) = 0;

  virtual tinygizmo::DragState begin_gizmo(const RayState &ray_state,
                                           bool local_toggle) = 0;

  virtual tinygizmo::Transform
  drag(tinygizmo::DragState *drag,
       const std::shared_ptr<tinygizmo::gizmo_component> &component,
       const tinygizmo::FrameState &frame, bool local_toggle,
       const tinygizmo::Transform &src) = 0;

  virtual void
  draw(const std::shared_ptr<tinygizmo::gizmo_component> &component,
       const tinygizmo::AddTriangleFunc &add_triangle,
       const tinygizmo::Float4x4 &modelMatrix) = 0;
};

class TranslationGizmo : public Gizmo {
public:
  void draw(const std::shared_ptr<tinygizmo::gizmo_component> &component,
            const tinygizmo::AddTriangleFunc &add_triangle,
            const tinygizmo::Float4x4 &modelMatrix) override;

  std::tuple<std::shared_ptr<tinygizmo::gizmo_component>, float>
  intersect(const tinygizmo::Ray &local_ray) override;

  tinygizmo::DragState begin_gizmo(const RayState &ray_state,
                                   bool local_toggle) override {
    auto ray = ray_state.local_ray.scaling(ray_state.draw_scale);
    tinygizmo::DragState drag_state = {
        .click_offset = ray.point(ray_state.t),
        .original_position = ray_state.transform.position,
    };
    if (local_toggle) {
      drag_state.click_offset =
          ray_state.gizmo_transform.transform_vector(drag_state.click_offset);
    }
    return drag_state;
  }

  Transform drag(tinygizmo::DragState *state,
                 const std::shared_ptr<tinygizmo::gizmo_component> &component,
                 const tinygizmo::FrameState &frame, bool local_toggle,
                 const Transform &src) override;
};

class RotationGizmo : public Gizmo {
public:
  void draw(const std::shared_ptr<tinygizmo::gizmo_component> &component,
            const tinygizmo::AddTriangleFunc &add_triangle,
            const tinygizmo::Float4x4 &modelMatrix) override;

  std::tuple<std::shared_ptr<tinygizmo::gizmo_component>, float>
  intersect(const tinygizmo::Ray &local_ray) override;

  tinygizmo::DragState begin_gizmo(const RayState &ray_state,
                                   bool local_toggle) override {
    auto ray = ray_state.local_ray.scaling(ray_state.draw_scale);
    tinygizmo::DragState drag_state = {
        .click_offset = ray_state.transform.transform_point(
            ray.origin + ray.direction.scale(ray_state.t)),
        .original_orientation = ray_state.transform.orientation,
    };
    return drag_state;
  }

  Transform drag(tinygizmo::DragState *state,
                 const std::shared_ptr<tinygizmo::gizmo_component> &component,
                 const tinygizmo::FrameState &frame, bool local_toggle,
                 const Transform &src) override;
};

class ScalingGizmo : public Gizmo {
public:
  void draw(const std::shared_ptr<tinygizmo::gizmo_component> &component,
            const tinygizmo::AddTriangleFunc &add_triangle,
            const tinygizmo::Float4x4 &modelMatrix) override;

  bool uniform = false;
  std::tuple<std::shared_ptr<tinygizmo::gizmo_component>, float>
  intersect(const tinygizmo::Ray &local_ray) override;

  tinygizmo::DragState begin_gizmo(const RayState &ray_state,
                                   bool local_toggle) override {
    auto ray = ray_state.local_ray.scaling(ray_state.draw_scale);
    tinygizmo::DragState drag_state = {
        .click_offset = ray_state.transform.transform_point(
            ray.origin + ray.direction.scale(ray_state.t)),
        .original_scale = ray_state.transform.scale,
    };
    return drag_state;
  }

  Transform drag(tinygizmo::DragState *state,
                 const std::shared_ptr<tinygizmo::gizmo_component> &component,
                 const tinygizmo::FrameState &frame, bool local_toggle,
                 const Transform &src) override;
};

} // namespace tinygizmo
