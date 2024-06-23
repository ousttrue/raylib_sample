#include "tinygizmo_rotation.h"
#include <assert.h>

namespace tinygizmo {

std::vector<Float2> ring_points = {
    {+0.025f, 1},    {-0.025f, 1},    {-0.025f, 1},    {-0.025f, 1.1f},
    {-0.025f, 1.1f}, {+0.025f, 1.1f}, {+0.025f, 1.1f}, {+0.025f, 1}};

auto _rotate_x = std::make_shared<gizmo_component>(
    GeometryMesh::make_lathed_geometry({1, 0, 0}, {0, 1, 0}, {0, 0, 1}, 32,
                                       ring_points, 0.003f),
    Float4{1, 0.5f, 0.5f, 1.f}, Float4{1, 0, 0, 1.f});

auto _rotate_y = std::make_shared<gizmo_component>(
    GeometryMesh::make_lathed_geometry({0, 1, 0}, {0, 0, 1}, {1, 0, 0}, 32,
                                       ring_points, -0.003f),
    Float4{0.5f, 1, 0.5f, 1.f}, Float4{0, 1, 0, 1.f});

auto _rotate_z = std::make_shared<gizmo_component>(
    GeometryMesh::make_lathed_geometry({0, 0, 1}, {1, 0, 0}, {0, 1, 0}, 32,
                                       ring_points),
    Float4{0.5f, 0.5f, 1, 1.f}, Float4{0, 0, 1, 1.f});

std::tuple<std::shared_ptr<gizmo_component>, float>
rotation_intersect(const Ray &ray) {

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

inline Quaternion
make_rotation_quat_between_vectors_snapped(const Float3 &from, const Float3 &to,
                                           const float angle) {
  auto a = from.normalize();
  auto b = to.normalize();
  auto snappedAcos = std::floor(std::acos(Float3::dot(a, b)) / angle) * angle;
  return Quaternion::from_axis_angle(Float3::cross(a, b).normalize(),
                                     snappedAcos);
}

static RigidTransform axis_rotation_dragger(
    drag_state *drag, const gizmo_application_state &active_state,
    bool local_toggle, const Float3 &axis, const RigidTransform &src, bool);

Float4 rotation_drag(drag_state *drag, const gizmo_application_state &state,
                     bool local_toggle,
                     const std::shared_ptr<gizmo_component> &active,
                     const RigidTransform &src) {
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
                   const Float4x4 &modelMatrix) {

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

} // namespace tinygizmo
