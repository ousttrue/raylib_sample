#pragma once
#include "minalg.hpp"
#include "tiny-gizmo.hpp"
#include <assert.h>
#include <map>

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

class gizmo_context_impl {
  std::vector<draw_vertex> vertices;
  std::vector<uint32_t> indices;

  std::map<interact, gizmo_mesh_component> mesh_components;
  std::vector<gizmo_renderable> drawlist;

  std::map<uint32_t, interaction_state> gizmos;

  gizmo_application_state active_state;
  gizmo_application_state last_state;
  bool has_clicked{false}; // State to describe if the user has pressed the left
                           // mouse button during the last frame
  bool has_released{false}; // State to describe if the user has released the
                            // left mouse button during the last frame

public:
  gizmo_context_impl();

  void begin_frame(const gizmo_application_state &state);

  std::tuple<std::span<draw_vertex>, std::span<uint32_t>> end_frame();

private:
  // The only purpose of this is readability: to reduce the total column width
  // of the intersect(...) statements in every gizmo
  bool intersect(const ray &r, interact i, float &t, const float best_t);

  // This will calculate a scale constant based on the number of screenspace
  // pixels passed as pixel_scale.
  float scale_screenspace(const minalg::float3 position,
                          const float pixel_scale);

  void plane_translation_dragger(const uint32_t id,
                                 const minalg::float3 &plane_normal,
                                 minalg::float3 &point);

  void axis_translation_dragger(const uint32_t id, const minalg::float3 &axis,
                                minalg::float3 &point);

  void axis_rotation_dragger(const uint32_t id, const minalg::float3 &axis,
                             const minalg::float3 &center,
                             const minalg::float4 &start_orientation,
                             minalg::float4 &orientation);

  void axis_scale_dragger(const uint32_t &id, const minalg::float3 &axis,
                          const minalg::float3 &center, minalg::float3 &scale,
                          const bool uniform);

public:
  gizmo_result position_gizmo(bool local_toggle, uint32_t id,
                              const minalg::float4 &rotation,
                              const minalg::float3 &_position);

  gizmo_result rotation_gizmo(bool local_toggle, uint32_t id,
                              const minalg::float3 &center,
                              const minalg::float4 &_orientation);

  gizmo_result scale_gizmo(bool local_toggle, uint32_t id,
                           const minalg::float4 &orientation,
                           const minalg::float3 &center,
                           const minalg::float3 &_scale, bool uniform);
};

} // namespace tinygizmo
