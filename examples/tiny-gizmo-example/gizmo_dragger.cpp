#include "gizmo_dragger.h"
#include <assert.h>
#include <iostream>

void TRSGizmo::begin(const Vector2 &cursor) {
  float best_t = std::numeric_limits<float>::infinity();
  for (auto &target : this->_scene) {
    //
    // ray intersection
    //
    switch (this->_visible) {
    case GizmoMode::Translation: {
      auto intersection = tinygizmo::TranslationGizmo::intersect(
          _current_state, _local_toggle, target->transform);
      if (intersection) {
        auto [ray_state, active_component] = *intersection;
        if (ray_state.t < best_t) {
          best_t = ray_state.t;
          this->_t = active_component;
          this->_gizmo_target = target;
          this->_ray_state = ray_state;
        }
      }
      break;
    }

    case GizmoMode::Rotation: {
      auto intersection = tinygizmo::RotationGizmo::intersect(
          _current_state, _local_toggle, target->transform);
      if (intersection) {
        auto [ray_state, active_component] = *intersection;
        if (ray_state.t < best_t) {
          best_t = ray_state.t;
          this->_r = active_component;
          this->_gizmo_target = target;
          this->_ray_state = ray_state;
        }
      }
      break;
    }

    case GizmoMode::Scaling: {
      auto intersection = tinygizmo::ScalingGizmo::intersect(
          _current_state, _local_toggle, target->transform, _uniform);
      if (intersection) {
        auto [ray_state, active_component] = *intersection;
        if (ray_state.t < best_t) {
          best_t = ray_state.t;
          this->_s = active_component;
          this->_gizmo_target = target;
          this->_ray_state = ray_state;
        }
      }
      break;
    }

    default:
      assert(false);
      throw std::runtime_error("unknown gizmo mode");
    }
  }
}

void TRSGizmo::end(const Vector2 &cursor) {
  _t = {};
  _r = {};
  _s = {};
  _gizmo_target = {};
  _ray_state = {};
}

void TRSGizmo::drag(const DragState &state, int w, int h,
                    const Vector2 &cursor) {
  if (auto target = _gizmo_target) {
    switch (_visible) {
    case GizmoMode::Translation:
      if (_t) {
        target->transform = tinygizmo::TranslationGizmo::drag(
            *_t, _current_state, _ray_state, target->transform);
      }
      break;
    case GizmoMode::Rotation:
      if (_r) {
        target->transform = tinygizmo::RotationGizmo::drag(
            *_r, _current_state, _ray_state, target->transform);
      }
      break;
    case GizmoMode::Scaling:
      if (_s) {
        target->transform = tinygizmo::ScalingGizmo::drag(
            *_s, _current_state, _ray_state, target->transform);
      }
      break;
    }
  }
}

void TRSGizmo::hotkey(int w, int h, const Vector2 &cursor,
                      const struct Hotkey &hotkey) {
  if (hotkey.hotkey_ctrl == true) {
    if (_last_hotkey.hotkey_translate == false &&
        hotkey.hotkey_translate == true) {
      _visible = GizmoMode::Translation;
    }
    if (_last_hotkey.hotkey_rotate == false && hotkey.hotkey_rotate == true) {
      _visible = GizmoMode::Rotation;
    }
    if (_last_hotkey.hotkey_scale == false && hotkey.hotkey_scale == true) {
      _visible = GizmoMode::Scaling;
    }

    if (hotkey.hotkey_local) {
      _local_toggle = !_local_toggle;
      std::cout << "_local_toggle: " << _local_toggle << std::endl;
    }
  }
  _last_hotkey = _current_hotkey;
  _current_hotkey = hotkey;

  auto ray = GetMouseRay(cursor, *_camera);
  auto rot =
      QuaternionFromEuler(ray.direction.x, ray.direction.y, ray.direction.z);

  _last_state = _current_state;
  _current_state = {
      .mouse_down = IsMouseButtonDown(MOUSE_BUTTON_LEFT),
      // optional flag to draw the gizmos at a constant screen-space
      // scale gizmo_state.screenspace_scale = 80.f; camera projection
      .viewport_size = {static_cast<float>(w), static_cast<float>(h)},
      .ray =
          {
              .origin = {ray.position.x, ray.position.y, ray.position.z},
              .direction = {ray.direction.x, ray.direction.y, ray.direction.z},
          },
      .cam_yfov = 1.0f,
      .cam_orientation = {rot.x, rot.y, rot.z, rot.w},
  };
}

void TRSGizmo::load(Drawable *drawable) {
  _positions.clear();
  _colors.clear();
  _indices.clear();
  tinygizmo::AddTriangleFunc add_world_triangle =
      [self = this](const tinygizmo::Float4 &rgba, const tinygizmo::Float3 &p0,
                    const tinygizmo::Float3 &p1, const tinygizmo::Float3 &p2) {
        //
        auto offset = self->_positions.size();
        Color color{
            static_cast<unsigned char>(std::max(0.0f, rgba.x) * 255),
            static_cast<unsigned char>(std::max(0.0f, rgba.y) * 255),
            static_cast<unsigned char>(std::max(0.0f, rgba.z) * 255),
            static_cast<unsigned char>(std::max(0.0f, rgba.w) * 255),
        };
        self->_positions.push_back({p0.x, p0.y, p0.z});
        self->_positions.push_back({p1.x, p1.y, p1.z});
        self->_positions.push_back({p2.x, p2.y, p2.z});
        self->_colors.push_back(color);
        self->_colors.push_back(color);
        self->_colors.push_back(color);
        self->_indices.push_back(offset + 0);
        self->_indices.push_back(offset + 1);
        self->_indices.push_back(offset + 2);
      };

  for (auto &target : this->_scene) {
    auto [draw_scale, p, ray] = _current_state.gizmo_transform_and_local_ray(
        _local_toggle, target->transform);
    auto gizmoMatrix = p.matrix() * tinygizmo::Float4x4::scaling(
                                        draw_scale, draw_scale, draw_scale);
    switch (this->_visible) {
    case GizmoMode::Translation:
      tinygizmo::TranslationGizmo::mesh(gizmoMatrix, add_world_triangle, _t);
      break;
    case GizmoMode::Rotation:
      tinygizmo::RotationGizmo::mesh(gizmoMatrix, add_world_triangle, _r);
      break;
    case GizmoMode::Scaling:
      tinygizmo::ScalingGizmo::mesh(gizmoMatrix, add_world_triangle, _s);
      break;
    }
  }

  if (_positions.size() && _indices.size()) {
    drawable->load(_positions.size(), _positions.data(), _colors.data(),
                   _indices.size(), _indices.data(), true);
  }
}
