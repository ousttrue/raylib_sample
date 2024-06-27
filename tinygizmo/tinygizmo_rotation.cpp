#include "tinygizmo_rotation.h"
#include "tinygizmo_geometrymesh.h"
#include <assert.h>
#include <optional>
#include <stdexcept>

namespace tinygizmo {

std::vector<Float2> ring_points = {
    {+0.025f, 1},    {-0.025f, 1},    {-0.025f, 1},    {-0.025f, 1.1f},
    {-0.025f, 1.1f}, {+0.025f, 1.1f}, {+0.025f, 1.1f}, {+0.025f, 1}};

auto _rotate_x = GeometryMesh::make_lathed_geometry(
    {1, 0, 0}, {0, 1, 0}, {0, 0, 1}, 32, ring_points,
    Float4{1, 0.5f, 0.5f, 1.f}, Float4{1, 0, 0, 1.f}, 0.003f);

auto _rotate_y = GeometryMesh::make_lathed_geometry(
    {0, 1, 0}, {0, 0, 1}, {1, 0, 0}, 32, ring_points,
    Float4{0.5f, 1, 0.5f, 1.f}, Float4{0, 1, 0, 1.f}, -0.003f);

auto _rotate_z = GeometryMesh::make_lathed_geometry(
    {0, 0, 1}, {1, 0, 0}, {0, 1, 0}, 32, ring_points,
    Float4{0.5f, 0.5f, 1, 1.f}, Float4{0, 0, 1, 1.f});

static auto _gizmo_components = {
    std::make_pair(RotationGizmo::GizmoComponentType::RotationY, _rotate_x),
    std::make_pair(RotationGizmo::GizmoComponentType::RotationY, _rotate_y),
    std::make_pair(RotationGizmo::GizmoComponentType::RotationZ, _rotate_z),
};

void rotation_draw(
    const Float4x4 &modelMatrix, const AddTriangleFunc &add_world_triangle,
    std::optional<RotationGizmo::GizmoComponentType> active_component) {

  // std::vector<std::shared_ptr<GizmoComponent>> draw_interactions;
  // if (!state.local_toggle && this->active)
  //   draw_interactions = {interaction_mode(this->active)};
  // else

  for (auto &[component, mesh] : _gizmo_components) {
    mesh.add_triangles(add_world_triangle, modelMatrix,
                       (component == active_component) ? mesh.base_color
                                                       : mesh.highlight_color);
  }

  //   Float4 orientation;
  //   // For non-local transformations, we only present one rotation ring
  //   // and draw an arrow from the center of the gizmo to indicate the degree
  //   // of rotation
  //   if (state.local_toggle == false && this->active) {
  //
  //     // Create orthonormal basis for drawing the arrow
  //     Float3 a = qrot(p.orientation, this->drag.click_offset -
  //                                                this->drag.original_position);
  //
  //     Float3 activeAxis;
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
  //     Float3 zDir = normalize(activeAxis),
  //                    xDir = normalize(cross(a, zDir)), yDir = cross(zDir,
  //                    xDir);
  //
  //     // Ad-hoc geometry
  //     std::initializer_list<Float2> arrow_points = {
  //         {0.0f, 0.f}, {0.0f, 0.05f}, {0.8f, 0.05f}, {0.9f, 0.10f}, {1.0f,
  //         0}};
  //     auto geo = make_lathed_geometry(yDir, xDir, zDir, 32, arrow_points);
  //
  //     add_triangles(add_world_triangle, modelMatrix, geo, Float4(1));
  //
  //     orientation = qmul(p.orientation, this->drag.original_orientation);
  //   } else if (state.local_toggle == true && this->active) {
  //     orientation = p.orientation;
  //   }
  //
}

std::tuple<std::optional<RotationGizmo::GizmoComponentType>, float>
rotation_intersect(const Ray &ray) {
  float best_t = std::numeric_limits<float>::infinity();
  std::optional<RotationGizmo::GizmoComponentType> updated_state = {};
  for (auto &[component, mesh] : _gizmo_components) {
    float t = mesh.intersect(ray);
    if (t < best_t) {
      updated_state = component;
      best_t = t;
    }
  }
  return {updated_state, best_t};
}

inline Quaternion
make_rotation_quat_between_vectors_snapped(const Float3 &from, const Float3 &to,
                                           const float angle) {
  auto a = from.normalize();
  auto b = to.normalize();
  auto snappedAcos = std::floor(std::acos(Float3::dot(a, b)) / angle) * angle;
  return Quaternion::from_axis_angle(Float3::cross(a, b).normalize(),
                                     snappedAcos);
}

static Float3 click_offset(const RayState &ray_state) {
  auto ray = ray_state.local_ray.scaling(ray_state.draw_scale);
  return ray_state.transform.transform_point(ray.origin +
                                             ray.direction.scale(ray_state.t));
}

static std::optional<Quaternion>
axis_rotation_dragger(const RayState &drag, const FrameState &active_state,
                      bool local_toggle, const Float3 &axis,
                      const Transform &src, bool) {
  auto start_orientation =
      local_toggle ? drag.transform.orientation : Quaternion{0, 0, 0, 1};

  assert(active_state.mouse_down);

  Transform original_pose = {
      start_orientation,
      drag.transform.position,
  };
  auto the_axis = original_pose.transform_vector(axis);
  auto the_plane =
      Plane::from_normal_and_position(the_axis, click_offset(drag));

  auto t = active_state.ray.intersect_plane(the_plane);
  if (!t) {
    return {};
  }

  auto center_of_rotation =
      drag.transform.position +
      the_axis.scale(
          Float3::dot(the_axis, click_offset(drag) - drag.transform.position));
  auto arm1 = (click_offset(drag) - center_of_rotation).normalize();
  auto arm2 = (active_state.ray.point(*t) - center_of_rotation).normalize();

  float d = Float3::dot(arm1, arm2);
  if (d > 0.999f) {
    return {};
  }

  float angle = std::acos(d);
  if (angle < 0.001f) {
    return {};
  }

  auto a = Float3::cross(arm1, arm2).normalize();
  return Quaternion::from_axis_angle(a, angle) * start_orientation;
}

std::optional<Quaternion>
rotation_drag(RotationGizmo::GizmoComponentType active_component,
              const FrameState &state, bool local_toggle, const Transform &src,
              const RayState &drag) {
  switch (active_component) {
  case RotationGizmo::GizmoComponentType::RotationX:
    return axis_rotation_dragger(drag, state, local_toggle, {1, 0, 0}, src, {});

  case RotationGizmo::GizmoComponentType::RotationY:
    return axis_rotation_dragger(drag, state, local_toggle, {0, 1, 0}, src, {});

  case RotationGizmo::GizmoComponentType::RotationZ:
    return axis_rotation_dragger(drag, state, local_toggle, {0, 0, 1}, src, {});

  default:
    assert(false);
    throw std::runtime_error("unknown rotation");
    ;
  }
}

} // namespace tinygizmo
