// This is free and unencumbered software released into the public domain.
// For more information, please refer to <http://unlicense.org>

#include "tiny-gizmo.hpp"
#include "minalg.hpp"

#include <assert.h>
#include <map>
#include <memory>
#include <string>
#include <vector>

namespace tinygizmo {

minalg::float3 to_minalg(const float3 &v) { return {v.x, v.y, v.z}; }
minalg::float4 to_minalg(const float4 &v) { return {v.x, v.y, v.z, v.w}; }

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

struct interaction_state {
  bool active{
      false}; // Flag to indicate if the gizmo is being actively manipulated
  bool hover{false}; // Flag to indicate if the gizmo is being hovered
  minalg::float3 original_position;    // Original position of an object being
                                       // manipulated with a gizmo
  minalg::float4 original_orientation; // Original orientation of an object
                                       // being manipulated with a gizmo
  minalg::float3 original_scale;       // Original scale of an object being
                                       // manipulated with a gizmo
  minalg::float3 click_offset; // Offset from position of grabbed object to
                               // coordinates of clicked point
  interact interaction_mode;   // Currently active component
};

//////////////////////////////////
// Gizmo Context Implementation //
//////////////////////////////////
struct gizmo_context_impl {
  gizmo_context *ctx;

  std::vector<draw_vertex> vertices;
  std::vector<uint32_t> indices;

  std::map<interact, gizmo_mesh_component> mesh_components;
  std::vector<gizmo_renderable> drawlist;

  std::map<uint32_t, interaction_state> gizmos;

  gizmo_application_state active_state;
  gizmo_application_state last_state;
  bool local_toggle{
      true}; // State to describe if the gizmo should use transform-local math
  bool has_clicked{false}; // State to describe if the user has pressed the left
                           // mouse button during the last frame
  bool has_released{false}; // State to describe if the user has released the
                            // left mouse button during the last frame

  // Public methods
  gizmo_context_impl(gizmo_context *ctx) : ctx(ctx) {
    std::vector<minalg::float2> arrow_points = {
        {0.25f, 0}, {0.25f, 0.05f}, {1, 0.05f}, {1, 0.10f}, {1.2f, 0}};
    std::vector<minalg::float2> mace_points = {{0.25f, 0},    {0.25f, 0.05f},
                                               {1, 0.05f},    {1, 0.1f},
                                               {1.25f, 0.1f}, {1.25f, 0}};
    std::vector<minalg::float2> ring_points = {
        {+0.025f, 1},    {-0.025f, 1},    {-0.025f, 1},    {-0.025f, 1.1f},
        {-0.025f, 1.1f}, {+0.025f, 1.1f}, {+0.025f, 1.1f}, {+0.025f, 1}};
    mesh_components[interact::translate_x] = {
        make_lathed_geometry({1, 0, 0}, {0, 1, 0}, {0, 0, 1}, 16, arrow_points),
        {1, 0.5f, 0.5f, 1.f},
        {1, 0, 0, 1.f}};
    mesh_components[interact::translate_y] = {
        make_lathed_geometry({0, 1, 0}, {0, 0, 1}, {1, 0, 0}, 16, arrow_points),
        {0.5f, 1, 0.5f, 1.f},
        {0, 1, 0, 1.f}};
    mesh_components[interact::translate_z] = {
        make_lathed_geometry({0, 0, 1}, {1, 0, 0}, {0, 1, 0}, 16, arrow_points),
        {0.5f, 0.5f, 1, 1.f},
        {0, 0, 1, 1.f}};
    mesh_components[interact::translate_yz] = {
        make_box_geometry({-0.01f, 0.25, 0.25}, {0.01f, 0.75f, 0.75f}),
        {0.5f, 1, 1, 0.5f},
        {0, 1, 1, 0.6f}};
    mesh_components[interact::translate_zx] = {
        make_box_geometry({0.25, -0.01f, 0.25}, {0.75f, 0.01f, 0.75f}),
        {1, 0.5f, 1, 0.5f},
        {1, 0, 1, 0.6f}};
    mesh_components[interact::translate_xy] = {
        make_box_geometry({0.25, 0.25, -0.01f}, {0.75f, 0.75f, 0.01f}),
        {1, 1, 0.5f, 0.5f},
        {1, 1, 0, 0.6f}};
    mesh_components[interact::translate_xyz] = {
        make_box_geometry({-0.05f, -0.05f, -0.05f}, {0.05f, 0.05f, 0.05f}),
        {0.9f, 0.9f, 0.9f, 0.25f},
        {1, 1, 1, 0.35f}};
    mesh_components[interact::rotate_x] = {
        make_lathed_geometry({1, 0, 0}, {0, 1, 0}, {0, 0, 1}, 32, ring_points,
                             0.003f),
        {1, 0.5f, 0.5f, 1.f},
        {1, 0, 0, 1.f}};
    mesh_components[interact::rotate_y] = {
        make_lathed_geometry({0, 1, 0}, {0, 0, 1}, {1, 0, 0}, 32, ring_points,
                             -0.003f),
        {0.5f, 1, 0.5f, 1.f},
        {0, 1, 0, 1.f}};
    mesh_components[interact::rotate_z] = {
        make_lathed_geometry({0, 0, 1}, {1, 0, 0}, {0, 1, 0}, 32, ring_points),
        {0.5f, 0.5f, 1, 1.f},
        {0, 0, 1, 1.f}};
    mesh_components[interact::scale_x] = {
        make_lathed_geometry({1, 0, 0}, {0, 1, 0}, {0, 0, 1}, 16, mace_points),
        {1, 0.5f, 0.5f, 1.f},
        {1, 0, 0, 1.f}};
    mesh_components[interact::scale_y] = {
        make_lathed_geometry({0, 1, 0}, {0, 0, 1}, {1, 0, 0}, 16, mace_points),
        {0.5f, 1, 0.5f, 1.f},
        {0, 1, 0, 1.f}};
    mesh_components[interact::scale_z] = {
        make_lathed_geometry({0, 0, 1}, {1, 0, 0}, {0, 1, 0}, 16, mace_points),
        {0.5f, 0.5f, 1, 1.f},
        {0, 0, 1, 1.f}};
  }

  void update(const gizmo_application_state &state) {
    active_state = state;
    local_toggle = (!last_state.hotkey_local && active_state.hotkey_local &&
                    active_state.hotkey_ctrl)
                       ? !local_toggle
                       : local_toggle;
    has_clicked =
        (!last_state.mouse_left && active_state.mouse_left) ? true : false;
    has_released =
        (last_state.mouse_left && !active_state.mouse_left) ? true : false;
    drawlist.clear();
  }
};

// This will calculate a scale constant based on the number of screenspace
// pixels passed as pixel_scale.
float scale_screenspace(gizmo_context_impl &g, const minalg::float3 position,
                        const float pixel_scale) {
  float dist = length(position - to_minalg(g.active_state.ray_origin));
  return std::tan(g.active_state.cam_yfov) * dist *
         (pixel_scale / g.active_state.viewport_size.y);
}

// The only purpose of this is readability: to reduce the total column width of
// the intersect(...) statements in every gizmo
bool intersect(gizmo_context_impl &g, const ray &r, interact i, float &t,
               const float best_t) {
  if (intersect_ray_mesh(r, g.mesh_components[i].mesh, &t) && t < best_t)
    return true;
  return false;
}

///////////////////////////////////
// Private Gizmo Implementations //
///////////////////////////////////

void axis_rotation_dragger(const uint32_t id, gizmo_context_impl &g,
                           const minalg::float3 &axis,
                           const minalg::float3 &center,
                           const minalg::float4 &start_orientation,
                           minalg::float4 &orientation) {
  interaction_state &interaction = g.gizmos[id];

  if (g.active_state.mouse_left) {
    rigid_transform original_pose = {start_orientation,
                                     interaction.original_position};
    auto the_axis = original_pose.transform_vector(axis);
    minalg::float4 the_plane = {the_axis,
                                -dot(the_axis, interaction.click_offset)};
    const ray r = {
        to_minalg(g.active_state.ray_origin),
        to_minalg(g.active_state.ray_direction),
    };

    float t;
    if (intersect_ray_plane(r, the_plane, &t)) {
      minalg::float3 center_of_rotation =
          interaction.original_position +
          the_axis * dot(the_axis, interaction.click_offset -
                                       interaction.original_position);
      minalg::float3 arm1 =
          normalize(interaction.click_offset - center_of_rotation);
      minalg::float3 arm2 =
          normalize(r.origin + r.direction * t - center_of_rotation);

      float d = dot(arm1, arm2);
      if (d > 0.999f) {
        orientation = start_orientation;
        return;
      }

      float angle = std::acos(d);
      if (angle < 0.001f) {
        orientation = start_orientation;
        return;
      }

      if (g.active_state.snap_rotation) {
        auto snapped = make_rotation_quat_between_vectors_snapped(
            arm1, arm2, g.active_state.snap_rotation);
        orientation = qmul(snapped, start_orientation);
      } else {
        auto a = normalize(cross(arm1, arm2));
        orientation = qmul(rotation_quat(a, angle), start_orientation);
      }
    }
  }
}

void plane_translation_dragger(const uint32_t id, gizmo_context_impl &g,
                               const minalg::float3 &plane_normal,
                               minalg::float3 &point) {
  interaction_state &interaction = g.gizmos[id];

  // Mouse clicked
  if (g.has_clicked)
    interaction.original_position = point;

  if (g.active_state.mouse_left) {
    // Define the plane to contain the original position of the object
    auto plane_point = interaction.original_position;
    const ray r = {
        to_minalg(g.active_state.ray_origin),
        to_minalg(g.active_state.ray_direction),
    };

    // If an intersection exists between the ray and the plane, place the object
    // at that point
    const float denom = dot(r.direction, plane_normal);
    if (std::abs(denom) == 0)
      return;

    const float t = dot(plane_point - r.origin, plane_normal) / denom;
    if (t < 0)
      return;

    point = r.origin + r.direction * t;

    if (g.active_state.snap_translation)
      point = snap(point, g.active_state.snap_translation);
  }
}

void axis_translation_dragger(const uint32_t id, gizmo_context_impl &g,
                              const minalg::float3 &axis,
                              minalg::float3 &point) {
  interaction_state &interaction = g.gizmos[id];

  if (g.active_state.mouse_left) {
    // First apply a plane translation dragger with a plane that contains the
    // desired axis and is oriented to face the camera
    const minalg::float3 plane_tangent =
        cross(axis, point - to_minalg(g.active_state.ray_origin));
    const minalg::float3 plane_normal = cross(axis, plane_tangent);
    plane_translation_dragger(id, g, plane_normal, point);

    // Constrain object motion to be along the desired axis
    point = interaction.original_position +
            axis * dot(point - interaction.original_position, axis);
  }
}

///////////////////////////////
//   Gizmo Implementations   //
///////////////////////////////

void position_gizmo(const std::string &name, gizmo_context_impl &g,
                    const minalg::float4 &orientation,
                    minalg::float3 &position) {
  rigid_transform p = rigid_transform(
      g.local_toggle ? orientation : minalg::float4(0, 0, 0, 1), position);
  const float draw_scale =
      (g.active_state.screenspace_scale > 0.f)
          ? scale_screenspace(g, p.position, g.active_state.screenspace_scale)
          : 1.f;
  const uint32_t id = hash_fnv1a(name);

  // interaction_mode will only change on clicked
  if (g.has_clicked)
    g.gizmos[id].interaction_mode = interact::none;

  {
    interact updated_state = interact::none;
    auto ray = detransform(p, {
                                  to_minalg(g.active_state.ray_origin),
                                  to_minalg(g.active_state.ray_direction),
                              });
    detransform(draw_scale, ray);

    float best_t = std::numeric_limits<float>::infinity(), t;
    if (intersect(g, ray, interact::translate_x, t, best_t)) {
      updated_state = interact::translate_x;
      best_t = t;
    }
    if (intersect(g, ray, interact::translate_y, t, best_t)) {
      updated_state = interact::translate_y;
      best_t = t;
    }
    if (intersect(g, ray, interact::translate_z, t, best_t)) {
      updated_state = interact::translate_z;
      best_t = t;
    }
    if (intersect(g, ray, interact::translate_yz, t, best_t)) {
      updated_state = interact::translate_yz;
      best_t = t;
    }
    if (intersect(g, ray, interact::translate_zx, t, best_t)) {
      updated_state = interact::translate_zx;
      best_t = t;
    }
    if (intersect(g, ray, interact::translate_xy, t, best_t)) {
      updated_state = interact::translate_xy;
      best_t = t;
    }
    if (intersect(g, ray, interact::translate_xyz, t, best_t)) {
      updated_state = interact::translate_xyz;
      best_t = t;
    }

    if (g.has_clicked) {
      g.gizmos[id].interaction_mode = updated_state;

      if (g.gizmos[id].interaction_mode != interact::none) {
        transform(draw_scale, ray);
        g.gizmos[id].click_offset =
            g.local_toggle ? p.transform_vector(ray.origin + ray.direction * t)
                           : ray.origin + ray.direction * t;
        g.gizmos[id].active = true;
      } else
        g.gizmos[id].active = false;
    }

    g.gizmos[id].hover =
        (best_t == std::numeric_limits<float>::infinity()) ? false : true;
  }

  std::vector<minalg::float3> axes;
  if (g.local_toggle)
    axes = {qxdir(p.orientation), qydir(p.orientation), qzdir(p.orientation)};
  else
    axes = {{1, 0, 0}, {0, 1, 0}, {0, 0, 1}};

  if (g.gizmos[id].active) {
    position += g.gizmos[id].click_offset;
    switch (g.gizmos[id].interaction_mode) {
    case interact::translate_x:
      axis_translation_dragger(id, g, axes[0], position);
      break;
    case interact::translate_y:
      axis_translation_dragger(id, g, axes[1], position);
      break;
    case interact::translate_z:
      axis_translation_dragger(id, g, axes[2], position);
      break;
    case interact::translate_yz:
      plane_translation_dragger(id, g, axes[0], position);
      break;
    case interact::translate_zx:
      plane_translation_dragger(id, g, axes[1], position);
      break;
    case interact::translate_xy:
      plane_translation_dragger(id, g, axes[2], position);
      break;
    case interact::translate_xyz:
      plane_translation_dragger(
          id, g, -minalg::qzdir(to_minalg(g.active_state.cam_orientation)),
          position);
      break;
    }
    position -= g.gizmos[id].click_offset;
  }

  if (g.has_released) {
    g.gizmos[id].interaction_mode = interact::none;
    g.gizmos[id].active = false;
  }

  std::vector<interact> draw_interactions{
      interact::translate_x,  interact::translate_y,  interact::translate_z,
      interact::translate_yz, interact::translate_zx, interact::translate_xy,
      interact::translate_xyz};

  minalg::float4x4 modelMatrix = p.matrix();
  minalg::float4x4 scaleMatrix = scaling_matrix(minalg::float3(draw_scale));
  modelMatrix = mul(modelMatrix, scaleMatrix);

  for (auto c : draw_interactions) {
    gizmo_renderable r;
    r.mesh = g.mesh_components[c].mesh;
    r.color = (c == g.gizmos[id].interaction_mode)
                  ? g.mesh_components[c].base_color
                  : g.mesh_components[c].highlight_color;
    for (auto &v : r.mesh.vertices) {
      v.position = transform_coord(
          modelMatrix,
          v.position); // transform local coordinates into worldspace
      v.normal = transform_vector(modelMatrix, v.normal);
    }
    g.drawlist.push_back(r);
  }
}

void orientation_gizmo(const std::string &name, gizmo_context_impl &g,
                       const minalg::float3 &center,
                       minalg::float4 &orientation) {
  assert(length2(orientation) > float(1e-6));

  rigid_transform p =
      rigid_transform(g.local_toggle ? orientation : minalg::float4(0, 0, 0, 1),
                      center); // Orientation is local by default
  const float draw_scale =
      (g.active_state.screenspace_scale > 0.f)
          ? scale_screenspace(g, p.position, g.active_state.screenspace_scale)
          : 1.f;
  const uint32_t id = hash_fnv1a(name);

  // interaction_mode will only change on clicked
  if (g.has_clicked)
    g.gizmos[id].interaction_mode = interact::none;

  {
    interact updated_state = interact::none;

    auto ray = detransform(p, {
                                  to_minalg(g.active_state.ray_origin),
                                  to_minalg(g.active_state.ray_direction),
                              });
    detransform(draw_scale, ray);
    float best_t = std::numeric_limits<float>::infinity(), t;

    if (intersect(g, ray, interact::rotate_x, t, best_t)) {
      updated_state = interact::rotate_x;
      best_t = t;
    }
    if (intersect(g, ray, interact::rotate_y, t, best_t)) {
      updated_state = interact::rotate_y;
      best_t = t;
    }
    if (intersect(g, ray, interact::rotate_z, t, best_t)) {
      updated_state = interact::rotate_z;
      best_t = t;
    }

    if (g.has_clicked) {
      g.gizmos[id].interaction_mode = updated_state;
      if (g.gizmos[id].interaction_mode != interact::none) {
        transform(draw_scale, ray);
        g.gizmos[id].original_position = center;
        g.gizmos[id].original_orientation = orientation;
        g.gizmos[id].click_offset =
            p.transform_point(ray.origin + ray.direction * t);
        g.gizmos[id].active = true;
      } else
        g.gizmos[id].active = false;
    }
  }

  minalg::float3 activeAxis;
  if (g.gizmos[id].active) {
    const minalg::float4 starting_orientation =
        g.local_toggle ? g.gizmos[id].original_orientation
                       : minalg::float4(0, 0, 0, 1);
    switch (g.gizmos[id].interaction_mode) {
    case interact::rotate_x:
      axis_rotation_dragger(id, g, {1, 0, 0}, center, starting_orientation,
                            p.orientation);
      activeAxis = {1, 0, 0};
      break;
    case interact::rotate_y:
      axis_rotation_dragger(id, g, {0, 1, 0}, center, starting_orientation,
                            p.orientation);
      activeAxis = {0, 1, 0};
      break;
    case interact::rotate_z:
      axis_rotation_dragger(id, g, {0, 0, 1}, center, starting_orientation,
                            p.orientation);
      activeAxis = {0, 0, 1};
      break;
    }
  }

  if (g.has_released) {
    g.gizmos[id].interaction_mode = interact::none;
    g.gizmos[id].active = false;
  }

  minalg::float4x4 modelMatrix = p.matrix();
  minalg::float4x4 scaleMatrix = scaling_matrix(minalg::float3(draw_scale));
  modelMatrix = mul(modelMatrix, scaleMatrix);

  std::vector<interact> draw_interactions;
  if (!g.local_toggle && g.gizmos[id].interaction_mode != interact::none)
    draw_interactions = {g.gizmos[id].interaction_mode};
  else
    draw_interactions = {interact::rotate_x, interact::rotate_y,
                         interact::rotate_z};

  for (auto c : draw_interactions) {
    gizmo_renderable r;
    r.mesh = g.mesh_components[c].mesh;
    r.color = (c == g.gizmos[id].interaction_mode)
                  ? g.mesh_components[c].base_color
                  : g.mesh_components[c].highlight_color;
    for (auto &v : r.mesh.vertices) {
      v.position = transform_coord(
          modelMatrix,
          v.position); // transform local coordinates into worldspace
      v.normal = transform_vector(modelMatrix, v.normal);
    }
    g.drawlist.push_back(r);
  }

  // For non-local transformations, we only present one rotation ring
  // and draw an arrow from the center of the gizmo to indicate the degree of
  // rotation
  if (g.local_toggle == false &&
      g.gizmos[id].interaction_mode != interact::none) {
    interaction_state &interaction = g.gizmos[id];

    // Create orthonormal basis for drawing the arrow
    minalg::float3 a = qrot(p.orientation, interaction.click_offset -
                                               interaction.original_position);
    minalg::float3 zDir = normalize(activeAxis),
                   xDir = normalize(cross(a, zDir)), yDir = cross(zDir, xDir);

    // Ad-hoc geometry
    std::initializer_list<minalg::float2> arrow_points = {
        {0.0f, 0.f}, {0.0f, 0.05f}, {0.8f, 0.05f}, {0.9f, 0.10f}, {1.0f, 0}};
    auto geo = make_lathed_geometry(yDir, xDir, zDir, 32, arrow_points);

    gizmo_renderable r;
    r.mesh = geo;
    r.color = minalg::float4(1);
    for (auto &v : r.mesh.vertices) {
      v.position = transform_coord(modelMatrix, v.position);
      v.normal = transform_vector(modelMatrix, v.normal);
    }
    g.drawlist.push_back(r);

    orientation = qmul(p.orientation, interaction.original_orientation);
  } else if (g.local_toggle == true &&
             g.gizmos[id].interaction_mode != interact::none)
    orientation = p.orientation;
}

void axis_scale_dragger(const uint32_t &id, gizmo_context_impl &g,
                        const minalg::float3 &axis,
                        const minalg::float3 &center, minalg::float3 &scale,
                        const bool uniform) {
  interaction_state &interaction = g.gizmos[id];

  if (g.active_state.mouse_left) {
    const minalg::float3 plane_tangent =
        cross(axis, center - to_minalg(g.active_state.ray_origin));
    const minalg::float3 plane_normal = cross(axis, plane_tangent);

    minalg::float3 distance;
    if (g.active_state.mouse_left) {
      // Define the plane to contain the original position of the object
      const minalg::float3 plane_point = center;
      const ray ray = {
          to_minalg(g.active_state.ray_origin),
          to_minalg(g.active_state.ray_direction),
      };

      // If an intersection exists between the ray and the plane, place the
      // object at that point
      const float denom = dot(ray.direction, plane_normal);
      if (std::abs(denom) == 0)
        return;

      const float t = dot(plane_point - ray.origin, plane_normal) / denom;
      if (t < 0)
        return;

      distance = ray.origin + ray.direction * t;
    }

    minalg::float3 offset_on_axis =
        (distance - interaction.click_offset) * axis;
    flush_to_zero(offset_on_axis);
    minalg::float3 new_scale = interaction.original_scale + offset_on_axis;

    if (uniform)
      scale = minalg::float3(clamp(dot(distance, new_scale), 0.01f, 1000.f));
    else
      scale = minalg::float3(clamp(new_scale.x, 0.01f, 1000.f),
                             clamp(new_scale.y, 0.01f, 1000.f),
                             clamp(new_scale.z, 0.01f, 1000.f));
    if (g.active_state.snap_scale)
      scale = snap(scale, g.active_state.snap_scale);
  }
}

void scale_gizmo(const std::string &name, gizmo_context_impl &g,
                 const minalg::float4 &orientation,
                 const minalg::float3 &center, minalg::float3 &scale) {
  rigid_transform p = rigid_transform(orientation, center);
  const float draw_scale =
      (g.active_state.screenspace_scale > 0.f)
          ? scale_screenspace(g, p.position, g.active_state.screenspace_scale)
          : 1.f;
  const uint32_t id = hash_fnv1a(name);

  if (g.has_clicked)
    g.gizmos[id].interaction_mode = interact::none;

  {
    interact updated_state = interact::none;
    auto ray = detransform(p, {
                                  to_minalg(g.active_state.ray_origin),
                                  to_minalg(g.active_state.ray_direction),
                              });
    detransform(draw_scale, ray);
    float best_t = std::numeric_limits<float>::infinity(), t;
    if (intersect(g, ray, interact::scale_x, t, best_t)) {
      updated_state = interact::scale_x;
      best_t = t;
    }
    if (intersect(g, ray, interact::scale_y, t, best_t)) {
      updated_state = interact::scale_y;
      best_t = t;
    }
    if (intersect(g, ray, interact::scale_z, t, best_t)) {
      updated_state = interact::scale_z;
      best_t = t;
    }

    if (g.has_clicked) {
      g.gizmos[id].interaction_mode = updated_state;
      if (g.gizmos[id].interaction_mode != interact::none) {
        transform(draw_scale, ray);
        g.gizmos[id].original_scale = scale;
        g.gizmos[id].click_offset =
            p.transform_point(ray.origin + ray.direction * t);
        g.gizmos[id].active = true;
      } else
        g.gizmos[id].active = false;
    }
  }

  if (g.has_released) {
    g.gizmos[id].interaction_mode = interact::none;
    g.gizmos[id].active = false;
  }

  if (g.gizmos[id].active) {
    switch (g.gizmos[id].interaction_mode) {
    case interact::scale_x:
      axis_scale_dragger(id, g, {1, 0, 0}, center, scale,
                         g.active_state.hotkey_ctrl);
      break;
    case interact::scale_y:
      axis_scale_dragger(id, g, {0, 1, 0}, center, scale,
                         g.active_state.hotkey_ctrl);
      break;
    case interact::scale_z:
      axis_scale_dragger(id, g, {0, 0, 1}, center, scale,
                         g.active_state.hotkey_ctrl);
      break;
    }
  }

  minalg::float4x4 modelMatrix = p.matrix();
  minalg::float4x4 scaleMatrix = scaling_matrix(minalg::float3(draw_scale));
  modelMatrix = mul(modelMatrix, scaleMatrix);

  std::vector<interact> draw_components{interact::scale_x, interact::scale_y,
                                        interact::scale_z};

  for (auto c : draw_components) {
    gizmo_renderable r;
    r.mesh = g.mesh_components[c].mesh;
    r.color = (c == g.gizmos[id].interaction_mode)
                  ? g.mesh_components[c].base_color
                  : g.mesh_components[c].highlight_color;
    for (auto &v : r.mesh.vertices) {
      v.position = transform_coord(
          modelMatrix,
          v.position); // transform local coordinates into worldspace
      v.normal = transform_vector(modelMatrix, v.normal);
    }
    g.drawlist.push_back(r);
  }
}

//////////////////////////////////
// Public Gizmo Implementations //
//////////////////////////////////
gizmo_context::gizmo_context() { impl.reset(new gizmo_context_impl(this)); };
gizmo_context::~gizmo_context() {}
void gizmo_context::update(const gizmo_application_state &state) {
  impl->update(state);
}
std::tuple<std::span<draw_vertex>, std::span<uint32_t>>
gizmo_context::drawlist() {
  impl->last_state = impl->active_state;
  impl->vertices.clear();
  impl->indices.clear();

  for (auto &m : impl->drawlist) {
    uint32_t numVerts = (uint32_t)impl->vertices.size();
    auto it = impl->vertices.insert(
        impl->vertices.end(), (draw_vertex *)m.mesh.vertices.data(),
        (draw_vertex *)m.mesh.vertices.data() + m.mesh.vertices.size());
    for (auto &t : m.mesh.triangles) {
      impl->indices.push_back(numVerts + t.x);
      impl->indices.push_back(numVerts + t.y);
      impl->indices.push_back(numVerts + t.z);
    }
    for (; it != impl->vertices.end(); ++it) {
      // Take the color and shove it into a per-vertex attribute
      it->color = {m.color.x, m.color.y, m.color.z, m.color.w};
    }
  }

  return {impl->vertices, impl->indices};
}

bool gizmo_context::transform_gizmo(transform_mode mode,
                                    const std::string &name, float *position,
                                    float *rotation, float *scale) {

  rigid_transform t;
  t.position = {position[0], position[1], position[2]};
  t.orientation = {rotation[0], rotation[1], rotation[2], rotation[3]};
  t.scale = {scale[0], scale[1], scale[2]};

  switch (mode) {
  case transform_mode::translate:
    position_gizmo(name, *this->impl, t.orientation, t.position);
    break;
  case transform_mode::rotate:
    orientation_gizmo(name, *this->impl, t.position, t.orientation);
    break;
  case transform_mode::scale:
    scale_gizmo(name, *this->impl, t.orientation, t.position, t.scale);
    break;
  }

  bool activated = false;
  auto s = this->impl->gizmos[hash_fnv1a(name)];
  if (s.hover == true || s.active == true) {
    activated = true;
  }
  if (activated) {
    position[0] = t.position.x;
    position[1] = t.position.y;
    position[2] = t.position.z;
    rotation[0] = t.orientation.x;
    rotation[1] = t.orientation.y;
    rotation[2] = t.orientation.z;
    rotation[3] = t.orientation.w;
    scale[0] = t.scale.x;
    scale[1] = t.scale.y;
    scale[2] = t.scale.z;
  }
  return activated;
}

} // namespace tinygizmo
