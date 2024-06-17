#include "tinygizmo_translation.h"
#include <assert.h>

namespace tinygizmo {

std::vector<minalg::float2> arrow_points = {
    {0.25f, 0}, {0.25f, 0.05f}, {1, 0.05f}, {1, 0.10f}, {1.2f, 0}};

auto _translate_x = std::make_shared<gizmo_component>(
    geometry_mesh::make_lathed_geometry({1, 0, 0}, {0, 1, 0}, {0, 0, 1}, 16,
                                        arrow_points),
    minalg::float4{1, 0.5f, 0.5f, 1.f}, minalg::float4{1, 0, 0, 1.f});
auto _translate_y = std::make_shared<gizmo_component>(
    geometry_mesh::make_lathed_geometry({0, 1, 0}, {0, 0, 1}, {1, 0, 0}, 16,
                                        arrow_points),
    minalg::float4{0.5f, 1, 0.5f, 1.f}, minalg::float4{0, 1, 0, 1.f});
auto _translate_z = std::make_shared<gizmo_component>(
    geometry_mesh::make_lathed_geometry({0, 0, 1}, {1, 0, 0}, {0, 1, 0}, 16,
                                        arrow_points),
    minalg::float4{0.5f, 0.5f, 1, 1.f}, minalg::float4{0, 0, 1, 1.f});
auto _translate_yz = std::make_shared<gizmo_component>(
    geometry_mesh::make_box_geometry({-0.01f, 0.25, 0.25},
                                     {0.01f, 0.75f, 0.75f}),
    minalg::float4{0.5f, 1, 1, 0.5f}, minalg::float4{0, 1, 1, 0.6f});
auto _translate_zx = std::make_shared<gizmo_component>(
    geometry_mesh::make_box_geometry({0.25, -0.01f, 0.25},
                                     {0.75f, 0.01f, 0.75f}),
    minalg::float4{1, 0.5f, 1, 0.5f}, minalg::float4{1, 0, 1, 0.6f});
auto _translate_xy = std::make_shared<gizmo_component>(
    geometry_mesh::make_box_geometry({0.25, 0.25, -0.01f},
                                     {0.75f, 0.75f, 0.01f}),
    minalg::float4{1, 1, 0.5f, 0.5f}, minalg::float4{1, 1, 0, 0.6f});
auto _translate_xyz = std::make_shared<gizmo_component>(
    geometry_mesh::make_box_geometry({-0.05f, -0.05f, -0.05f},
                                     {0.05f, 0.05f, 0.05f}),
    minalg::float4{0.9f, 0.9f, 0.9f, 0.25f}, minalg::float4{1, 1, 1, 0.35f});

std::tuple<std::shared_ptr<gizmo_component>, float>
position_intersect(const ray &ray) {
  float best_t = std::numeric_limits<float>::infinity(), t;
  std::shared_ptr<gizmo_component> updated_state = {};
  if (ray.intersect_mesh(_translate_x->mesh, &t) && t < best_t) {
    updated_state = _translate_x;
    best_t = t;
  }
  if (ray.intersect_mesh(_translate_y->mesh, &t) && t < best_t) {
    updated_state = _translate_y;
    best_t = t;
  }
  if (ray.intersect_mesh(_translate_z->mesh, &t) && t < best_t) {
    updated_state = _translate_z;
    best_t = t;
  }
  if (ray.intersect_mesh(_translate_yz->mesh, &t) && t < best_t) {
    updated_state = _translate_yz;
    best_t = t;
  }
  if (ray.intersect_mesh(_translate_zx->mesh, &t) && t < best_t) {
    updated_state = _translate_zx;
    best_t = t;
  }
  if (ray.intersect_mesh(_translate_xy->mesh, &t) && t < best_t) {
    updated_state = _translate_xy;
    best_t = t;
  }
  if (ray.intersect_mesh(_translate_xyz->mesh, &t) && t < best_t) {
    updated_state = _translate_xyz;
    best_t = t;
  }
  return {updated_state, best_t};
}

minalg::rigid_transform
plane_translation_dragger(drag_state *drag,
                          const gizmo_application_state &active_state,
                          bool local_toggle, const minalg::float3 &plane_normal,
                          const minalg::rigid_transform &src, bool) {
  // Define the plane to contain the original position of the object
  auto plane_point = drag->original_position;
  const ray r = {
      to_minalg(active_state.ray_origin),
      to_minalg(active_state.ray_direction),
  };

  // If an intersection exists between the ray and the plane, place the
  // object at that point
  const float denom = dot(r.direction, plane_normal);
  if (std::abs(denom) == 0) {
    return src;
  }

  const float t = dot(plane_point - r.origin, plane_normal) / denom;
  if (t < 0) {
    return src;
  }

  auto point = r.origin + r.direction * t;
  if (active_state.snap_translation) {
    point = snap(point, active_state.snap_translation);
  }
  return minalg::rigid_transform(src.orientation, point, src.scale);
}

minalg::rigid_transform
axis_translation_dragger(drag_state *drag,
                         const gizmo_application_state &active_state,
                         bool local_toggle, const minalg::float3 &axis,
                         const minalg::rigid_transform &t, bool) {
  // First apply a plane translation dragger with a plane that contains the
  // desired axis and is oriented to face the camera
  const minalg::float3 plane_tangent =
      cross(axis, t.position - to_minalg(active_state.ray_origin));
  const minalg::float3 plane_normal = cross(axis, plane_tangent);
  auto dst = plane_translation_dragger(drag, active_state, local_toggle,
                                       plane_normal, t, {});

  // Constrain object motion to be along the desired axis
  auto point = drag->original_position +
               axis * dot(dst.position - drag->original_position, axis);
  return minalg::rigid_transform(t.orientation, point, t.scale);
}

minalg::float3 position_drag(drag_state *drag,
                             const gizmo_application_state &state,
                             bool local_toggle,
                             const std::shared_ptr<gizmo_component> &active,
                             const minalg::rigid_transform &p) {
  std::vector<minalg::float3> axes;
  if (local_toggle)
    axes = {qxdir(p.orientation), qydir(p.orientation), qzdir(p.orientation)};
  else
    axes = {{1, 0, 0}, {0, 1, 0}, {0, 0, 1}};

  minalg::float3 position = p.position;
  if (active) {
    position += drag->click_offset;
    minalg::rigid_transform src(minalg::float4(0, 0, 0, 1), position,
                                minalg::float3(1, 1, 1));
    if (active == _translate_x) {
      position =
          axis_translation_dragger(drag, state, local_toggle, axes[0], src, {})
              .position;
    } else if (active == _translate_y) {
      position =
          axis_translation_dragger(drag, state, local_toggle, axes[1], src, {})
              .position;
    } else if (active == _translate_z) {
      position =
          axis_translation_dragger(drag, state, local_toggle, axes[2], src, {})
              .position;
    } else if (active == _translate_yz) {
      position =
          plane_translation_dragger(drag, state, local_toggle, axes[0], src, {})
              .position;
    } else if (active == _translate_zx) {
      position =
          plane_translation_dragger(drag, state, local_toggle, axes[1], src, {})
              .position;
    } else if (active == _translate_xy) {
      position =
          plane_translation_dragger(drag, state, local_toggle, axes[2], src, {})
              .position;
    } else if (active == _translate_xyz) {
      position = plane_translation_dragger(
                     drag, state, local_toggle,
                     -minalg::qzdir(to_minalg(state.cam_orientation)), src, {})
                     .position;
    } else {
      assert(false);
    }
    position -= drag->click_offset;
  }
  return position;
}

void position_draw(const AddTriangleFunc &add_world_triangle,
                   const std::shared_ptr<gizmo_component> &active,
                   const minalg::float4x4 &modelMatrix) {
  std::vector<std::shared_ptr<gizmo_component>> draw_interactions{
      _translate_x,  _translate_y,  _translate_z,   _translate_yz,
      _translate_zx, _translate_xy, _translate_xyz,
  };

  for (auto c : draw_interactions) {
    auto mesh = c->mesh;
    auto color = (c == active) ? c->base_color : c->highlight_color;
    add_triangles(add_world_triangle, modelMatrix, mesh, color);
  }
}

} // namespace tinygizmo
