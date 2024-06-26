#include "tinygizmo_translation.h"
#include <assert.h>
#include <optional>

namespace tinygizmo {

std::vector<Float2> arrow_points = {
    {0.25f, 0}, {0.25f, 0.05f}, {1, 0.05f}, {1, 0.10f}, {1.2f, 0}};

auto _translate_x = GeometryMesh::make_lathed_geometry(
    {1, 0, 0}, {0, 1, 0}, {0, 0, 1}, 16, arrow_points,
    Float4{1, 0.5f, 0.5f, 1.f}, Float4{1, 0, 0, 1.f});
auto _translate_y = GeometryMesh::make_lathed_geometry(
    {0, 1, 0}, {0, 0, 1}, {1, 0, 0}, 16, arrow_points,
    Float4{0.5f, 1, 0.5f, 1.f}, Float4{0, 1, 0, 1.f});
auto _translate_z = GeometryMesh::make_lathed_geometry(
    {0, 0, 1}, {1, 0, 0}, {0, 1, 0}, 16, arrow_points,
    Float4{0.5f, 0.5f, 1, 1.f}, Float4{0, 0, 1, 1.f});
auto _translate_yz = GeometryMesh::make_box_geometry(
    {-0.01f, 0.25, 0.25}, {0.01f, 0.75f, 0.75f}, Float4{0.5f, 1, 1, 0.5f},
    Float4{0, 1, 1, 0.6f});
auto _translate_zx = GeometryMesh::make_box_geometry(
    {0.25, -0.01f, 0.25}, {0.75f, 0.01f, 0.75f}, Float4{1, 0.5f, 1, 0.5f},
    Float4{1, 0, 1, 0.6f});
auto _translate_xy = GeometryMesh::make_box_geometry(
    {0.25, 0.25, -0.01f}, {0.75f, 0.75f, 0.01f}, Float4{1, 1, 0.5f, 0.5f},
    Float4{1, 1, 0, 0.6f});
auto _translate_xyz = GeometryMesh::make_box_geometry(
    {-0.05f, -0.05f, -0.05f}, {0.05f, 0.05f, 0.05f},
    Float4{0.9f, 0.9f, 0.9f, 0.25f}, Float4{1, 1, 1, 0.35f});

auto _gizmo_components = {
    std::make_pair(GizmoComponentType::TranslationX, _translate_x),
    std::make_pair(GizmoComponentType::TranslationY, _translate_y),
    std::make_pair(GizmoComponentType::TranslationZ, _translate_z),
    std::make_pair(GizmoComponentType::TranslationYZ, _translate_yz),
    std::make_pair(GizmoComponentType::TranslationZX, _translate_zx),
    std::make_pair(GizmoComponentType::TranslationXY, _translate_xy),
    std::make_pair(GizmoComponentType::TranslationView, _translate_xyz),
};

void position_draw(const Float4x4 &modelMatrix,
                   const AddTriangleFunc &add_world_triangle,
                   GizmoComponentType active_component) {
  for (auto [component, mesh] : _gizmo_components) {
    auto color = (component == active_component) ? mesh.base_color
                                                 : mesh.highlight_color;
    mesh.add_triangles(add_world_triangle, modelMatrix, color);
  }
}

std::tuple<GizmoComponentType, float> position_intersect(const Ray &ray) {
  float best_t = std::numeric_limits<float>::infinity();
  GizmoComponentType updated_state = {};
  float t;
  for (auto [compoennt, mesh] : _gizmo_components) {
    if (ray.intersect_mesh(mesh, &t) && t < best_t) {
      updated_state = compoennt;
      best_t = t;
    }
  }
  return {updated_state, best_t};
}

static std::optional<Transform> plane_drag(const Float3 &plane_normal,
                                           DragState *drag,
                                           const FrameState &active_state,
                                           bool local_toggle,
                                           const Transform &p) {
  auto plane =
      Plane::from_normal_and_position(plane_normal, drag->original_position);
  auto t = active_state.ray.intersect_plane(plane);
  if (!t) {
    return {};
  }
  auto dst = active_state.ray.point(*t) - drag->click_offset;

  return Transform{
      .orientation = p.orientation,
      .position = dst,
      .scale = p.scale,
  };
}

static std::optional<Transform> axis_drag(const Float3 &axis, DragState *drag,
                                          const FrameState &active_state,
                                          bool local_toggle,
                                          const Transform &p) {
  // First apply a plane translation dragger with a plane that contains the
  // desired axis and is oriented to face the camera
  auto plane_tangent =
      Float3::cross(axis, p.position - active_state.ray.origin);
  auto plane_normal = Float3::cross(axis, plane_tangent);
  auto plane =
      Plane::from_normal_and_position(plane_normal, drag->original_position);
  auto t = active_state.ray.intersect_plane(plane);
  if (!t) {
    return {};
  }
  auto dst = active_state.ray.point(*t);

  // Constrain object motion to be along the desired axis
  auto point = drag->original_position +
               axis.scale(Float3::dot(dst - drag->original_position, axis)) -
               drag->click_offset;
  return Transform(p.orientation, point, p.scale);
}

Float3 position_drag(GizmoComponentType active_component,
                     const FrameState &state, bool local_toggle,
                     const Transform &p, DragState *drag) {

  switch (active_component) {
  case GizmoComponentType::TranslationX: {
    auto axis = (local_toggle) ? p.orientation.xdir() : Float3{1, 0, 0};
    if (auto dst = axis_drag(axis, drag, state, local_toggle, p)) {
      return dst->position;
    }
  }
  case GizmoComponentType::TranslationY: {
    auto axis = (local_toggle) ? p.orientation.ydir() : Float3{0, 1, 0};
    if (auto dst = axis_drag(axis, drag, state, local_toggle, p)) {
      return dst->position;
    }
  }
  case GizmoComponentType::TranslationZ: {
    auto axis = (local_toggle) ? p.orientation.zdir() : Float3{0, 0, 1};
    if (auto dst = axis_drag(axis, drag, state, local_toggle, p)) {
      return dst->position;
    }
  }
  case GizmoComponentType::TranslationXY: {
    auto normal = (local_toggle) ? p.orientation.zdir() : Float3{0, 0, 1};
    if (auto dst = plane_drag(normal, drag, state, local_toggle, p)) {
      return dst->position;
    }
  }
  case GizmoComponentType::TranslationYZ: {
    auto normal = (local_toggle) ? p.orientation.xdir() : Float3{1, 0, 0};
    if (auto dst = plane_drag(normal, drag, state, local_toggle, p)) {
      return dst->position;
    }
  }
  case GizmoComponentType::TranslationZX: {
    auto normal = (local_toggle) ? p.orientation.ydir() : Float3{0, 1, 0};
    if (auto dst = plane_drag(normal, drag, state, local_toggle, p)) {
      return dst->position;
    }
  }
  case GizmoComponentType::TranslationView: {
    auto normal = -state.cam_orientation.zdir();
    if (auto dst = plane_drag(normal, drag, state, local_toggle, p)) {
      return dst->position;
    }
  }
  default:
    assert(false);
  }

  return p.position;
}

} // namespace tinygizmo
