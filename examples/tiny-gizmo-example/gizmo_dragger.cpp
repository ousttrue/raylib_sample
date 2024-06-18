#include "gizmo_dragger.h"
#include "ray.h"

struct RayState {
  minalg::rigid_transform transform;
  float draw_scale;
  minalg::rigid_transform gizmo_transform;
  tinygizmo::ray local_ray;
  float t;
};

void TranslationGizmo::begin(const Vector2 &cursor) {
  float best_t = std::numeric_limits<float>::infinity();
  std::unordered_map<std::shared_ptr<Drawable>, RayState> ray_map;
  for (auto &target : this->_scene) {
    minalg::rigid_transform src{
        .orientation = *(minalg::float4 *)&target->rotation,
        .position = *(minalg::float3 *)&target->position,
        .scale = *(minalg::float3 *)&target->scale,
    };
    auto [draw_scale, gizmo_transform, local_ray] =
        tinygizmo::gizmo_transform(gizmo_state, local_toggle, src);
    // ray intersection
    auto [updated_state, t] = tinygizmo::position_intersect(local_ray);
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
        this->active = updated_state;
        this->gizmo_target = target;
      }
    }
  }

  if (this->active) {
    auto ray_state = ray_map[this->gizmo_target];
    // begin drag
    auto ray = tinygizmo::scaling(ray_state.draw_scale, ray_state.local_ray);
    // click point in gizmo local
    this->drag_state = {
        .original_position = ray_state.transform.position,
        .click_offset = local_toggle
                            ? ray_state.gizmo_transform.transform_vector(
                                  ray.origin + ray.direction * ray_state.t)
                            : ray.origin + ray.direction * ray_state.t,
    };
  }
}

void TranslationGizmo::drag(const DragState &state, int w, int h,
                            const Vector2 &cursor) {
  for (auto &target : this->_scene) {
    if (this->active && this->gizmo_target == target) {
      minalg::rigid_transform src{
          .orientation = *(minalg::float4 *)&target->rotation,
          .position = *(minalg::float3 *)&target->position,
          .scale = *(minalg::float3 *)&target->scale,
      };
      auto [_0, gizmo_transform, _2] =
          tinygizmo::gizmo_transform(gizmo_state, local_toggle, src);
      auto position =
          tinygizmo::position_drag(&drag_state, gizmo_state, local_toggle,
                                   this->active, gizmo_transform);
      target->position = *(Vector3 *)&position;
    }
  }
}

void RotationGizmo::begin(const Vector2 &cursor) {
  float best_t = std::numeric_limits<float>::infinity();
  std::unordered_map<std::shared_ptr<Drawable>, RayState> ray_map;
  for (auto &target : this->_scene) {
    minalg::rigid_transform src{
        .orientation = *(minalg::float4 *)&target->rotation,
        .position = *(minalg::float3 *)&target->position,
        .scale = *(minalg::float3 *)&target->scale,
    };
    auto [draw_scale, gizmo_transform, local_ray] =
        tinygizmo::gizmo_transform(gizmo_state, local_toggle, src);
    // ray intersection
    auto [updated_state, t] = tinygizmo::rotation_intersect(local_ray);
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
        this->active = updated_state;
        this->gizmo_target = target;
      }
    }
  }

  if (this->active) {
    auto ray_state = ray_map[this->gizmo_target];
    // begin drag
    auto ray = tinygizmo::scaling(ray_state.draw_scale, ray_state.local_ray);
    // click point in gizmo local
    this->drag_state = {
        .original_position = ray_state.transform.position,
        // .click_offset = local_toggle
        //                     ? ray_state.gizmo_transform.transform_vector(
        //                           ray.origin + ray.direction * ray_state.t)
        //                     : ray.origin + ray.direction * ray_state.t,
        .click_offset = ray_state.transform.transform_point(
            ray.origin + ray.direction * ray_state.t),
        .original_orientation = ray_state.transform.orientation,
    };
  }
}

void RotationGizmo::drag(const DragState &state, int w, int h,
                         const Vector2 &cursor) {
  for (auto &target : this->_scene) {
    if (this->active && this->gizmo_target == target) {
      minalg::rigid_transform src{
          .orientation = *(minalg::float4 *)&target->rotation,
          .position = *(minalg::float3 *)&target->position,
          .scale = *(minalg::float3 *)&target->scale,
      };
      auto [_0, gizmo_transform, _2] =
          tinygizmo::gizmo_transform(gizmo_state, local_toggle, src);

      auto rotation =
          tinygizmo::rotation_drag(&drag_state, gizmo_state, local_toggle,
                                   this->active, gizmo_transform);
      target->rotation = *(Vector4 *)&rotation;
    }
  }
}

void ScalingGizmo::begin(const Vector2 &cursor) {
  float best_t = std::numeric_limits<float>::infinity();
  std::unordered_map<std::shared_ptr<Drawable>, RayState> ray_map;
  for (auto &target : this->_scene) {
    minalg::rigid_transform src{
        .orientation = *(minalg::float4 *)&target->rotation,
        .position = *(minalg::float3 *)&target->position,
        .scale = *(minalg::float3 *)&target->scale,
    };
    auto [draw_scale, gizmo_transform, local_ray] =
        tinygizmo::gizmo_transform(gizmo_state, local_toggle, src);
    // ray intersection
    auto [updated_state, t] = tinygizmo::scaling_intersect(local_ray);
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
        this->active = updated_state;
        this->gizmo_target = target;
      }
    }
  }

  if (this->active) {
    auto ray_state = ray_map[this->gizmo_target];
    // begin drag
    auto ray = tinygizmo::scaling(ray_state.draw_scale, ray_state.local_ray);
    // click point in gizmo local
    this->drag_state = {
        // .click_offset = local_toggle
        //                     ? ray_state.gizmo_transform.transform_vector(
        //                           ray.origin + ray.direction * ray_state.t)
        //                     : ray.origin + ray.direction * ray_state.t,
        .click_offset = ray_state.transform.transform_point(
            ray.origin + ray.direction * ray_state.t),
        .original_scale = ray_state.transform.scale,
    };
  }
}

void ScalingGizmo::drag(const DragState &state, int w, int h,
                        const Vector2 &cursor) {
  for (auto &target : this->_scene) {
    if (this->active && this->gizmo_target == target) {
      minalg::rigid_transform src{
          .orientation = *(minalg::float4 *)&target->rotation,
          .position = *(minalg::float3 *)&target->position,
          .scale = *(minalg::float3 *)&target->scale,
      };
      // auto [_0, gizmo_transform, _2] =
      //     tinygizmo::gizmo_transform(gizmo_state, local_toggle, src);
      auto scale = tinygizmo::scaling_drag(
          &drag_state, gizmo_state, local_toggle, this->active, src, uniform);
      target->scale = *(Vector3 *)&scale;
    }
  }
}

void TRSGizmo::hotkey(int w, int h, const Vector2 &cursor,
                      const struct hotkey &hotkey) {
  if (hotkey.hotkey_ctrl == true) {
    if (last_hotkey.hotkey_translate == false &&
        hotkey.hotkey_translate == true) {
      _active = _t;
    } else if (last_hotkey.hotkey_rotate == false &&
               hotkey.hotkey_rotate == true) {
      _active = _r;
    } else if (last_hotkey.hotkey_scale == false &&
               hotkey.hotkey_scale == true) {
      _active = _s;
    }

    local_toggle = (!last_hotkey.hotkey_local && hotkey.hotkey_local)
                       ? !local_toggle
                       : local_toggle;
  }
  last_hotkey = active_hotkey;
  active_hotkey = hotkey;

  auto ray = GetMouseRay(cursor, *_camera);
  auto rot =
      QuaternionFromEuler(ray.direction.x, ray.direction.y, ray.direction.z);

  last_state = active_state;
  active_state = {
      .mouse_left = IsMouseButtonDown(MOUSE_BUTTON_LEFT),
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

  _active->gizmo_state = active_state;
  _active->local_toggle = local_toggle;
}

void TRSGizmo::load(Drawable *drawable) {
  positions.clear();
  colors.clear();
  indices.clear();
  tinygizmo::AddTriangleFunc add_world_triangle =
      [self = this](
          const std::array<float, 4> &rgba, const std::array<float, 3> &p0,
          const std::array<float, 3> &p1, const std::array<float, 3> &p2) {
        //
        auto offset = self->positions.size();
        Color color{
            static_cast<unsigned char>(std::max(0.0f, rgba[0]) * 255),
            static_cast<unsigned char>(std::max(0.0f, rgba[1]) * 255),
            static_cast<unsigned char>(std::max(0.0f, rgba[2]) * 255),
            static_cast<unsigned char>(std::max(0.0f, rgba[3]) * 255),
        };
        self->positions.push_back({p0[0], p0[1], p0[2]});
        self->positions.push_back({p1[0], p1[1], p1[2]});
        self->positions.push_back({p2[0], p2[1], p2[2]});
        self->colors.push_back(color);
        self->colors.push_back(color);
        self->colors.push_back(color);
        self->indices.push_back(offset + 0);
        self->indices.push_back(offset + 1);
        self->indices.push_back(offset + 2);
      };

  for (auto &target : this->_scene) {
    minalg::rigid_transform src(*(minalg::float4 *)&target->rotation,
                                *(minalg::float3 *)&target->position,
                                *(minalg::float3 *)&target->scale);
    auto [draw_scale, p, ray] =
        tinygizmo::gizmo_transform(active_state, local_toggle, src);

    minalg::float4x4 modelMatrix = p.matrix();
    minalg::float4x4 scaleMatrix = scaling_matrix(minalg::float3(draw_scale));
    modelMatrix = mul(modelMatrix, scaleMatrix);

    // tinygizmo::position_draw(add_world_triangle, nullptr, modelMatrix);
    _active->draw(add_world_triangle, modelMatrix);
  }

  if (positions.size() && indices.size()) {
    drawable->load(positions.size(), positions.data(), colors.data(),
                   indices.size(), indices.data(), true);
  }
}
