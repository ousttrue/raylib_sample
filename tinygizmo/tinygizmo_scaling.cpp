#include "tinygizmo_scaling.h"
#include <assert.h>

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

void scaling_draw(const Float4x4 &modelMatrix,
                  const AddTriangleFunc &add_world_triangle,
                  GizmoComponentType active_component) {
  auto draw_components = {
      std::make_pair(GizmoComponentType::ScalingX, _scale_x),
      std::make_pair(GizmoComponentType::ScalingY, _scale_y),
      std::make_pair(GizmoComponentType::ScalingZ, _scale_z),
  };

  for (auto [component, mesh] : draw_components) {
    mesh.add_triangles(add_world_triangle, modelMatrix,
                       (component == active_component) ? mesh.base_color
                                                       : mesh.highlight_color);
  }
}

std::tuple<GizmoComponentType, float> scaling_intersect(const Ray &ray) {
  float best_t = std::numeric_limits<float>::infinity(), t;
  GizmoComponentType updated_state = {};
  if (ray.intersect_mesh(_scale_x, &t) && t < best_t) {
    updated_state = GizmoComponentType::ScalingX;
    ;
    best_t = t;
  }
  if (ray.intersect_mesh(_scale_y, &t) && t < best_t) {
    updated_state = GizmoComponentType::ScalingY;
    best_t = t;
  }
  if (ray.intersect_mesh(_scale_z, &t) && t < best_t) {
    updated_state = GizmoComponentType::ScalingZ;
    best_t = t;
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

static Transform axis_scale_dragger(DragState *drag,
                                    const FrameState &active_state,
                                    bool local_toggle, const Float3 &axis,
                                    const Transform &src, bool uniform) {
  if (!active_state.mouse_down) {
    return src;
  }

  auto plane_tangent =
      Float3::cross(axis, src.position - active_state.ray.origin);
  auto plane_normal = Float3::cross(axis, plane_tangent);

  // Define the plane to contain the original position of the object
  auto plane_point = src.position;

  // If an intersection exists between the ray and the plane, place the
  // object at that point
  auto denom = Float3::dot(active_state.ray.direction, plane_normal);
  if (std::abs(denom) == 0) {
    return src;
  }

  auto t =
      Float3::dot(plane_point - active_state.ray.origin, plane_normal) / denom;
  if (t < 0) {
    return src;
  }

  auto distance = active_state.ray.point(t);

  auto offset_on_axis = (distance - drag->click_offset).mult_each(axis);
  flush_to_zero(offset_on_axis);
  Float3 new_scale = drag->original_scale + offset_on_axis;

  Float3 scale =(uniform) 
    ? Float3{
      clamp(Float3::dot(distance, new_scale), 0.01f, 1000.f),
        clamp(Float3::dot(distance, new_scale), 0.01f, 1000.f),
        clamp(Float3::dot(distance, new_scale), 0.01f, 1000.f),
    } 
  : Float3{clamp(new_scale.x, 0.01f, 1000.f),
    clamp(new_scale.y, 0.01f, 1000.f),
    clamp(new_scale.z, 0.01f, 1000.f),};
  return Transform(src.orientation, src.position, scale);
}

Float3 scaling_drag(GizmoComponentType active_component,
                    const FrameState &state, bool local_toggle,
                    const Transform &src, bool uniform, DragState *drag) {
  auto scale = src.scale;
  Transform _src{
      .orientation = {0, 0, 0, 1},
      .position = src.position,
      .scale = scale,
  };
  switch (active_component) {
  case GizmoComponentType::ScalingX:
    scale =
        axis_scale_dragger(drag, state, local_toggle, {1, 0, 0}, _src, uniform)
            .scale;
    break;

  case GizmoComponentType::ScalingY:

    scale =
        axis_scale_dragger(drag, state, local_toggle, {0, 1, 0}, _src, uniform)
            .scale;
    break;

  case GizmoComponentType::ScalingZ:
    scale =
        axis_scale_dragger(drag, state, local_toggle, {0, 0, 1}, _src, uniform)
            .scale;
    break;

  default:
    assert(false);
    break;
  }

  return scale;
}

} // namespace tinygizmo
