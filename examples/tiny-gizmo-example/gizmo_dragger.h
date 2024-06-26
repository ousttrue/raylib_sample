#pragma once
#include "drawable.h"
#include "rdrag.h"
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

  std::shared_ptr<tinygizmo::TranslationGizmo> _t;
  std::shared_ptr<tinygizmo::RotationGizmo> _r;
  std::shared_ptr<tinygizmo::ScalingGizmo> _s;
  std::shared_ptr<tinygizmo::Gizmo> _visible;

  std::vector<Vector3> _positions;
  std::vector<Color> _colors;
  std::vector<unsigned short> _indices;

public:
  TRSGizmo(Camera *camera, std::list<std::shared_ptr<Drawable>> &scene)
      : _camera(camera), _scene(scene) {
    _t = std::make_shared<tinygizmo::TranslationGizmo>();
    _r = std::make_shared<tinygizmo::RotationGizmo>();
    _s = std::make_shared<tinygizmo::ScalingGizmo>();
    _visible = _t;
  }

  void begin(const Vector2 &cursor) override {
    float best_t = std::numeric_limits<float>::infinity();
    std::unordered_map<std::shared_ptr<Drawable>, tinygizmo::RayState> ray_map;
    for (auto &target : this->_scene) {
      auto [draw_scale, gizmo_transform, local_ray] =
          _active_state.gizmo_transform_and_local_ray(_local_toggle,
                                                      target->transform);
      // ray intersection
      auto [updated_state, t] = _visible->intersect(local_ray);
      if (updated_state) {
        ray_map.insert({target,
                        {
                            .transform = target->transform,
                            .draw_scale = draw_scale,
                            .gizmo_transform = gizmo_transform,
                            .local_ray = local_ray,
                            .t = t,
                        }});
        if (t < best_t) {
          best_t = t;
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
        target->transform = _visible->drag(&_drag_state, _active, _active_state,
                                           _local_toggle, target->transform);
      }
    }
  }

  void hotkey(int w, int h, const Vector2 &cursor, const hotkey &hotkey);
  void load(Drawable *drawable);
};
