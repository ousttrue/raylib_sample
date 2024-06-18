#include "tinygizmo_scaling.h"

namespace tinygizmo {

template <typename T> T clamp(const T &val, const T &min, const T &max) {
  return std::min(std::max(val, min), max);
}

std::vector<minalg::float2> mace_points = {{0.25f, 0},    {0.25f, 0.05f},
                                           {1, 0.05f},    {1, 0.1f},
                                           {1.25f, 0.1f}, {1.25f, 0}};

auto _scale_x = std::make_shared<gizmo_component>(
    geometry_mesh::make_lathed_geometry({1, 0, 0}, {0, 1, 0}, {0, 0, 1}, 16,
                                        mace_points),
    minalg::float4{1, 0.5f, 0.5f, 1.f}, minalg::float4{1, 0, 0, 1.f});

auto _scale_y = std::make_shared<gizmo_component>(
    geometry_mesh::make_lathed_geometry({0, 1, 0}, {0, 0, 1}, {1, 0, 0}, 16,
                                        mace_points),
    minalg::float4{0.5f, 1, 0.5f, 1.f}, minalg::float4{0, 1, 0, 1.f});

auto _scale_z = std::make_shared<gizmo_component>(
    geometry_mesh::make_lathed_geometry({0, 0, 1}, {1, 0, 0}, {0, 1, 0}, 16,
                                        mace_points),
    minalg::float4{0.5f, 0.5f, 1, 1.f}, minalg::float4{0, 0, 1, 1.f});

std::tuple<std::shared_ptr<gizmo_component>, float>
scaling_intersect(const ray &ray) {

  float best_t = std::numeric_limits<float>::infinity(), t;
  std::shared_ptr<gizmo_component> updated_state = {};
  if (ray.intersect_mesh(_scale_x->mesh, &t) && t < best_t) {
    updated_state = _scale_x;
    best_t = t;
  }
  if (ray.intersect_mesh(_scale_y->mesh, &t) && t < best_t) {
    updated_state = _scale_y;
    best_t = t;
  }
  if (ray.intersect_mesh(_scale_z->mesh, &t) && t < best_t) {
    updated_state = _scale_z;
    best_t = t;
  }

  return {updated_state, best_t};
}

inline void flush_to_zero(minalg::float3 &f) {
  if (std::abs(f.x) < 0.02f)
    f.x = 0.f;
  if (std::abs(f.y) < 0.02f)
    f.y = 0.f;
  if (std::abs(f.z) < 0.02f)
    f.z = 0.f;
}

static minalg::rigid_transform
axis_scale_dragger(drag_state *drag,
                   const gizmo_application_state &active_state,
                   bool local_toggle, const minalg::float3 &axis,
                   const minalg::rigid_transform &src, bool uniform) {
  if (!active_state.mouse_left) {
    return src;
  }

  const minalg::float3 plane_tangent =
      cross(axis, src.position - active_state.ray.origin);
  const minalg::float3 plane_normal = cross(axis, plane_tangent);

  // Define the plane to contain the original position of the object
  const minalg::float3 plane_point = src.position;

  // If an intersection exists between the ray and the plane, place the
  // object at that point
  const float denom = dot(active_state.ray.direction, plane_normal);
  if (std::abs(denom) == 0)
    return src;

  const float t =
      dot(plane_point - active_state.ray.origin, plane_normal) / denom;
  if (t < 0)
    return src;

  auto distance = active_state.ray.point(t);
  ;

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
  return minalg::rigid_transform(src.orientation, src.position, scale);
}

minalg::float3 scaling_drag(drag_state *drag,
                            const gizmo_application_state &state,
                            bool local_toggle,
                            const std::shared_ptr<gizmo_component> &active,
                            const minalg::rigid_transform &src, bool uniform) {

  auto scale = src.scale;
  if (active) {
    minalg::rigid_transform _src{
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
                  const std::shared_ptr<gizmo_component> &active,
                  const minalg::float4x4 &modelMatrix) {
  std::vector<std::shared_ptr<gizmo_component>> draw_components{
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