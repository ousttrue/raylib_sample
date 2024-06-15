#include "drag.h"

namespace tinygizmo {

rigid_transform plane_translation_dragger(drag_state *drag,
                                          const gizmo_state &state,
                                          const minalg::float3 &plane_normal,
                                          const rigid_transform &src, bool) {
  // Mouse clicked
  if (state.has_clicked) {
    drag->original_position = src.position;
  }

  if (!state.active_state.mouse_left) {
    return src;
  }

  // Define the plane to contain the original position of the object
  auto plane_point = drag->original_position;
  const ray r = {
      to_minalg(state.active_state.ray_origin),
      to_minalg(state.active_state.ray_direction),
  };

  // If an intersection exists between the ray and the plane, place the
  // object at that point
  const float denom = dot(r.direction, plane_normal);
  if (std::abs(denom) == 0) {
    return src;
  }

  const float t = dot(plane_point - r.origin, plane_normal) / denom;
  if (t < 0) {
    return src;
  }

  auto point = r.origin + r.direction * t;
  if (state.active_state.snap_translation) {
    point = snap(point, state.active_state.snap_translation);
  }
  return rigid_transform(src.orientation, point, src.scale);
}

rigid_transform axis_translation_dragger(drag_state *drag,
                                         const gizmo_state &state,
                                         const minalg::float3 &axis,
                                         const rigid_transform &t, bool) {
  if (!state.active_state.mouse_left) {
    return t;
  }

  // First apply a plane translation dragger with a plane that contains the
  // desired axis and is oriented to face the camera
  const minalg::float3 plane_tangent =
      cross(axis, t.position - to_minalg(state.active_state.ray_origin));
  const minalg::float3 plane_normal = cross(axis, plane_tangent);
  auto dst = plane_translation_dragger(drag, state, plane_normal, t, {});

  // Constrain object motion to be along the desired axis
  auto point = drag->original_position +
               axis * dot(dst.position - drag->original_position, axis);
  return rigid_transform(t.orientation, point, t.scale);
}

rigid_transform axis_rotation_dragger(drag_state *drag,
                                      const gizmo_state &state,
                                      const minalg::float3 &axis,
                                      const rigid_transform &src, bool) {

  const minalg::float4 start_orientation = state.local_toggle
                                               ? drag->original_orientation
                                               : minalg::float4(0, 0, 0, 1);

  if (!state.active_state.mouse_left) {
    return src;
  }

  rigid_transform original_pose = {start_orientation, drag->original_position};
  auto the_axis = original_pose.transform_vector(axis);
  minalg::float4 the_plane = {the_axis, -dot(the_axis, drag->click_offset)};
  const ray r = {
      to_minalg(state.active_state.ray_origin),
      to_minalg(state.active_state.ray_direction),
  };

  float t;
  if (!intersect_ray_plane(r, the_plane, &t)) {
    return src;
  }

  minalg::float3 center_of_rotation =
      drag->original_position +
      the_axis * dot(the_axis, drag->click_offset - drag->original_position);
  minalg::float3 arm1 = normalize(drag->click_offset - center_of_rotation);
  minalg::float3 arm2 =
      normalize(r.origin + r.direction * t - center_of_rotation);

  float d = dot(arm1, arm2);
  if (d > 0.999f) {
    return rigid_transform(start_orientation, src.position, src.scale);
  }

  float angle = std::acos(d);
  if (angle < 0.001f) {
    return rigid_transform(start_orientation, src.position, src.scale);
  }

  if (state.active_state.snap_rotation) {
    auto snapped = make_rotation_quat_between_vectors_snapped(
        arm1, arm2, state.active_state.snap_rotation);
    return rigid_transform(qmul(snapped, start_orientation), src.position,
                           src.scale);
  } else {
    auto a = normalize(cross(arm1, arm2));
    return rigid_transform(qmul(rotation_quat(a, angle), start_orientation),
                           src.position, src.scale);
  }
}

rigid_transform axis_scale_dragger(drag_state *drag, const gizmo_state &state,
                                   const minalg::float3 &axis,
                                   const rigid_transform &src, bool uniform) {
  if (!state.active_state.mouse_left) {
    return src;
  }

  const minalg::float3 plane_tangent =
      cross(axis, src.position - to_minalg(state.active_state.ray_origin));
  const minalg::float3 plane_normal = cross(axis, plane_tangent);

  // Define the plane to contain the original position of the object
  const minalg::float3 plane_point = src.position;
  const ray ray = {
      to_minalg(state.active_state.ray_origin),
      to_minalg(state.active_state.ray_direction),
  };

  // If an intersection exists between the ray and the plane, place the
  // object at that point
  const float denom = dot(ray.direction, plane_normal);
  if (std::abs(denom) == 0)
    return src;

  const float t = dot(plane_point - ray.origin, plane_normal) / denom;
  if (t < 0)
    return src;

  auto distance = ray.origin + ray.direction * t;

  minalg::float3 offset_on_axis = (distance - drag->click_offset) * axis;
  flush_to_zero(offset_on_axis);
  minalg::float3 new_scale = drag->original_scale + offset_on_axis;

  minalg::float3 scale =
      (uniform) ? minalg::float3(clamp(dot(distance, new_scale), 0.01f, 1000.f))
                : minalg::float3(clamp(new_scale.x, 0.01f, 1000.f),
                                 clamp(new_scale.y, 0.01f, 1000.f),
                                 clamp(new_scale.z, 0.01f, 1000.f));
  if (state.active_state.snap_scale) {
    scale = snap(scale, state.active_state.snap_scale);
  }
  return rigid_transform(src.orientation, src.position, scale);
}

} // namespace tinygizmo
