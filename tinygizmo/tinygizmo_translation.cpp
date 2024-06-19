#include "tinygizmo_translation.h"
#include <assert.h>
#include <optional>

namespace tinygizmo {

std::vector<minalg::float2> arrow_points = {
    {0.25f, 0}, {0.25f, 0.05f}, {1, 0.05f}, {1, 0.10f}, {1.2f, 0}};

struct translation_plane_component : gizmo_component {
  using gizmo_component::gizmo_component;

  virtual minalg::float3 get_axis(const gizmo_application_state &state,
                                  bool local_toggle,
                                  const minalg::float4 &rotation) const = 0;

  std::optional<minalg::rigid_transform>
  drag(drag_state *drag, const gizmo_application_state &active_state,
       bool local_toggle, const minalg::rigid_transform &p) const override {
    auto plane_normal = get_axis(active_state, local_toggle, p.orientation);
    auto plane = make_plane(plane_normal, drag->original_position);
    auto t = active_state.ray.intersect_plane(plane);
    if (!t) {
      return {};
    }
    auto dst = active_state.ray.point(*t) - drag->click_offset;

    return minalg::rigid_transform{
        .orientation = p.orientation,
        .position = dst,
        .scale = p.scale,
    };
  }
};

struct translation_axis_component : gizmo_component {
  using gizmo_component::gizmo_component;

  virtual minalg::float3 get_axis(const gizmo_application_state &state,
                                  bool local_toggle,
                                  const minalg::float4 &rotation) const = 0;

  std::optional<minalg::rigid_transform>
  drag(drag_state *drag, const gizmo_application_state &active_state,
       bool local_toggle, const minalg::rigid_transform &p) const override {
    // First apply a plane translation dragger with a plane that contains the
    // desired axis and is oriented to face the camera
    auto axis = get_axis(active_state, local_toggle, p.orientation);
    auto plane_tangent = cross(axis, p.position - active_state.ray.origin);
    auto plane_normal = cross(axis, plane_tangent);
    auto plane = make_plane(plane_normal, drag->original_position);
    auto t = active_state.ray.intersect_plane(plane);
    if (!t) {
      return {};
    }
    auto dst = active_state.ray.point(*t) - drag->click_offset;

    // Constrain object motion to be along the desired axis
    auto point = drag->original_position +
                 axis * dot(dst - drag->original_position, axis);
    return minalg::rigid_transform(p.orientation, point, p.scale);
  }
};

struct translation_x_component : translation_axis_component {
  translation_x_component()
      : translation_axis_component(
            geometry_mesh::make_lathed_geometry({1, 0, 0}, {0, 1, 0}, {0, 0, 1},
                                                16, arrow_points),
            minalg::float4{1, 0.5f, 0.5f, 1.f}, minalg::float4{1, 0, 0, 1.f}) {}

  minalg::float3 get_axis(const gizmo_application_state &state,
                          bool local_toggle,
                          const minalg::float4 &rotation) const override {
    return (local_toggle) ? qxdir(rotation) : minalg::float3{1, 0, 0};
  }
};

struct translation_y_component : translation_axis_component {
  translation_y_component()
      : translation_axis_component(
            geometry_mesh::make_lathed_geometry({0, 1, 0}, {0, 0, 1}, {1, 0, 0},
                                                16, arrow_points),
            minalg::float4{0.5f, 1, 0.5f, 1.f}, minalg::float4{0, 1, 0, 1.f}) {}

  minalg::float3 get_axis(const gizmo_application_state &state,
                          bool local_toggle,
                          const minalg::float4 &rotation) const override {
    return (local_toggle) ? qydir(rotation) : minalg::float3{0, 1, 0};
  }
};

struct translation_z_component : translation_axis_component {
  translation_z_component()
      : translation_axis_component(
            geometry_mesh::make_lathed_geometry({0, 0, 1}, {1, 0, 0}, {0, 1, 0},
                                                16, arrow_points),
            minalg::float4{0.5f, 0.5f, 1, 1.f}, minalg::float4{0, 0, 1, 1.f}) {}

  minalg::float3 get_axis(const gizmo_application_state &state,
                          bool local_toggle,
                          const minalg::float4 &rotation) const override {
    return (local_toggle) ? qzdir(rotation) : minalg::float3{0, 0, 1};
  }
};

struct translation_yz_component : translation_plane_component {
  translation_yz_component()
      : translation_plane_component(
            geometry_mesh::make_box_geometry({-0.01f, 0.25, 0.25},
                                             {0.01f, 0.75f, 0.75f}),
            minalg::float4{0.5f, 1, 1, 0.5f}, minalg::float4{0, 1, 1, 0.6f}) {}

  minalg::float3 get_axis(const gizmo_application_state &state,
                          bool local_toggle,
                          const minalg::float4 &rotation) const override {
    return (local_toggle) ? qxdir(rotation) : minalg::float3{1, 0, 0};
  }
};

struct translation_zx_component : translation_plane_component {
  translation_zx_component()
      : translation_plane_component(
            geometry_mesh::make_box_geometry({0.25, -0.01f, 0.25},
                                             {0.75f, 0.01f, 0.75f}),
            minalg::float4{1, 0.5f, 1, 0.5f}, minalg::float4{1, 0, 1, 0.6f}) {}

  minalg::float3 get_axis(const gizmo_application_state &state,
                          bool local_toggle,
                          const minalg::float4 &rotation) const override {
    return (local_toggle) ? qydir(rotation) : minalg::float3{0, 1, 0};
  }
};

struct translation_xy_component : translation_plane_component {
  translation_xy_component()
      : translation_plane_component(
            geometry_mesh::make_box_geometry({0.25, 0.25, -0.01f},
                                             {0.75f, 0.75f, 0.01f}),
            minalg::float4{1, 1, 0.5f, 0.5f}, minalg::float4{1, 1, 0, 0.6f}) {}

  minalg::float3 get_axis(const gizmo_application_state &state,
                          bool local_toggle,
                          const minalg::float4 &rotation) const override {
    return (local_toggle) ? qzdir(rotation) : minalg::float3{0, 0, 1};
  }
};

struct translation_xyz_component : translation_plane_component {
  translation_xyz_component()
      : translation_plane_component(
            geometry_mesh::make_box_geometry({-0.05f, -0.05f, -0.05f},
                                             {0.05f, 0.05f, 0.05f}),
            minalg::float4{0.9f, 0.9f, 0.9f, 0.25f},
            minalg::float4{1, 1, 1, 0.35f}) {}

  minalg::float3 get_axis(const gizmo_application_state &state,
                          bool local_toggle,
                          const minalg::float4 &) const override {
    return -minalg::qzdir(state.cam_orientation);
  }
};

auto _translate_x = std::make_shared<translation_x_component>();
auto _translate_y = std::make_shared<translation_y_component>();
auto _translate_z = std::make_shared<translation_z_component>();
auto _translate_yz = std::make_shared<translation_yz_component>();
auto _translate_zx = std::make_shared<translation_zx_component>();
auto _translate_xy = std::make_shared<translation_xy_component>();
auto _translate_xyz = std::make_shared<translation_xyz_component>();
std::shared_ptr<gizmo_component> _gizmo_components[] = {
    _translate_x,  _translate_y,  _translate_z,   _translate_yz,
    _translate_zx, _translate_xy, _translate_xyz,
};

std::tuple<std::shared_ptr<gizmo_component>, float>
position_intersect(const ray &ray) {
  float best_t = std::numeric_limits<float>::infinity();
  std::shared_ptr<gizmo_component> updated_state = {};
  float t;
  for (auto c : _gizmo_components) {
    if (ray.intersect_mesh(c->mesh, &t) && t < best_t) {
      updated_state = c;
      best_t = t;
    }
  }
  return {updated_state, best_t};
}

minalg::float3 position_drag(drag_state *drag,
                             const gizmo_application_state &state,
                             bool local_toggle,
                             const std::shared_ptr<gizmo_component> &active,
                             const minalg::rigid_transform &p) {

  if (auto dst =
          active->drag(drag, state, local_toggle, {.position = p.position})) {
    return dst->position;
  } else {
    return p.position;
  }
}

void position_draw(const AddTriangleFunc &add_world_triangle,
                   const std::shared_ptr<gizmo_component> &active,
                   const minalg::float4x4 &modelMatrix) {
  for (auto c : _gizmo_components) {
    auto mesh = c->mesh;
    auto color = (c == active) ? c->base_color : c->highlight_color;
    add_triangles(add_world_triangle, modelMatrix, mesh, color);
  }
}

} // namespace tinygizmo
