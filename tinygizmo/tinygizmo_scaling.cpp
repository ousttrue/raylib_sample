#include "tinygizmo_scaling.h"
#include <memory>

namespace tinygizmo {

template <typename T> T clamp(const T &val, const T &min, const T &max) {
  return std::min(std::max(val, min), max);
}

std::vector<Float2> mace_points = {{0.25f, 0}, {0.25f, 0.05f}, {1, 0.05f},
                                   {1, 0.1f},  {1.25f, 0.1f},  {1.25f, 0}};

auto _scale_x = std::make_shared<gizmo_component>(
    GeometryMesh::make_lathed_geometry({1, 0, 0}, {0, 1, 0}, {0, 0, 1}, 16,
                                       mace_points),
    Float4{1, 0.5f, 0.5f, 1.f}, Float4{1, 0, 0, 1.f});

auto _scale_y = std::make_shared<gizmo_component>(
    GeometryMesh::make_lathed_geometry({0, 1, 0}, {0, 0, 1}, {1, 0, 0}, 16,
                                       mace_points),
    Float4{0.5f, 1, 0.5f, 1.f}, Float4{0, 1, 0, 1.f});

auto _scale_z = std::make_shared<gizmo_component>(
    GeometryMesh::make_lathed_geometry({0, 0, 1}, {1, 0, 0}, {0, 1, 0}, 16,
                                       mace_points),
    Float4{0.5f, 0.5f, 1, 1.f}, Float4{0, 0, 1, 1.f});

std::tuple<std::shared_ptr<gizmo_component>, float>
scaling_intersect(const Ray &ray) {

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

inline void flush_to_zero(Float3 &f) {
  if (std::abs(f.x) < 0.02f)
    f.x = 0.f;
  if (std::abs(f.y) < 0.02f)
    f.y = 0.f;
  if (std::abs(f.z) < 0.02f)
    f.z = 0.f;
}

static RigidTransform axis_scale_dragger(DragState *drag,
                                         const FrameState &active_state,
                                         bool local_toggle, const Float3 &axis,
                                         const RigidTransform &src,
                                         bool uniform) {
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

  Float3 scale =
      (uniform) ? Float3(clamp(Float3::dot(distance, new_scale), 0.01f, 1000.f))
                : Float3(clamp(new_scale.x, 0.01f, 1000.f),
                         clamp(new_scale.y, 0.01f, 1000.f),
                         clamp(new_scale.z, 0.01f, 1000.f));
  return RigidTransform(src.orientation, src.position, scale);
}

Float3 scaling_drag(DragState *drag, const FrameState &state, bool local_toggle,
                    const std::shared_ptr<gizmo_component> &active,
                    const RigidTransform &src, bool uniform) {

  auto scale = src.scale;
  if (active) {
    RigidTransform _src{
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
                  const Float4x4 &modelMatrix) {
  std::vector<std::shared_ptr<gizmo_component>> draw_components{
      _scale_x,
      _scale_y,
      _scale_z,
  };

  for (auto c : draw_components) {
    c->mesh.add_triangles(add_world_triangle, modelMatrix,
                          (c == active) ? c->base_color : c->highlight_color);
  }
}

} // namespace tinygizmo
