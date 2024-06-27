#include "tinygizmo_translation.h"
#include "tinygizmo_geometrymesh.h"
#include <assert.h>
#include <optional>
#include <stdexcept>

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

static auto _gizmo_components = {
    std::make_pair(TranslationGizmo::GizmoComponentType::TranslationX,
                   _translate_x),
    std::make_pair(TranslationGizmo::GizmoComponentType::TranslationY,
                   _translate_y),
    std::make_pair(TranslationGizmo::GizmoComponentType::TranslationZ,
                   _translate_z),
    std::make_pair(TranslationGizmo::GizmoComponentType::TranslationYZ,
                   _translate_yz),
    std::make_pair(TranslationGizmo::GizmoComponentType::TranslationZX,
                   _translate_zx),
    std::make_pair(TranslationGizmo::GizmoComponentType::TranslationXY,
                   _translate_xy),
    std::make_pair(TranslationGizmo::GizmoComponentType::TranslationView,
                   _translate_xyz),
};

void position_draw(
    const Float4x4 &modelMatrix, const AddTriangleFunc &add_world_triangle,
    std::optional<TranslationGizmo::GizmoComponentType> active_component) {
  for (auto &[component, mesh] : _gizmo_components) {
    auto color = (component == active_component) ? mesh.base_color
                                                 : mesh.highlight_color;
    mesh.add_triangles(add_world_triangle, modelMatrix, color);
  }
}

std::tuple<std::optional<TranslationGizmo::GizmoComponentType>, float>
position_intersect(const Ray &ray) {
  float best_t = std::numeric_limits<float>::infinity();
  std::optional<TranslationGizmo::GizmoComponentType> updated_state = {};
  for (auto &[compoennt, mesh] : _gizmo_components) {
    float t = mesh.intersect(ray);
    if (t < best_t) {
      updated_state = compoennt;
      best_t = t;
    }
  }
  return {updated_state, best_t};
}

static Float3 click_offset(const RayState &ray_state) {
  auto ray = ray_state.local_ray.scaling(ray_state.draw_scale);
  //   DragState drag_state = {
  auto click_offset = ray.point(ray_state.t);
  //       .original_position = ray_state.transform.position,
  //   };
  if (ray_state.local_toggle) {
    click_offset = ray_state.gizmo_transform.transform_vector(click_offset);
  }
  //   return drag_state;
  return click_offset;
}

static std::optional<Float3> plane_drag(const Float3 &plane_normal,
                                        const RayState &drag,
                                        const FrameState &active_state,
                                        bool local_toggle, const Transform &p) {
  auto plane =
      Plane::from_normal_and_position(plane_normal, drag.transform.position);
  auto t = active_state.ray.intersect_plane(plane);
  if (!t) {
    return {};
  }

  return active_state.ray.point(*t) - click_offset(drag);
}

static std::optional<Float3> axis_drag(const Float3 &axis, const RayState &drag,
                                       const FrameState &active_state,
                                       bool local_toggle, const Transform &p) {
  // First apply a plane translation dragger with a plane that contains the
  // desired axis and is oriented to face the camera
  auto plane_tangent =
      Float3::cross(axis, p.position - active_state.ray.origin);
  auto plane_normal = Float3::cross(axis, plane_tangent);
  auto plane =
      Plane::from_normal_and_position(plane_normal, drag.transform.position);
  auto t = active_state.ray.intersect_plane(plane);
  if (!t) {
    return {};
  }
  auto dst = active_state.ray.point(*t);

  // Constrain object motion to be along the desired axis
  return drag.transform.position +
         axis.scale(Float3::dot(dst - drag.transform.position, axis)) -
         click_offset(drag);
}

std::optional<Float3>
position_drag(TranslationGizmo::GizmoComponentType active_component,
              const FrameState &state, bool local_toggle, const Transform &p,
              const RayState &drag) {

  switch (active_component) {
  case TranslationGizmo::GizmoComponentType::TranslationX: {
    auto axis = (local_toggle) ? p.orientation.xdir() : Float3{1, 0, 0};
    return axis_drag(axis, drag, state, local_toggle, p);
  }

  case TranslationGizmo::GizmoComponentType::TranslationY: {
    auto axis = (local_toggle) ? p.orientation.ydir() : Float3{0, 1, 0};
    return axis_drag(axis, drag, state, local_toggle, p);
  }

  case TranslationGizmo::GizmoComponentType::TranslationZ: {
    auto axis = (local_toggle) ? p.orientation.zdir() : Float3{0, 0, 1};
    return axis_drag(axis, drag, state, local_toggle, p);
  }

  case TranslationGizmo::GizmoComponentType::TranslationXY: {
    auto normal = (local_toggle) ? p.orientation.zdir() : Float3{0, 0, 1};
    return plane_drag(normal, drag, state, local_toggle, p);
  }

  case TranslationGizmo::GizmoComponentType::TranslationYZ: {
    auto normal = (local_toggle) ? p.orientation.xdir() : Float3{1, 0, 0};
    return plane_drag(normal, drag, state, local_toggle, p);
  }

  case TranslationGizmo::GizmoComponentType::TranslationZX: {
    auto normal = (local_toggle) ? p.orientation.ydir() : Float3{0, 1, 0};
    return plane_drag(normal, drag, state, local_toggle, p);
  }

  case TranslationGizmo::GizmoComponentType::TranslationView: {
    auto normal = -state.cam_orientation.zdir();
    return plane_drag(normal, drag, state, local_toggle, p);
  }

  default:
    assert(false);
    throw std::runtime_error("unknown translation");
  }
}

} // namespace tinygizmo
