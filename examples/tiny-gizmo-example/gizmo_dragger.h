#pragma once
#include "drawable.h"
#include "rdrag.h"
#include <list>
#include <tinygizmo_rotation.h>
#include <tinygizmo_scaling.h>
#include <tinygizmo_translation.h>

struct RayState {
  tinygizmo::RigidTransform transform;
  float draw_scale;
  tinygizmo::RigidTransform gizmo_transform;
  tinygizmo::Ray local_ray;
  float t;
};

class Gizmo {
public:
  virtual ~Gizmo() {}
  virtual std::tuple<std::shared_ptr<tinygizmo::gizmo_component>, float>
  intersect(const tinygizmo::Ray &local_ray) = 0;

  virtual tinygizmo::DragState begin_gizmo(const RayState &ray_state,
                                           bool local_toggle) = 0;

  virtual void
  drag(const std::shared_ptr<Drawable> &target, tinygizmo::DragState *drag,
       const std::shared_ptr<tinygizmo::gizmo_component> &component,
       const tinygizmo::FrameState &frame, bool local_toggle) = 0;

  virtual void
  draw(const std::shared_ptr<tinygizmo::gizmo_component> &component,
       const tinygizmo::AddTriangleFunc &add_triangle,
       const tinygizmo::Float4x4 &modelMatrix) = 0;
};

class TranslationGizmo : public Gizmo {
public:
  std::tuple<std::shared_ptr<tinygizmo::gizmo_component>, float>
  intersect(const tinygizmo::Ray &local_ray) override {
    return tinygizmo::position_intersect(local_ray);
  };

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

  void drag(const std::shared_ptr<Drawable> &target,
            tinygizmo::DragState *state,
            const std::shared_ptr<tinygizmo::gizmo_component> &component,
            const tinygizmo::FrameState &frame, bool local_toggle) override {

    tinygizmo::RigidTransform src{
        .orientation = *(tinygizmo::Quaternion *)&target->rotation,
        .position = *(tinygizmo::Float3 *)&target->position,
        .scale = *(tinygizmo::Float3 *)&target->scale,
    };
    auto position =
        tinygizmo::position_drag(state, frame, local_toggle, component, src);
    target->position = *(Vector3 *)&position;
  }

  void draw(const std::shared_ptr<tinygizmo::gizmo_component> &component,
            const tinygizmo::AddTriangleFunc &add_triangle,
            const tinygizmo::Float4x4 &modelMatrix) override {
    tinygizmo::position_draw(add_triangle, component, modelMatrix);
  }
};

class RotationGizmo : public Gizmo {
public:
  std::tuple<std::shared_ptr<tinygizmo::gizmo_component>, float>
  intersect(const tinygizmo::Ray &local_ray) override {
    return tinygizmo::rotation_intersect(local_ray);
  };

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

  void drag(const std::shared_ptr<Drawable> &target,
            tinygizmo::DragState *state,
            const std::shared_ptr<tinygizmo::gizmo_component> &component,
            const tinygizmo::FrameState &frame, bool local_toggle) override {
    tinygizmo::RigidTransform src{
        .orientation = *(tinygizmo::Quaternion *)&target->rotation,
        .position = *(tinygizmo::Float3 *)&target->position,
        .scale = *(tinygizmo::Float3 *)&target->scale,
    };
    auto rotation =
        tinygizmo::rotation_drag(state, frame, local_toggle, component, src);
    target->rotation = *(Vector4 *)&rotation;
  }

  void draw(const std::shared_ptr<tinygizmo::gizmo_component> &component,
            const tinygizmo::AddTriangleFunc &add_triangle,
            const tinygizmo::Float4x4 &modelMatrix) override {
    tinygizmo::rotation_draw(add_triangle, component, modelMatrix);
  }
};

class ScalingGizmo : public Gizmo {
public:
  bool uniform = false;
  std::tuple<std::shared_ptr<tinygizmo::gizmo_component>, float>
  intersect(const tinygizmo::Ray &local_ray) override {
    return tinygizmo::scaling_intersect(local_ray);
  };

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

  void drag(const std::shared_ptr<Drawable> &target,
            tinygizmo::DragState *state,
            const std::shared_ptr<tinygizmo::gizmo_component> &component,
            const tinygizmo::FrameState &frame, bool local_toggle) override {
    tinygizmo::RigidTransform src{
        .orientation = *(tinygizmo::Quaternion *)&target->rotation,
        .position = *(tinygizmo::Float3 *)&target->position,
        .scale = *(tinygizmo::Float3 *)&target->scale,
    };
    auto scale = tinygizmo::scaling_drag(state, frame, local_toggle, component,
                                         src, uniform);
    target->scale = *(Vector3 *)&scale;
  }
  void draw(const std::shared_ptr<tinygizmo::gizmo_component> &component,
            const tinygizmo::AddTriangleFunc &add_triangle,
            const tinygizmo::Float4x4 &modelMatrix) override {
    tinygizmo::scaling_draw(add_triangle, component, modelMatrix);
  }
};

struct hotkey {
  bool hotkey_ctrl = false;
  bool hotkey_translate = false;
  bool hotkey_rotate = false;
  bool hotkey_scale = false;
  bool hotkey_local = false;
};

class TRSGizmo : public Dragger {
  Camera *_camera;
  std::list<std::shared_ptr<Drawable>> _scene;
  std::shared_ptr<Drawable> _gizmo_target;

  hotkey _active_hotkey = {0};
  hotkey _last_hotkey = {0};
  tinygizmo::FrameState _active_state;
  tinygizmo::FrameState _last_state;
  bool _local_toggle = true;

  std::shared_ptr<tinygizmo::gizmo_component> _active;
  tinygizmo::DragState _drag_state;

  std::shared_ptr<TranslationGizmo> _t;
  std::shared_ptr<RotationGizmo> _r;
  std::shared_ptr<ScalingGizmo> _s;
  std::shared_ptr<Gizmo> _visible;

  std::vector<Vector3> _positions;
  std::vector<Color> _colors;
  std::vector<unsigned short> _indices;

public:
  TRSGizmo(Camera *camera, std::list<std::shared_ptr<Drawable>> &scene)
      : _camera(camera), _scene(scene) {
    _t = std::make_shared<TranslationGizmo>();
    _r = std::make_shared<RotationGizmo>();
    _s = std::make_shared<ScalingGizmo>();
    _visible = _t;
  }

  void begin(const Vector2 &cursor) override {
    float best_t = std::numeric_limits<float>::infinity();
    std::unordered_map<std::shared_ptr<Drawable>, RayState> ray_map;
    for (auto &target : this->_scene) {
      tinygizmo::RigidTransform src{
          .orientation = *(tinygizmo::Quaternion *)&target->rotation,
          .position = *(tinygizmo::Float3 *)&target->position,
          .scale = *(tinygizmo::Float3 *)&target->scale,
      };
      auto [draw_scale, gizmo_transform, local_ray] =
          _active_state.gizmo_transform_and_local_ray(_local_toggle, src);
      // ray intersection
      auto [updated_state, t] = _visible->intersect(local_ray);
      if (updated_state) {
        ray_map.insert({target,
                        {
                            .transform = src,
                            .draw_scale = draw_scale,
                            .gizmo_transform = gizmo_transform,
                            .local_ray = local_ray,
                            .t = t,
                        }});
        if (t < best_t) {
          best_t= t;
          this->_active = updated_state;
          this->_gizmo_target = target;
        }
      }
    }

    if (this->_active) {
      // begin drag
      auto ray_state = ray_map[this->_gizmo_target];
      this->_drag_state = _visible->begin_gizmo(ray_state, _local_toggle);
    }
  }

  void end(const Vector2 &cursor) override {
    this->_active = {};
    this->_gizmo_target = {};
    this->_drag_state = {};
  }

  void drag(const DragState &state, int w, int h,
            const Vector2 &cursor) override {
    for (auto &target : this->_scene) {
      if (this->_active && this->_gizmo_target == target) {
        _visible->drag(target, &_drag_state, _active, _active_state,
                       _local_toggle);
      }
    }
  }

  void hotkey(int w, int h, const Vector2 &cursor, const hotkey &hotkey);
  void load(Drawable *drawable);
};
