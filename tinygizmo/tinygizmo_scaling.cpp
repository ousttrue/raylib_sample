#include "tinygizmo_scaling.h"
#include <assert.h>
#include <stdexcept>

namespace tinygizmo {

template <typename T> T clamp(const T &val, const T &min, const T &max) {
  return std::min(std::max(val, min), max);
}

std::vector<Float2> mace_points = {{0.25f, 0}, {0.25f, 0.05f}, {1, 0.05f},
                                   {1, 0.1f},  {1.25f, 0.1f},  {1.25f, 0}};

auto _scale_x = GeometryMesh::make_lathed_geometry(
    {1, 0, 0}, {0, 1, 0}, {0, 0, 1}, 16, mace_points,
    Float4{1, 0.5f, 0.5f, 1.f}, Float4{1, 0, 0, 1.f});

auto _scale_y = GeometryMesh::make_lathed_geometry(
    {0, 1, 0}, {0, 0, 1}, {1, 0, 0}, 16, mace_points,
    Float4{0.5f, 1, 0.5f, 1.f}, Float4{0, 1, 0, 1.f});

auto _scale_z = GeometryMesh::make_lathed_geometry(
    {0, 0, 1}, {1, 0, 0}, {0, 1, 0}, 16, mace_points,
    Float4{0.5f, 0.5f, 1, 1.f}, Float4{0, 0, 1, 1.f});

auto _gizmo_components = {
    std::make_pair(ScalingGizmo::GizmoComponentType::ScalingX, _scale_x),
    std::make_pair(ScalingGizmo::GizmoComponentType::ScalingY, _scale_y),
    std::make_pair(ScalingGizmo::GizmoComponentType::ScalingZ, _scale_z),
};

void scaling_draw(
    const Float4x4 &modelMatrix, const AddTriangleFunc &add_world_triangle,
    std::optional<ScalingGizmo::GizmoComponentType> active_component) {
  for (auto &[component, mesh] : _gizmo_components) {
    mesh.add_triangles(add_world_triangle, modelMatrix,
                       (component == active_component) ? mesh.base_color
                                                       : mesh.highlight_color);
  }
}

std::tuple<std::optional<ScalingGizmo::GizmoComponentType>, float>
scaling_intersect(const Ray &ray) {
  float best_t = std::numeric_limits<float>::infinity();
  std::optional<ScalingGizmo::GizmoComponentType> updated_state = {};
  for (auto &[component, mesh] : _gizmo_components) {
    float t;
    if (ray.intersect_mesh(mesh, &t) && t < best_t) {
      updated_state = component;
      best_t = t;
    }
  }
  return {updated_state, best_t};
}

inline void flush_to_zero(Float3 &f) {
  if (std::abs(f.x) < 0.02f)
    f.x = 0.f;
  if (std::abs(f.y) < 0.02f)
    f.y = 0.f;
  if (std::abs(f.z) < 0.02f)
    f.z = 0.f;
}

static Float3 click_offset(const RayState &ray_state) {
  auto ray = ray_state.local_ray.scaling(ray_state.draw_scale);
  return ray_state.transform.transform_point(ray.origin +
                                             ray.direction.scale(ray_state.t));
}

static std::optional<Float3>
axis_scale_dragger(const RayState &drag, const FrameState &active_state,
                   bool local_toggle, const Float3 &axis, const Transform &src,
                   bool uniform) {
  assert(active_state.mouse_down);

  auto plane_tangent =
      Float3::cross(axis, src.position - active_state.ray.origin);
  auto plane_normal = Float3::cross(axis, plane_tangent);

  // Define the plane to contain the original position of the object
  auto plane_point = src.position;

  // If an intersection exists between the ray and the plane, place the
  // object at that point
  auto denom = Float3::dot(active_state.ray.direction, plane_normal);
  if (std::abs(denom) == 0) {
    return {};
  }

  auto t =
      Float3::dot(plane_point - active_state.ray.origin, plane_normal) / denom;
  if (t < 0) {
    return {};
  }

  auto distance = active_state.ray.point(t);

  auto offset_on_axis = (distance - click_offset(drag)).mult_each(axis);
  flush_to_zero(offset_on_axis);
  Float3 new_scale = drag.transform.scale + offset_on_axis;

  Float3 scale =(uniform) 
    ? Float3{
      clamp(Float3::dot(distance, new_scale), 0.01f, 1000.f),
        clamp(Float3::dot(distance, new_scale), 0.01f, 1000.f),
        clamp(Float3::dot(distance, new_scale), 0.01f, 1000.f),
    } 
  : Float3{clamp(new_scale.x, 0.01f, 1000.f),
    clamp(new_scale.y, 0.01f, 1000.f),
    clamp(new_scale.z, 0.01f, 1000.f),};
  return scale;
}

std::optional<Float3>
scaling_drag(ScalingGizmo::GizmoComponentType active_component,
             const FrameState &state, bool local_toggle, const Transform &src,
             bool uniform, const RayState &drag) {
  // auto scale = src.scale;
  Transform _src{
      .orientation = {0, 0, 0, 1},
      .position = src.position,
      .scale = src.scale,
  };
  switch (active_component) {
  case ScalingGizmo::GizmoComponentType::ScalingX:
    return axis_scale_dragger(drag, state, local_toggle, {1, 0, 0}, _src,
                              uniform);

  case ScalingGizmo::GizmoComponentType::ScalingY:
    return axis_scale_dragger(drag, state, local_toggle, {0, 1, 0}, _src,
                              uniform);

  case ScalingGizmo::GizmoComponentType::ScalingZ:
    return axis_scale_dragger(drag, state, local_toggle, {0, 0, 1}, _src,
                              uniform);

  default:
    assert(false);
    throw std::runtime_error("unknown scaling");
  }
}

} // namespace tinygizmo
