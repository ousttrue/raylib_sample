#include "interaction_state.hpp"
#include <memory>

namespace tinygizmo {

enum class interact {
  none,
  translate_x,
  translate_y,
  translate_z,
  translate_yz,
  translate_zx,
  translate_xy,
  translate_xyz,
  rotate_x,
  rotate_y,
  rotate_z,
  scale_x,
  scale_y,
  scale_z,
  scale_xyz,
};

std::vector<minalg::float2> arrow_points = {
    {0.25f, 0}, {0.25f, 0.05f}, {1, 0.05f}, {1, 0.10f}, {1.2f, 0}};
std::vector<minalg::float2> mace_points = {{0.25f, 0},    {0.25f, 0.05f},
                                           {1, 0.05f},    {1, 0.1f},
                                           {1.25f, 0.1f}, {1.25f, 0}};
std::vector<minalg::float2> ring_points = {
    {+0.025f, 1},    {-0.025f, 1},    {-0.025f, 1},    {-0.025f, 1.1f},
    {-0.025f, 1.1f}, {+0.025f, 1.1f}, {+0.025f, 1.1f}, {+0.025f, 1}};

std::unordered_map<interact, std::shared_ptr<gizmo_mesh_component>>
    mesh_components = {
        {
            interact::translate_x,
            std::make_shared<gizmo_mesh_component>(
                make_lathed_geometry({1, 0, 0}, {0, 1, 0}, {0, 0, 1}, 16,
                                     arrow_points),
                minalg::float4{1, 0.5f, 0.5f, 1.f},
                minalg::float4{1, 0, 0, 1.f}),
        },
        {
            interact::translate_y,
            std::make_shared<gizmo_mesh_component>(
                make_lathed_geometry({0, 1, 0}, {0, 0, 1}, {1, 0, 0}, 16,
                                     arrow_points),
                minalg::float4{0.5f, 1, 0.5f, 1.f},
                minalg::float4{0, 1, 0, 1.f}),
        },
        {
            interact::translate_z,
            std::make_shared<gizmo_mesh_component>(
                make_lathed_geometry({0, 0, 1}, {1, 0, 0}, {0, 1, 0}, 16,
                                     arrow_points),
                minalg::float4{0.5f, 0.5f, 1, 1.f},
                minalg::float4{0, 0, 1, 1.f}),
        },
        {
            interact::translate_yz,
            std::make_shared<gizmo_mesh_component>(
                make_box_geometry({-0.01f, 0.25, 0.25}, {0.01f, 0.75f, 0.75f}),
                minalg::float4{0.5f, 1, 1, 0.5f},
                minalg::float4{0, 1, 1, 0.6f}),
        },
        {
            interact::translate_zx,
            std::make_shared<gizmo_mesh_component>(
                make_box_geometry({0.25, -0.01f, 0.25}, {0.75f, 0.01f, 0.75f}),
                minalg::float4{1, 0.5f, 1, 0.5f},
                minalg::float4{1, 0, 1, 0.6f}),
        },
        {
            interact::translate_xy,
            std::make_shared<gizmo_mesh_component>(
                make_box_geometry({0.25, 0.25, -0.01f}, {0.75f, 0.75f, 0.01f}),
                minalg::float4{1, 1, 0.5f, 0.5f},
                minalg::float4{1, 1, 0, 0.6f}),
        },
        {
            interact::translate_xyz,
            std::make_shared<gizmo_mesh_component>(
                make_box_geometry({-0.05f, -0.05f, -0.05f},
                                  {0.05f, 0.05f, 0.05f}),
                minalg::float4{0.9f, 0.9f, 0.9f, 0.25f},
                minalg::float4{1, 1, 1, 0.35f}),

        },
        {
            interact::rotate_x,
            std::make_shared<gizmo_mesh_component>(
                make_lathed_geometry({1, 0, 0}, {0, 1, 0}, {0, 0, 1}, 32,
                                     ring_points, 0.003f),
                minalg::float4{1, 0.5f, 0.5f, 1.f},
                minalg::float4{1, 0, 0, 1.f}),
        },
        {
            interact::rotate_y,
            std::make_shared<gizmo_mesh_component>(
                make_lathed_geometry({0, 1, 0}, {0, 0, 1}, {1, 0, 0}, 32,
                                     ring_points, -0.003f),
                minalg::float4{0.5f, 1, 0.5f, 1.f},
                minalg::float4{0, 1, 0, 1.f}),
        },
        {
            interact::rotate_z,
            std::make_shared<gizmo_mesh_component>(
                make_lathed_geometry({0, 0, 1}, {1, 0, 0}, {0, 1, 0}, 32,
                                     ring_points),
                minalg::float4{0.5f, 0.5f, 1, 1.f},
                minalg::float4{0, 0, 1, 1.f}),
        },
        {
            interact::scale_x,
            std::make_shared<gizmo_mesh_component>(
                make_lathed_geometry({1, 0, 0}, {0, 1, 0}, {0, 0, 1}, 16,
                                     mace_points),
                minalg::float4{1, 0.5f, 0.5f, 1.f},
                minalg::float4{1, 0, 0, 1.f}),
        },
        {
            interact::scale_y,
            std::make_shared<gizmo_mesh_component>(
                make_lathed_geometry({0, 1, 0}, {0, 0, 1}, {1, 0, 0}, 16,
                                     mace_points),
                minalg::float4{0.5f, 1, 0.5f, 1.f},
                minalg::float4{0, 1, 0, 1.f}),
        },
        {
            interact::scale_z,
            std::make_shared<gizmo_mesh_component>(
                make_lathed_geometry({0, 0, 1}, {1, 0, 0}, {0, 1, 0}, 16,
                                     mace_points),
                minalg::float4{0.5f, 0.5f, 1, 1.f},
                minalg::float4{0, 0, 1, 1.f}),
        },
};
inline interact
interaction_mode(const std::shared_ptr<gizmo_mesh_component> &p) {
  if (!p) {
    return interact::none;
  }
  for (auto &kv : mesh_components) {
    if (kv.second == p) {
      return kv.first;
    }
  }
  assert(false);
  return {};
}

// This will calculate a scale constant based on the number of screenspace
// pixels passed as pixel_scale.
static float scale_screenspace(const gizmo_state &state,
                               const minalg::float3 position,
                               const float pixel_scale) {
  float dist = length(position - to_minalg(state.active_state.ray_origin));
  return std::tan(state.active_state.cam_yfov) * dist *
         (pixel_scale / state.active_state.viewport_size[1]);
}

static void add_triangles(const AddTriangleFunc &add_triangle,
                          const minalg::float4x4 &modelMatrix,
                          const geometry_mesh &mesh,
                          const minalg::float4 &color) {
  for (auto &t : mesh.triangles) {
    auto v0 = mesh.vertices[t.x];
    auto v1 = mesh.vertices[t.y];
    auto v2 = mesh.vertices[t.z];
    auto p0 = transform_coord(
        modelMatrix,
        v0.position); // transform local coordinates into worldspace
    auto p1 = transform_coord(
        modelMatrix,
        v1.position); // transform local coordinates into worldspace
    auto p2 = transform_coord(
        modelMatrix,
        v2.position); // transform local coordinates into worldspace
    add_triangle({color.x, color.y, color.z, color.w}, {p0.x, p0.y, p0.z},
                 {p1.x, p1.y, p1.z}, {p2.x, p2.y, p2.z});
  }
}

gizmo_result interaction_state::position_gizmo(
    const gizmo_state &state, const AddTriangleFunc &add_world_triangle,
    const minalg::float4 &rotation, const minalg::float3 &_position) {

  const float draw_scale =
      (state.active_state.screenspace_scale > 0.f)
          ? scale_screenspace(state, _position,
                              state.active_state.screenspace_scale)
          : 1.f;

  // interaction_mode will only change on clicked
  if (state.has_clicked) {
    this->active = nullptr;
  }

  rigid_transform p = rigid_transform(
      state.local_toggle ? rotation : minalg::float4(0, 0, 0, 1), _position);

  auto ray = detransform(p, {
                                to_minalg(state.active_state.ray_origin),
                                to_minalg(state.active_state.ray_direction),
                            });
  ray = ray.descale(draw_scale);

  float best_t = std::numeric_limits<float>::infinity(), t;
  std::shared_ptr<gizmo_mesh_component> updated_state = {};
  if (intersect_ray_mesh(ray, mesh_components[interact::translate_x]->mesh,
                         &t) &&
      t < best_t) {
    updated_state = mesh_components[interact::translate_x];
    best_t = t;
  }
  if (intersect_ray_mesh(ray, mesh_components[interact::translate_y]->mesh,
                         &t) &&
      t < best_t) {
    updated_state = mesh_components[interact::translate_y];
    best_t = t;
  }
  if (intersect_ray_mesh(ray, mesh_components[interact::translate_z]->mesh,
                         &t) &&
      t < best_t) {
    updated_state = mesh_components[interact::translate_z];
    best_t = t;
  }
  if (intersect_ray_mesh(ray, mesh_components[interact::translate_yz]->mesh,
                         &t) &&
      t < best_t) {
    updated_state = mesh_components[interact::translate_yz];
    best_t = t;
  }
  if (intersect_ray_mesh(ray, mesh_components[interact::translate_zx]->mesh,
                         &t) &&
      t < best_t) {
    updated_state = mesh_components[interact::translate_zx];
    best_t = t;
  }
  if (intersect_ray_mesh(ray, mesh_components[interact::translate_xy]->mesh,
                         &t) &&
      t < best_t) {
    updated_state = mesh_components[interact::translate_xy];
    best_t = t;
  }
  if (intersect_ray_mesh(ray, mesh_components[interact::translate_xyz]->mesh,
                         &t) &&
      t < best_t) {
    updated_state = mesh_components[interact::translate_xyz];
    best_t = t;
  }

  if (state.has_clicked) {
    this->active = updated_state;
    if (this->active) {
      ray = ray.scaling(draw_scale);
      this->drag.click_offset =
          state.local_toggle
              ? p.transform_vector(ray.origin + ray.direction * t)
              : ray.origin + ray.direction * t;
    }
  }

  auto hover =
      (best_t == std::numeric_limits<float>::infinity()) ? false : true;

  std::vector<minalg::float3> axes;
  if (state.local_toggle)
    axes = {qxdir(p.orientation), qydir(p.orientation), qzdir(p.orientation)};
  else
    axes = {{1, 0, 0}, {0, 1, 0}, {0, 0, 1}};

  minalg::float3 position = _position;
  if (this->active) {
    position += this->drag.click_offset;
    switch (interaction_mode(this->active)) {
    case interact::translate_x:
      axis_translation_dragger(&drag, state, axes[0], position);
      break;
    case interact::translate_y:
      axis_translation_dragger(&drag, state, axes[1], position);
      break;
    case interact::translate_z:
      axis_translation_dragger(&drag, state, axes[2], position);
      break;
    case interact::translate_yz:
      plane_translation_dragger(&drag, state, axes[0], position);
      break;
    case interact::translate_zx:
      plane_translation_dragger(&drag, state, axes[1], position);
      break;
    case interact::translate_xy:
      plane_translation_dragger(&drag, state, axes[2], position);
      break;
    case interact::translate_xyz:
      plane_translation_dragger(
          &drag, state,
          -minalg::qzdir(to_minalg(state.active_state.cam_orientation)),
          position);
      break;
    default:
      assert(false);
      break;
    }
    position -= this->drag.click_offset;
  }

  if (state.has_released) {
    this->active = nullptr;
  }

  std::vector<interact> draw_interactions{
      interact::translate_x,  interact::translate_y,  interact::translate_z,
      interact::translate_yz, interact::translate_zx, interact::translate_xy,
      interact::translate_xyz};

  minalg::float4x4 modelMatrix = p.matrix();
  minalg::float4x4 scaleMatrix = scaling_matrix(minalg::float3(draw_scale));
  modelMatrix = mul(modelMatrix, scaleMatrix);

  for (auto c : draw_interactions) {
    auto mesh = mesh_components[c]->mesh;
    auto color = (c == interaction_mode(this->active))
                     ? mesh_components[c]->base_color
                     : mesh_components[c]->highlight_color;
    add_triangles(add_world_triangle, modelMatrix, mesh, color);
  }

  return {.hover = hover,
          .active = this->active ? true : false,
          .t = {
              position.x,
              position.y,
              position.z,
          }};
}

gizmo_result interaction_state::rotation_gizmo(
    const gizmo_state &state, const AddTriangleFunc &add_world_triangle,
    const minalg::float3 &center, const minalg::float4 &_orientation) {
  assert(length2(_orientation) > float(1e-6));

  rigid_transform p = rigid_transform(
      state.local_toggle ? _orientation : minalg::float4(0, 0, 0, 1),
      center); // Orientation is local by default
  const float draw_scale =
      (state.active_state.screenspace_scale > 0.f)
          ? scale_screenspace(state, p.position,
                              state.active_state.screenspace_scale)
          : 1.f;

  // interaction_mode will only change on clicked
  if (state.has_clicked) {
    this->active = nullptr;
  }

  {

    auto ray = detransform(p, {
                                  to_minalg(state.active_state.ray_origin),
                                  to_minalg(state.active_state.ray_direction),
                              });
    ray = ray.descale(draw_scale);
    float best_t = std::numeric_limits<float>::infinity(), t;

    std::shared_ptr<gizmo_mesh_component> updated_state = {};
    if (intersect_ray_mesh(ray, mesh_components[interact::rotate_x]->mesh,
                           &t) &&
        t < best_t) {
      updated_state = mesh_components[interact::rotate_x];
      best_t = t;
    }
    if (intersect_ray_mesh(ray, mesh_components[interact::rotate_y]->mesh,
                           &t) &&
        t < best_t) {
      updated_state = mesh_components[interact::rotate_y];
      best_t = t;
    }
    if (intersect_ray_mesh(ray, mesh_components[interact::rotate_z]->mesh,
                           &t) &&
        t < best_t) {
      updated_state = mesh_components[interact::rotate_z];
      best_t = t;
    }

    if (state.has_clicked) {
      this->active = updated_state;
      if (this->active) {
        ray = ray.scaling(draw_scale);
        this->drag.original_position = center;
        this->drag.original_orientation = _orientation;
        this->drag.click_offset =
            p.transform_point(ray.origin + ray.direction * t);
      }
    }
  }

  minalg::float3 activeAxis;
  if (this->active) {
    const minalg::float4 starting_orientation =
        state.local_toggle ? this->drag.original_orientation
                           : minalg::float4(0, 0, 0, 1);
    switch (interaction_mode(this->active)) {
    case interact::rotate_x:
      axis_rotation_dragger(&drag, state, {1, 0, 0}, center,
                            starting_orientation, p.orientation);
      activeAxis = {1, 0, 0};
      break;
    case interact::rotate_y:
      axis_rotation_dragger(&drag, state, {0, 1, 0}, center,
                            starting_orientation, p.orientation);
      activeAxis = {0, 1, 0};
      break;
    case interact::rotate_z:
      axis_rotation_dragger(&drag, state, {0, 0, 1}, center,
                            starting_orientation, p.orientation);
      activeAxis = {0, 0, 1};
      break;
    default:
      assert(false);
      break;
    }
  }

  if (state.has_released) {
    this->active = nullptr;
  }

  minalg::float4x4 modelMatrix = p.matrix();
  minalg::float4x4 scaleMatrix = scaling_matrix(minalg::float3(draw_scale));
  modelMatrix = mul(modelMatrix, scaleMatrix);

  std::vector<interact> draw_interactions;
  if (!state.local_toggle && this->active)
    draw_interactions = {interaction_mode(this->active)};
  else
    draw_interactions = {interact::rotate_x, interact::rotate_y,
                         interact::rotate_z};

  for (auto c : draw_interactions) {
    add_triangles(add_world_triangle, modelMatrix, mesh_components[c]->mesh,
                  (c == interaction_mode(this->active))
                      ? mesh_components[c]->base_color
                      : mesh_components[c]->highlight_color);
  }

  auto orientation = _orientation;
  // For non-local transformations, we only present one rotation ring
  // and draw an arrow from the center of the gizmo to indicate the degree
  // of rotation
  if (state.local_toggle == false && this->active) {

    // Create orthonormal basis for drawing the arrow
    minalg::float3 a = qrot(p.orientation, this->drag.click_offset -
                                               this->drag.original_position);
    minalg::float3 zDir = normalize(activeAxis),
                   xDir = normalize(cross(a, zDir)), yDir = cross(zDir, xDir);

    // Ad-hoc geometry
    std::initializer_list<minalg::float2> arrow_points = {
        {0.0f, 0.f}, {0.0f, 0.05f}, {0.8f, 0.05f}, {0.9f, 0.10f}, {1.0f, 0}};
    auto geo = make_lathed_geometry(yDir, xDir, zDir, 32, arrow_points);

    add_triangles(add_world_triangle, modelMatrix, geo, minalg::float4(1));

    orientation = qmul(p.orientation, this->drag.original_orientation);
  } else if (state.local_toggle == true && this->active) {
    orientation = p.orientation;
  }

  return {.hover = false,
          .active = this->active ? true : false,
          .r = {
              orientation.x,
              orientation.y,
              orientation.z,
              orientation.w,
          }};
}

gizmo_result interaction_state::scale_gizmo(
    const gizmo_state &state, const AddTriangleFunc &add_world_triangle,
    const minalg::float4 &orientation, const minalg::float3 &center,
    const minalg::float3 &_scale, bool uniform) {
  rigid_transform p = rigid_transform(orientation, center);
  const float draw_scale =
      (state.active_state.screenspace_scale > 0.f)
          ? scale_screenspace(state, p.position,
                              state.active_state.screenspace_scale)
          : 1.f;

  if (state.has_clicked) {
    this->active = nullptr;
  }

  {
    auto ray = detransform(p, {
                                  to_minalg(state.active_state.ray_origin),
                                  to_minalg(state.active_state.ray_direction),
                              });
    ray = ray.descale(draw_scale);
    float best_t = std::numeric_limits<float>::infinity(), t;
    std::shared_ptr<gizmo_mesh_component> updated_state = {};
    if (intersect_ray_mesh(ray, mesh_components[interact::scale_x]->mesh, &t) &&
        t < best_t) {
      updated_state = mesh_components[interact::scale_x];
      best_t = t;
    }
    if (intersect_ray_mesh(ray, mesh_components[interact::scale_y]->mesh, &t) &&
        t < best_t) {
      updated_state = mesh_components[interact::scale_y];
      best_t = t;
    }
    if (intersect_ray_mesh(ray, mesh_components[interact::scale_z]->mesh, &t) &&
        t < best_t) {
      updated_state = mesh_components[interact::scale_z];
      best_t = t;
    }

    if (state.has_clicked) {
      this->active = updated_state;
      if (this->active) {
        ray = ray.scaling(draw_scale);
        this->drag.original_scale = _scale;
        this->drag.click_offset =
            p.transform_point(ray.origin + ray.direction * t);
      }
    }
  }

  if (state.has_released) {
    this->active = nullptr;
  }

  auto scale = _scale;
  if (this->active) {
    switch (interaction_mode(this->active)) {
    case interact::scale_x:
      axis_scale_dragger(&drag, state, {1, 0, 0}, center, scale, uniform);
      break;
    case interact::scale_y:
      axis_scale_dragger(&drag, state, {0, 1, 0}, center, scale, uniform);
      break;
    case interact::scale_z:
      axis_scale_dragger(&drag, state, {0, 0, 1}, center, scale, uniform);
      break;
    default:
      assert(false);
      break;
    }
  }

  minalg::float4x4 modelMatrix = p.matrix();
  minalg::float4x4 scaleMatrix = scaling_matrix(minalg::float3(draw_scale));
  modelMatrix = mul(modelMatrix, scaleMatrix);

  std::vector<interact> draw_components{interact::scale_x, interact::scale_y,
                                        interact::scale_z};

  for (auto c : draw_components) {
    add_triangles(add_world_triangle, modelMatrix, mesh_components[c]->mesh,
                  (c == interaction_mode(this->active))
                      ? mesh_components[c]->base_color
                      : mesh_components[c]->highlight_color);
  }

  return {.hover = false,
          .active = this->active ? true : false,
          .s = {
              scale.x,
              scale.y,
              scale.z,
          }};
}

} // namespace tinygizmo
