#include "tinygizmo_scaling.h"

namespace tinygizmo {

std::vector<minalg::float2> mace_points = {{0.25f, 0},    {0.25f, 0.05f},
                                           {1, 0.05f},    {1, 0.1f},
                                           {1.25f, 0.1f}, {1.25f, 0}};

auto _scale_x = std::make_shared<gizmo_mesh_component>(
    make_lathed_geometry({1, 0, 0}, {0, 1, 0}, {0, 0, 1}, 16, mace_points),
    minalg::float4{1, 0.5f, 0.5f, 1.f}, minalg::float4{1, 0, 0, 1.f});

auto _scale_y = std::make_shared<gizmo_mesh_component>(
    make_lathed_geometry({0, 1, 0}, {0, 0, 1}, {1, 0, 0}, 16, mace_points),
    minalg::float4{0.5f, 1, 0.5f, 1.f}, minalg::float4{0, 1, 0, 1.f});

auto _scale_z = std::make_shared<gizmo_mesh_component>(
    make_lathed_geometry({0, 0, 1}, {1, 0, 0}, {0, 1, 0}, 16, mace_points),
    minalg::float4{0.5f, 0.5f, 1, 1.f}, minalg::float4{0, 0, 1, 1.f});

std::tuple<std::shared_ptr<gizmo_mesh_component>, float>
scaling_intersect(const ray &ray) {

  float best_t = std::numeric_limits<float>::infinity(), t;
  std::shared_ptr<gizmo_mesh_component> updated_state = {};
  if (intersect_ray_mesh(ray, _scale_x->mesh, &t) && t < best_t) {
    updated_state = _scale_x;
    best_t = t;
  }
  if (intersect_ray_mesh(ray, _scale_y->mesh, &t) && t < best_t) {
    updated_state = _scale_y;
    best_t = t;
  }
  if (intersect_ray_mesh(ray, _scale_z->mesh, &t) && t < best_t) {
    updated_state = _scale_z;
    best_t = t;
  }

  return {updated_state, best_t};
}

static rigid_transform
axis_scale_dragger(drag_state *drag,
                   const gizmo_application_state &active_state,
                   bool local_toggle, const minalg::float3 &axis,
                   const rigid_transform &src, bool uniform) {
  if (!active_state.mouse_left) {
    return src;
  }

  const minalg::float3 plane_tangent =
      cross(axis, src.position - to_minalg(active_state.ray_origin));
  const minalg::float3 plane_normal = cross(axis, plane_tangent);

  // Define the plane to contain the original position of the object
  const minalg::float3 plane_point = src.position;
  const ray ray = {
      to_minalg(active_state.ray_origin),
      to_minalg(active_state.ray_direction),
  };

  // If an intersection exists between the ray and the plane, place the
  // object at that point
  const float denom = dot(ray.direction, plane_normal);
  if (std::abs(denom) == 0)
    return src;

  const float t = dot(plane_point - ray.origin, plane_normal) / denom;
  if (t < 0)
    return src;

  auto distance = ray.origin + ray.direction * t;

  minalg::float3 offset_on_axis = (distance - drag->click_offset) * axis;
  flush_to_zero(offset_on_axis);
  minalg::float3 new_scale = drag->original_scale + offset_on_axis;

  minalg::float3 scale =
      (uniform) ? minalg::float3(clamp(dot(distance, new_scale), 0.01f, 1000.f))
                : minalg::float3(clamp(new_scale.x, 0.01f, 1000.f),
                                 clamp(new_scale.y, 0.01f, 1000.f),
                                 clamp(new_scale.z, 0.01f, 1000.f));
  if (active_state.snap_scale) {
    scale = snap(scale, active_state.snap_scale);
  }
  return rigid_transform(src.orientation, src.position, scale);
}

minalg::float3 scaling_drag(drag_state *drag,
                            const gizmo_application_state &state,
                            bool local_toggle,
                            const std::shared_ptr<gizmo_mesh_component> &active,
                            const rigid_transform &src, bool uniform) {

  auto scale = src.scale;
  if (active) {
    rigid_transform _src{
        .orientation = {0, 0, 0, 1},
        .position = src.position,
        .scale = scale,
    };
    if (active == _scale_x) {
      scale = axis_scale_dragger(drag, state, local_toggle, {1, 0, 0}, _src,
                                 uniform)
                  .scale;
    } else if (active == _scale_y) {
      scale = axis_scale_dragger(drag, state, local_toggle, {0, 1, 0}, _src,
                                 uniform)
                  .scale;
    } else if (active == _scale_z) {
      scale = axis_scale_dragger(drag, state, local_toggle, {0, 0, 1}, _src,
                                 uniform)
                  .scale;
    }
  }
  return scale;
}

void scaling_draw(const AddTriangleFunc &add_world_triangle,
                  const std::shared_ptr<gizmo_mesh_component> &active,
                  const minalg::float4x4 &modelMatrix) {
  std::vector<std::shared_ptr<gizmo_mesh_component>> draw_components{
      _scale_x,
      _scale_y,
      _scale_z,
  };

  for (auto c : draw_components) {
    add_triangles(add_world_triangle, modelMatrix, c->mesh,
                  (c == active) ? c->base_color : c->highlight_color);
  }
}

} // namespace tinygizmo
