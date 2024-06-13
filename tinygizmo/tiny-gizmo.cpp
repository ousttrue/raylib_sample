// This is free and unencumbered software released into the public domain.
// For more information, please refer to <http://unlicense.org>

#include "interaction_state.hpp"

namespace tinygizmo {

std::shared_ptr<interaction_state> gizmo_context::get_or_create(uint32_t id) {
  auto found = gizmos.find(id);
  if (found != gizmos.end()) {
    return found->second;
  }

  auto new_gizmo = std::make_shared<interaction_state>();
  gizmos.insert({id, new_gizmo});
  return new_gizmo;
}

gizmo_result gizmo_context::translation_gizmo(
    const gizmo_state &state, const AddTriangleFunc &add_triangle,
    bool local_toggle, uint32_t id, const float t[3], const float r[4]) {
  return this->get_or_create(id)->position_gizmo(
      state, add_triangle, local_toggle, *(minalg::float4 *)r,
      *(minalg::float3 *)t);
}

gizmo_result gizmo_context::rotationn_gizmo(const gizmo_state &state,
                                            const AddTriangleFunc &add_triangle,
                                            bool local_toggle, uint32_t id,
                                            const float t[3],
                                            const float r[4]) {
  return this->get_or_create(id)->rotation_gizmo(
      state, add_triangle, local_toggle, *(minalg::float3 *)t,
      *(minalg::float4 *)r);
}

gizmo_result gizmo_context::scale_gizmo(const gizmo_state &state,
                                        const AddTriangleFunc &add_triangle,
                                        bool local_toggle, bool uniform,
                                        uint32_t id, const float t[3],
                                        const float r[4], const float s[3]) {
  return this->get_or_create(id)->scale_gizmo(
      state, add_triangle, local_toggle, *(minalg::float4 *)&r,
      *(minalg::float3 *)&t, *(minalg::float3 *)&s, uniform);
}

} // namespace tinygizmo
