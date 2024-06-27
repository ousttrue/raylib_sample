#pragma once
#include "drawable.h"
#include "rdrag.h"
#include <assert.h>
#include <list>
#include <tinygizmo_rotation.h>
#include <tinygizmo_scaling.h>
#include <tinygizmo_translation.h>

struct hotkey {
  bool hotkey_ctrl = false;
  bool hotkey_translate = false;
  bool hotkey_rotate = false;
  bool hotkey_scale = false;
  bool hotkey_local = false;
};

enum class GizmoMode {
  Translation,
  Rotation,
  Scaling,
};

class TRSGizmo : public Dragger {
  Camera *_camera;
  std::list<std::shared_ptr<Drawable>> _scene;
  std::shared_ptr<Drawable> _gizmo_target;

  hotkey _current_hotkey = {0};
  hotkey _last_hotkey = {0};
  tinygizmo::FrameState _current_state;
  tinygizmo::FrameState _last_state;
  bool _local_toggle = true;
  bool _uniform = true;

  std::optional<tinygizmo::TranslationGizmo::GizmoComponentType> _t = {};
  std::optional<tinygizmo::RotationGizmo::GizmoComponentType> _r = {};
  std::optional<tinygizmo::ScalingGizmo::GizmoComponentType> _s = {};
  tinygizmo::RayState _drag_state;

  GizmoMode _visible = GizmoMode::Translation;

  std::vector<Vector3> _positions;
  std::vector<Color> _colors;
  std::vector<unsigned short> _indices;

public:
  TRSGizmo(Camera *camera, std::list<std::shared_ptr<Drawable>> &scene)
      : _camera(camera), _scene(scene) {}

  void begin(const Vector2 &cursor) override {
    float best_t = std::numeric_limits<float>::infinity();
    for (auto &target : this->_scene) {
      auto [draw_scale, gizmo_transform, local_ray] =
          _current_state.gizmo_transform_and_local_ray(_local_toggle,
                                                       target->transform);
      // ray intersection
      auto t = _intersect(local_ray);
      if (t < best_t) {
        best_t = t;
        this->_drag_state = {
            .local_toggle = _local_toggle,
            .transform = target->transform,
            .draw_scale = draw_scale,
            .gizmo_transform = gizmo_transform,
            .local_ray = local_ray,
            .t = t,
        };
        this->_gizmo_target = target;
      }
    }
  }

  // if (active_component != tinygizmo::GizmoComponentType::None) {
  float _intersect(const tinygizmo::Ray &local_ray) {
    switch (this->_visible) {
    case GizmoMode::Translation: {
      auto [active_component, t] =
          tinygizmo::TranslationGizmo::intersect(local_ray);
      if (active_component) {
        this->_t = active_component;
      }
      return t;
    }

    case GizmoMode::Rotation: {
      auto [active_component, t] =
          tinygizmo::RotationGizmo::intersect(local_ray);
      if (active_component) {
        this->_r = active_component;
      }
      return t;
    }

    case GizmoMode::Scaling: {
      auto [active_component, t] =
          tinygizmo::ScalingGizmo::intersect(local_ray);
      if (active_component) {
        this->_s = active_component;
      }
      return t;
    }

    default:
      assert(false);
      break;
    }
  }

  void end(const Vector2 &cursor) override {
    _t = {};
    _r = {};
    _s = {};
    _gizmo_target = {};
    _drag_state = {};
  }

  void drag(const DragState &state, int w, int h,
            const Vector2 &cursor) override {
    if (auto target = _gizmo_target) {
      switch (_visible) {
      case GizmoMode::Translation:
        if (_t) {
          target->transform = tinygizmo::TranslationGizmo::drag(
              *_t, _current_state, _local_toggle, target->transform,
              _drag_state);
        }
        break;
      case GizmoMode::Rotation:
        if (_r) {
          target->transform =
              tinygizmo::RotationGizmo::drag(*_r, _current_state, _local_toggle,
                                             target->transform, _drag_state);
        }
        break;
      case GizmoMode::Scaling:
        if (_s) {
          target->transform = tinygizmo::ScalingGizmo::drag(
              *_s, _current_state, _local_toggle, target->transform, _uniform,
              _drag_state);
        }
        break;
      }
    }
  }

  void hotkey(int w, int h, const Vector2 &cursor, const hotkey &hotkey);
  void load(Drawable *drawable);
};
