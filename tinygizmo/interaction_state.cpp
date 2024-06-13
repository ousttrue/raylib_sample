#include "interaction_state.hpp"

namespace tinygizmo {

minalg::float3 to_minalg(const float3 &v) { return {v.x, v.y, v.z}; }
minalg::float4 to_minalg(const float4 &v) { return {v.x, v.y, v.z, v.w}; }

interaction_state::interaction_state() {
  std::vector<minalg::float2> arrow_points = {
      {0.25f, 0}, {0.25f, 0.05f}, {1, 0.05f}, {1, 0.10f}, {1.2f, 0}};
  std::vector<minalg::float2> mace_points = {{0.25f, 0},    {0.25f, 0.05f},
                                             {1, 0.05f},    {1, 0.1f},
                                             {1.25f, 0.1f}, {1.25f, 0}};
  std::vector<minalg::float2> ring_points = {
      {+0.025f, 1},    {-0.025f, 1},    {-0.025f, 1},    {-0.025f, 1.1f},
      {-0.025f, 1.1f}, {+0.025f, 1.1f}, {+0.025f, 1.1f}, {+0.025f, 1}};
  mesh_components[interact::translate_x] = {
      make_lathed_geometry({1, 0, 0}, {0, 1, 0}, {0, 0, 1}, 16, arrow_points),
      {1, 0.5f, 0.5f, 1.f},
      {1, 0, 0, 1.f}};
  mesh_components[interact::translate_y] = {
      make_lathed_geometry({0, 1, 0}, {0, 0, 1}, {1, 0, 0}, 16, arrow_points),
      {0.5f, 1, 0.5f, 1.f},
      {0, 1, 0, 1.f}};
  mesh_components[interact::translate_z] = {
      make_lathed_geometry({0, 0, 1}, {1, 0, 0}, {0, 1, 0}, 16, arrow_points),
      {0.5f, 0.5f, 1, 1.f},
      {0, 0, 1, 1.f}};
  mesh_components[interact::translate_yz] = {
      make_box_geometry({-0.01f, 0.25, 0.25}, {0.01f, 0.75f, 0.75f}),
      {0.5f, 1, 1, 0.5f},
      {0, 1, 1, 0.6f}};
  mesh_components[interact::translate_zx] = {
      make_box_geometry({0.25, -0.01f, 0.25}, {0.75f, 0.01f, 0.75f}),
      {1, 0.5f, 1, 0.5f},
      {1, 0, 1, 0.6f}};
  mesh_components[interact::translate_xy] = {
      make_box_geometry({0.25, 0.25, -0.01f}, {0.75f, 0.75f, 0.01f}),
      {1, 1, 0.5f, 0.5f},
      {1, 1, 0, 0.6f}};
  mesh_components[interact::translate_xyz] = {
      make_box_geometry({-0.05f, -0.05f, -0.05f}, {0.05f, 0.05f, 0.05f}),
      {0.9f, 0.9f, 0.9f, 0.25f},
      {1, 1, 1, 0.35f}};
  mesh_components[interact::rotate_x] = {
      make_lathed_geometry({1, 0, 0}, {0, 1, 0}, {0, 0, 1}, 32, ring_points,
                           0.003f),
      {1, 0.5f, 0.5f, 1.f},
      {1, 0, 0, 1.f}};
  mesh_components[interact::rotate_y] = {
      make_lathed_geometry({0, 1, 0}, {0, 0, 1}, {1, 0, 0}, 32, ring_points,
                           -0.003f),
      {0.5f, 1, 0.5f, 1.f},
      {0, 1, 0, 1.f}};
  mesh_components[interact::rotate_z] = {
      make_lathed_geometry({0, 0, 1}, {1, 0, 0}, {0, 1, 0}, 32, ring_points),
      {0.5f, 0.5f, 1, 1.f},
      {0, 0, 1, 1.f}};
  mesh_components[interact::scale_x] = {
      make_lathed_geometry({1, 0, 0}, {0, 1, 0}, {0, 0, 1}, 16, mace_points),
      {1, 0.5f, 0.5f, 1.f},
      {1, 0, 0, 1.f}};
  mesh_components[interact::scale_y] = {
      make_lathed_geometry({0, 1, 0}, {0, 0, 1}, {1, 0, 0}, 16, mace_points),
      {0.5f, 1, 0.5f, 1.f},
      {0, 1, 0, 1.f}};
  mesh_components[interact::scale_z] = {
      make_lathed_geometry({0, 0, 1}, {1, 0, 0}, {0, 1, 0}, 16, mace_points),
      {0.5f, 0.5f, 1, 1.f},
      {0, 0, 1, 1.f}};
}

// The only purpose of this is readability: to reduce the total column width
// of the intersect(...) statements in every gizmo
bool interaction_state::intersect(const ray &r, interact i, float &t,
                                  const float best_t) {
  if (intersect_ray_mesh(r, this->mesh_components[i].mesh, &t) && t < best_t)
    return true;
  return false;
}

// This will calculate a scale constant based on the number of screenspace
// pixels passed as pixel_scale.
float interaction_state::scale_screenspace(const gizmo_state &state,
                                           const minalg::float3 position,
                                           const float pixel_scale) {
  float dist = length(position - to_minalg(state.active_state.ray_origin));
  return std::tan(state.active_state.cam_yfov) * dist *
         (pixel_scale / state.active_state.viewport_size.y);
}

void interaction_state::plane_translation_dragger(
    const gizmo_state &state, const minalg::float3 &plane_normal,
    minalg::float3 &point) {
  // interaction_state &interaction = this->gizmos[id];

  // Mouse clicked
  if (state.has_clicked()) {
    this->original_position = point;
  }

  if (state.active_state.mouse_left) {
    // Define the plane to contain the original position of the object
    auto plane_point = this->original_position;
    const ray r = {
        to_minalg(state.active_state.ray_origin),
        to_minalg(state.active_state.ray_direction),
    };

    // If an intersection exists between the ray and the plane, place the
    // object at that point
    const float denom = dot(r.direction, plane_normal);
    if (std::abs(denom) == 0)
      return;

    const float t = dot(plane_point - r.origin, plane_normal) / denom;
    if (t < 0)
      return;

    point = r.origin + r.direction * t;

    if (state.active_state.snap_translation)
      point = snap(point, state.active_state.snap_translation);
  }
}

void interaction_state::axis_translation_dragger(const gizmo_state &state,
                                                 const minalg::float3 &axis,
                                                 minalg::float3 &point) {
  if (state.active_state.mouse_left) {
    // First apply a plane translation dragger with a plane that contains the
    // desired axis and is oriented to face the camera
    const minalg::float3 plane_tangent =
        cross(axis, point - to_minalg(state.active_state.ray_origin));
    const minalg::float3 plane_normal = cross(axis, plane_tangent);
    this->plane_translation_dragger(state, plane_normal, point);

    // Constrain object motion to be along the desired axis
    point = this->original_position +
            axis * dot(point - this->original_position, axis);
  }
}

void interaction_state::axis_rotation_dragger(
    const gizmo_state &state, const minalg::float3 &axis,
    const minalg::float3 &center, const minalg::float4 &start_orientation,
    minalg::float4 &orientation) {

  if (state.active_state.mouse_left) {
    rigid_transform original_pose = {start_orientation,
                                     this->original_position};
    auto the_axis = original_pose.transform_vector(axis);
    minalg::float4 the_plane = {the_axis, -dot(the_axis, this->click_offset)};
    const ray r = {
        to_minalg(state.active_state.ray_origin),
        to_minalg(state.active_state.ray_direction),
    };

    float t;
    if (intersect_ray_plane(r, the_plane, &t)) {
      minalg::float3 center_of_rotation =
          this->original_position +
          the_axis *
              dot(the_axis, this->click_offset - this->original_position);
      minalg::float3 arm1 = normalize(this->click_offset - center_of_rotation);
      minalg::float3 arm2 =
          normalize(r.origin + r.direction * t - center_of_rotation);

      float d = dot(arm1, arm2);
      if (d > 0.999f) {
        orientation = start_orientation;
        return;
      }

      float angle = std::acos(d);
      if (angle < 0.001f) {
        orientation = start_orientation;
        return;
      }

      if (state.active_state.snap_rotation) {
        auto snapped = make_rotation_quat_between_vectors_snapped(
            arm1, arm2, state.active_state.snap_rotation);
        orientation = qmul(snapped, start_orientation);
      } else {
        auto a = normalize(cross(arm1, arm2));
        orientation = qmul(rotation_quat(a, angle), start_orientation);
      }
    }
  }
}

void interaction_state::axis_scale_dragger(const gizmo_state &state,
                                           const minalg::float3 &axis,
                                           const minalg::float3 &center,
                                           minalg::float3 &scale,
                                           const bool uniform) {
  if (state.active_state.mouse_left) {
    const minalg::float3 plane_tangent =
        cross(axis, center - to_minalg(state.active_state.ray_origin));
    const minalg::float3 plane_normal = cross(axis, plane_tangent);

    minalg::float3 distance;
    if (state.active_state.mouse_left) {
      // Define the plane to contain the original position of the object
      const minalg::float3 plane_point = center;
      const ray ray = {
          to_minalg(state.active_state.ray_origin),
          to_minalg(state.active_state.ray_direction),
      };

      // If an intersection exists between the ray and the plane, place the
      // object at that point
      const float denom = dot(ray.direction, plane_normal);
      if (std::abs(denom) == 0)
        return;

      const float t = dot(plane_point - ray.origin, plane_normal) / denom;
      if (t < 0)
        return;

      distance = ray.origin + ray.direction * t;
    }

    minalg::float3 offset_on_axis = (distance - this->click_offset) * axis;
    flush_to_zero(offset_on_axis);
    minalg::float3 new_scale = this->original_scale + offset_on_axis;

    if (uniform)
      scale = minalg::float3(clamp(dot(distance, new_scale), 0.01f, 1000.f));
    else
      scale = minalg::float3(clamp(new_scale.x, 0.01f, 1000.f),
                             clamp(new_scale.y, 0.01f, 1000.f),
                             clamp(new_scale.z, 0.01f, 1000.f));
    if (state.active_state.snap_scale) {
      scale = snap(scale, state.active_state.snap_scale);
    }
  }
}

gizmo_result
interaction_state::position_gizmo(const gizmo_state &state, bool local_toggle,
                                  const minalg::float4 &rotation,
                                  const minalg::float3 &_position) {
  rigid_transform p = rigid_transform(
      local_toggle ? rotation : minalg::float4(0, 0, 0, 1), _position);

  const float draw_scale =
      (state.active_state.screenspace_scale > 0.f)
          ? this->scale_screenspace(state, p.position,
                                    state.active_state.screenspace_scale)
          : 1.f;

  // interaction_mode will only change on clicked
  if (state.has_clicked()) {
    this->interaction_mode = interact::none;
  }

  {
    interact updated_state = interact::none;
    auto ray = detransform(p, {
                                  to_minalg(state.active_state.ray_origin),
                                  to_minalg(state.active_state.ray_direction),
                              });
    detransform(draw_scale, ray);

    float best_t = std::numeric_limits<float>::infinity(), t;
    if (this->intersect(ray, interact::translate_x, t, best_t)) {
      updated_state = interact::translate_x;
      best_t = t;
    }
    if (this->intersect(ray, interact::translate_y, t, best_t)) {
      updated_state = interact::translate_y;
      best_t = t;
    }
    if (this->intersect(ray, interact::translate_z, t, best_t)) {
      updated_state = interact::translate_z;
      best_t = t;
    }
    if (this->intersect(ray, interact::translate_yz, t, best_t)) {
      updated_state = interact::translate_yz;
      best_t = t;
    }
    if (this->intersect(ray, interact::translate_zx, t, best_t)) {
      updated_state = interact::translate_zx;
      best_t = t;
    }
    if (this->intersect(ray, interact::translate_xy, t, best_t)) {
      updated_state = interact::translate_xy;
      best_t = t;
    }
    if (this->intersect(ray, interact::translate_xyz, t, best_t)) {
      updated_state = interact::translate_xyz;
      best_t = t;
    }

    if (state.has_clicked()) {
      this->interaction_mode = updated_state;

      if (this->interaction_mode != interact::none) {
        transform(draw_scale, ray);
        this->click_offset =
            local_toggle ? p.transform_vector(ray.origin + ray.direction * t)
                         : ray.origin + ray.direction * t;
        this->active = true;
      } else {
        this->active = false;
      }
    }

    this->hover =
        (best_t == std::numeric_limits<float>::infinity()) ? false : true;
  }

  std::vector<minalg::float3> axes;
  if (local_toggle)
    axes = {qxdir(p.orientation), qydir(p.orientation), qzdir(p.orientation)};
  else
    axes = {{1, 0, 0}, {0, 1, 0}, {0, 0, 1}};

  minalg::float3 position = _position;
  if (this->active) {
    position += this->click_offset;
    switch (this->interaction_mode) {
    case interact::translate_x:
      this->axis_translation_dragger(state, axes[0], position);
      break;
    case interact::translate_y:
      this->axis_translation_dragger(state, axes[1], position);
      break;
    case interact::translate_z:
      this->axis_translation_dragger(state, axes[2], position);
      break;
    case interact::translate_yz:
      this->plane_translation_dragger(state, axes[0], position);
      break;
    case interact::translate_zx:
      this->plane_translation_dragger(state, axes[1], position);
      break;
    case interact::translate_xy:
      this->plane_translation_dragger(state, axes[2], position);
      break;
    case interact::translate_xyz:
      this->plane_translation_dragger(
          state, -minalg::qzdir(to_minalg(state.active_state.cam_orientation)),
          position);
      break;
    default:
      assert(false);
      break;
    }
    position -= this->click_offset;
  }

  if (state.has_released()) {
    this->interaction_mode = interact::none;
    this->active = false;
  }

  std::vector<interact> draw_interactions{
      interact::translate_x,  interact::translate_y,  interact::translate_z,
      interact::translate_yz, interact::translate_zx, interact::translate_xy,
      interact::translate_xyz};

  minalg::float4x4 modelMatrix = p.matrix();
  minalg::float4x4 scaleMatrix = scaling_matrix(minalg::float3(draw_scale));
  modelMatrix = mul(modelMatrix, scaleMatrix);

  for (auto c : draw_interactions) {
    state.add_drawable(modelMatrix, this->mesh_components[c].mesh,
                       (c == this->interaction_mode)
                           ? this->mesh_components[c].base_color
                           : this->mesh_components[c].highlight_color);
  }

  return {.hover = this->hover,
          .active = this->active,
          .t = {
              position.x,
              position.y,
              position.z,
          }};
}

gizmo_result
interaction_state::rotation_gizmo(const gizmo_state &state, bool local_toggle,
                                  const minalg::float3 &center,
                                  const minalg::float4 &_orientation) {
  assert(length2(_orientation) > float(1e-6));

  rigid_transform p =
      rigid_transform(local_toggle ? _orientation : minalg::float4(0, 0, 0, 1),
                      center); // Orientation is local by default
  const float draw_scale =
      (state.active_state.screenspace_scale > 0.f)
          ? this->scale_screenspace(state, p.position,
                                    state.active_state.screenspace_scale)
          : 1.f;

  // interaction_mode will only change on clicked
  if (state.has_clicked()) {
    this->interaction_mode = interact::none;
  }

  {
    interact updated_state = interact::none;

    auto ray = detransform(p, {
                                  to_minalg(state.active_state.ray_origin),
                                  to_minalg(state.active_state.ray_direction),
                              });
    detransform(draw_scale, ray);
    float best_t = std::numeric_limits<float>::infinity(), t;

    if (this->intersect(ray, interact::rotate_x, t, best_t)) {
      updated_state = interact::rotate_x;
      best_t = t;
    }
    if (this->intersect(ray, interact::rotate_y, t, best_t)) {
      updated_state = interact::rotate_y;
      best_t = t;
    }
    if (this->intersect(ray, interact::rotate_z, t, best_t)) {
      updated_state = interact::rotate_z;
      best_t = t;
    }

    if (state.has_clicked()) {
      this->interaction_mode = updated_state;
      if (this->interaction_mode != interact::none) {
        transform(draw_scale, ray);
        this->original_position = center;
        this->original_orientation = _orientation;
        this->click_offset = p.transform_point(ray.origin + ray.direction * t);
        this->active = true;
      } else
        this->active = false;
    }
  }

  minalg::float3 activeAxis;
  if (this->active) {
    const minalg::float4 starting_orientation =
        local_toggle ? this->original_orientation : minalg::float4(0, 0, 0, 1);
    switch (this->interaction_mode) {
    case interact::rotate_x:
      this->axis_rotation_dragger(state, {1, 0, 0}, center,
                                  starting_orientation, p.orientation);
      activeAxis = {1, 0, 0};
      break;
    case interact::rotate_y:
      this->axis_rotation_dragger(state, {0, 1, 0}, center,
                                  starting_orientation, p.orientation);
      activeAxis = {0, 1, 0};
      break;
    case interact::rotate_z:
      this->axis_rotation_dragger(state, {0, 0, 1}, center,
                                  starting_orientation, p.orientation);
      activeAxis = {0, 0, 1};
      break;
    default:
      assert(false);
      break;
    }
  }

  if (state.has_released()) {
    this->interaction_mode = interact::none;
    this->active = false;
  }

  minalg::float4x4 modelMatrix = p.matrix();
  minalg::float4x4 scaleMatrix = scaling_matrix(minalg::float3(draw_scale));
  modelMatrix = mul(modelMatrix, scaleMatrix);

  std::vector<interact> draw_interactions;
  if (!local_toggle && this->interaction_mode != interact::none)
    draw_interactions = {this->interaction_mode};
  else
    draw_interactions = {interact::rotate_x, interact::rotate_y,
                         interact::rotate_z};

  for (auto c : draw_interactions) {
    state.add_drawable(modelMatrix, this->mesh_components[c].mesh,
                       (c == this->interaction_mode)
                           ? this->mesh_components[c].base_color
                           : this->mesh_components[c].highlight_color);
  }

  auto orientation = _orientation;
  // For non-local transformations, we only present one rotation ring
  // and draw an arrow from the center of the gizmo to indicate the degree
  // of rotation
  if (local_toggle == false && this->interaction_mode != interact::none) {

    // Create orthonormal basis for drawing the arrow
    minalg::float3 a =
        qrot(p.orientation, this->click_offset - this->original_position);
    minalg::float3 zDir = normalize(activeAxis),
                   xDir = normalize(cross(a, zDir)), yDir = cross(zDir, xDir);

    // Ad-hoc geometry
    std::initializer_list<minalg::float2> arrow_points = {
        {0.0f, 0.f}, {0.0f, 0.05f}, {0.8f, 0.05f}, {0.9f, 0.10f}, {1.0f, 0}};
    auto geo = make_lathed_geometry(yDir, xDir, zDir, 32, arrow_points);

    state.add_drawable(modelMatrix, geo, minalg::float4(1));

    orientation = qmul(p.orientation, this->original_orientation);
  } else if (local_toggle == true && this->interaction_mode != interact::none) {
    orientation = p.orientation;
  }

  return {.hover = this->hover,
          .active = this->active,
          .r = {
              orientation.x,
              orientation.y,
              orientation.z,
              orientation.w,
          }};
}

gizmo_result interaction_state::scale_gizmo(const gizmo_state &state,
                                            bool local_toggle,
                                            const minalg::float4 &orientation,
                                            const minalg::float3 &center,
                                            const minalg::float3 &_scale,
                                            bool uniform) {
  rigid_transform p = rigid_transform(orientation, center);
  const float draw_scale =
      (state.active_state.screenspace_scale > 0.f)
          ? this->scale_screenspace(state, p.position,
                                    state.active_state.screenspace_scale)
          : 1.f;

  if (state.has_clicked()) {
    this->interaction_mode = interact::none;
  }

  {
    interact updated_state = interact::none;
    auto ray = detransform(p, {
                                  to_minalg(state.active_state.ray_origin),
                                  to_minalg(state.active_state.ray_direction),
                              });
    detransform(draw_scale, ray);
    float best_t = std::numeric_limits<float>::infinity(), t;
    if (this->intersect(ray, interact::scale_x, t, best_t)) {
      updated_state = interact::scale_x;
      best_t = t;
    }
    if (this->intersect(ray, interact::scale_y, t, best_t)) {
      updated_state = interact::scale_y;
      best_t = t;
    }
    if (this->intersect(ray, interact::scale_z, t, best_t)) {
      updated_state = interact::scale_z;
      best_t = t;
    }

    if (state.has_clicked()) {
      this->interaction_mode = updated_state;
      if (this->interaction_mode != interact::none) {
        transform(draw_scale, ray);
        this->original_scale = _scale;
        this->click_offset = p.transform_point(ray.origin + ray.direction * t);
        this->active = true;
      } else
        this->active = false;
    }
  }

  if (state.has_released()) {
    this->interaction_mode = interact::none;
    this->active = false;
  }

  auto scale = _scale;
  if (this->active) {
    switch (this->interaction_mode) {
    case interact::scale_x:
      this->axis_scale_dragger(state, {1, 0, 0}, center, scale, uniform);
      break;
    case interact::scale_y:
      this->axis_scale_dragger(state, {0, 1, 0}, center, scale, uniform);
      break;
    case interact::scale_z:
      this->axis_scale_dragger(state, {0, 0, 1}, center, scale, uniform);
      break;
    default:
      assert(false);
      break;
    }
  }

  minalg::float4x4 modelMatrix = p.matrix();
  minalg::float4x4 scaleMatrix = scaling_matrix(minalg::float3(draw_scale));
  modelMatrix = mul(modelMatrix, scaleMatrix);

  std::vector<interact> draw_components{interact::scale_x, interact::scale_y,
                                        interact::scale_z};

  for (auto c : draw_components) {
    state.add_drawable(modelMatrix, this->mesh_components[c].mesh,
                       (c == this->interaction_mode)
                           ? this->mesh_components[c].base_color
                           : this->mesh_components[c].highlight_color);
  }

  return {.hover = this->hover,
          .active = this->active,
          .s = {
              scale.x,
              scale.y,
              scale.z,
          }};
}

} // namespace tinygizmo
