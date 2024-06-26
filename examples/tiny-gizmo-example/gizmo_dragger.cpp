#include "gizmo_dragger.h"
#include <iostream>

void TRSGizmo::hotkey(int w, int h, const Vector2 &cursor,
                      const struct hotkey &hotkey) {
  if (hotkey.hotkey_ctrl == true) {
    if (_last_hotkey.hotkey_translate == false &&
        hotkey.hotkey_translate == true) {
      _visible = _t;
    }
    if (_last_hotkey.hotkey_rotate == false && hotkey.hotkey_rotate == true) {
      _visible = _r;
    }
    if (_last_hotkey.hotkey_scale == false && hotkey.hotkey_scale == true) {
      _visible = _s;
    }

    if (hotkey.hotkey_local) {
      _local_toggle = !_local_toggle;
      std::cout << "_local_toggle: " << _local_toggle << std::endl;
    }
  }
  _last_hotkey = _active_hotkey;
  _active_hotkey = hotkey;

  auto ray = GetMouseRay(cursor, *_camera);
  auto rot =
      QuaternionFromEuler(ray.direction.x, ray.direction.y, ray.direction.z);

  _last_state = _active_state;
  _active_state = {
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
    tinygizmo::Transform src(*(tinygizmo::Quaternion *)&target->rotation,
                                  *(tinygizmo::Float3 *)&target->position,
                                  *(tinygizmo::Float3 *)&target->scale);
    auto [draw_scale, p, ray] =
        _active_state.gizmo_transform_and_local_ray(_local_toggle, src);

    auto modelMatrix = p.matrix();
    auto scaleMatrix =
        tinygizmo::Float4x4::scaling(draw_scale, draw_scale, draw_scale);
    modelMatrix = modelMatrix * scaleMatrix;

    _visible->draw(_active, add_world_triangle, modelMatrix);
  }

  if (_positions.size() && _indices.size()) {
    drawable->load(_positions.size(), _positions.data(), _colors.data(),
                   _indices.size(), _indices.data(), true);
  }
}
