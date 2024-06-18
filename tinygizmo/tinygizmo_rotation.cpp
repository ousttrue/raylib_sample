#include "tinygizmo_rotation.h"
#include <assert.h>

namespace tinygizmo {

std::vector<minalg::float2> ring_points = {
    {+0.025f, 1},    {-0.025f, 1},    {-0.025f, 1},    {-0.025f, 1.1f},
    {-0.025f, 1.1f}, {+0.025f, 1.1f}, {+0.025f, 1.1f}, {+0.025f, 1}};

auto _rotate_x = std::make_shared<gizmo_component>(
    geometry_mesh::make_lathed_geometry({1, 0, 0}, {0, 1, 0}, {0, 0, 1}, 32,
                                        ring_points, 0.003f),
    minalg::float4{1, 0.5f, 0.5f, 1.f}, minalg::float4{1, 0, 0, 1.f});

auto _rotate_y = std::make_shared<gizmo_component>(
    geometry_mesh::make_lathed_geometry({0, 1, 0}, {0, 0, 1}, {1, 0, 0}, 32,
                                        ring_points, -0.003f),
    minalg::float4{0.5f, 1, 0.5f, 1.f}, minalg::float4{0, 1, 0, 1.f});

auto _rotate_z = std::make_shared<gizmo_component>(
    geometry_mesh::make_lathed_geometry({0, 0, 1}, {1, 0, 0}, {0, 1, 0}, 32,
                                        ring_points),
    minalg::float4{0.5f, 0.5f, 1, 1.f}, minalg::float4{0, 0, 1, 1.f});

std::tuple<std::shared_ptr<gizmo_component>, float>
rotation_intersect(const ray &ray) {

  float best_t = std::numeric_limits<float>::infinity(), t;
  std::shared_ptr<gizmo_component> updated_state = {};
  if (ray.intersect_mesh(_rotate_x->mesh, &t) && t < best_t) {
    updated_state = _rotate_x;
    best_t = t;
  }
  if (ray.intersect_mesh(_rotate_y->mesh, &t) && t < best_t) {
    updated_state = _rotate_y;
    best_t = t;
  }
  if (ray.intersect_mesh(_rotate_z->mesh, &t) && t < best_t) {
    updated_state = _rotate_z;
    best_t = t;
  }
  return {updated_state, best_t};
}

inline minalg::float4 make_rotation_quat_axis_angle(const minalg::float3 &axis,
                                                    float angle) {
  return {axis * std::sin(angle / 2), std::cos(angle / 2)};
}

inline minalg::float4 make_rotation_quat_between_vectors_snapped(
    const minalg::float3 &from, const minalg::float3 &to, const float angle) {
  auto a = normalize(from);
  auto b = normalize(to);
  auto snappedAcos = std::floor(std::acos(dot(a, b)) / angle) * angle;
  return make_rotation_quat_axis_angle(normalize(cross(a, b)), snappedAcos);
}

static minalg::rigid_transform
axis_rotation_dragger(drag_state *drag,
                      const gizmo_application_state &active_state,
                      bool local_toggle, const minalg::float3 &axis,
                      const minalg::rigid_transform &src, bool) {

  const minalg::float4 start_orientation =
      local_toggle ? drag->original_orientation : minalg::float4(0, 0, 0, 1);

  if (!active_state.mouse_left) {
    return src;
  }

  minalg::rigid_transform original_pose = {start_orientation,
                                           drag->original_position};
  auto the_axis = original_pose.transform_vector(axis);
  minalg::float4 the_plane = {the_axis, -dot(the_axis, drag->click_offset)};

  auto t = active_state.ray.intersect_plane(the_plane);
  if (!t) {
    return src;
  }

  minalg::float3 center_of_rotation =
      drag->original_position +
      the_axis * dot(the_axis, drag->click_offset - drag->original_position);
  minalg::float3 arm1 = normalize(drag->click_offset - center_of_rotation);
  minalg::float3 arm2 =
      normalize(active_state.ray.point(*t) - center_of_rotation);

  float d = dot(arm1, arm2);
  if (d > 0.999f) {
    return minalg::rigid_transform(start_orientation, src.position, src.scale);
  }

  float angle = std::acos(d);
  if (angle < 0.001f) {
    return minalg::rigid_transform(start_orientation, src.position, src.scale);
  }

  if (active_state.snap_rotation) {
    auto snapped = make_rotation_quat_between_vectors_snapped(
        arm1, arm2, active_state.snap_rotation);
    return minalg::rigid_transform(qmul(snapped, start_orientation),
                                   src.position, src.scale);
  } else {
    auto a = normalize(cross(arm1, arm2));
    return minalg::rigid_transform(
        qmul(rotation_quat(a, angle), start_orientation), src.position,
        src.scale);
  }
}

minalg::float4 rotation_drag(drag_state *drag,
                             const gizmo_application_state &state,
                             bool local_toggle,
                             const std::shared_ptr<gizmo_component> &active,
                             const minalg::rigid_transform &src) {
  if (active == _rotate_x) {
    return axis_rotation_dragger(drag, state, local_toggle, {1, 0, 0}, src, {})
        .orientation;
  } else if (active == _rotate_y) {
    return axis_rotation_dragger(drag, state, local_toggle, {0, 1, 0}, src, {})
        .orientation;
  } else if (active == _rotate_z) {
    return axis_rotation_dragger(drag, state, local_toggle, {0, 0, 1}, src, {})
        .orientation;
  } else {
    assert(false);
    return src.orientation;
  }
}

void rotation_draw(const AddTriangleFunc &add_world_triangle,
                   const std::shared_ptr<gizmo_component> &active,
                   const minalg::float4x4 &modelMatrix) {

  std::vector<std::shared_ptr<gizmo_component>> draw_interactions;
  // if (!state.local_toggle && this->active)
  //   draw_interactions = {interaction_mode(this->active)};
  // else
  draw_interactions = {
      _rotate_x,
      _rotate_y,
      _rotate_z,
  };

  for (auto c : draw_interactions) {
    add_triangles(add_world_triangle, modelMatrix, c->mesh,
                  (c == active) ? c->base_color : c->highlight_color);
  }

  //   minalg::float4 orientation;
  //   // For non-local transformations, we only present one rotation ring
  //   // and draw an arrow from the center of the gizmo to indicate the degree
  //   // of rotation
  //   if (state.local_toggle == false && this->active) {
  //
  //     // Create orthonormal basis for drawing the arrow
  //     minalg::float3 a = qrot(p.orientation, this->drag.click_offset -
  //                                                this->drag.original_position);
  //
  //     minalg::float3 activeAxis;
  //     // rigid_transform src(_orientation, center, {1, 1, 1});
  //     switch (interaction_mode(this->active)) {
  //     case interact::rotate_x:
  //       activeAxis = {1, 0, 0};
  //       break;
  //     case interact::rotate_y:
  //       activeAxis = {0, 1, 0};
  //       break;
  //     case interact::rotate_z:
  //       activeAxis = {0, 0, 1};
  //       break;
  //     default:
  //       assert(false);
  //       break;
  //     }
  //     minalg::float3 zDir = normalize(activeAxis),
  //                    xDir = normalize(cross(a, zDir)), yDir = cross(zDir,
  //                    xDir);
  //
  //     // Ad-hoc geometry
  //     std::initializer_list<minalg::float2> arrow_points = {
  //         {0.0f, 0.f}, {0.0f, 0.05f}, {0.8f, 0.05f}, {0.9f, 0.10f}, {1.0f,
  //         0}};
  //     auto geo = make_lathed_geometry(yDir, xDir, zDir, 32, arrow_points);
  //
  //     add_triangles(add_world_triangle, modelMatrix, geo, minalg::float4(1));
  //
  //     orientation = qmul(p.orientation, this->drag.original_orientation);
  //   } else if (state.local_toggle == true && this->active) {
  //     orientation = p.orientation;
  //   }
  //
}

} // namespace tinygizmo
