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

void gizmo_context::begin_frame(const gizmo_application_state &active_state) {
  state.active_state = active_state;
  state.has_clicked =
      (!state.last_state.mouse_left && active_state.mouse_left) ? true : false;
  state.has_released =
      (state.last_state.mouse_left && !active_state.mouse_left) ? true : false;
  state.drawlist.clear();
}

std::tuple<std::span<draw_vertex>, std::span<uint32_t>>
gizmo_context::end_frame() {
  state.last_state = state.active_state;
  this->vertices.clear();
  this->indices.clear();

  for (auto &m : state.drawlist) {
    uint32_t numVerts = (uint32_t)this->vertices.size();
    auto it = this->vertices.insert(
        this->vertices.end(), (draw_vertex *)m.mesh.vertices.data(),
        (draw_vertex *)m.mesh.vertices.data() + m.mesh.vertices.size());
    for (auto &t : m.mesh.triangles) {
      this->indices.push_back(numVerts + t.x);
      this->indices.push_back(numVerts + t.y);
      this->indices.push_back(numVerts + t.z);
    }
    for (; it != this->vertices.end(); ++it) {
      // Take the color and shove it into a per-vertex attribute
      it->color = {m.color.x, m.color.y, m.color.z, m.color.w};
    }
  }

  return {this->vertices, this->indices};
}

gizmo_result gizmo_context::translation_gizmo(bool local_toggle, uint32_t id,
                                              const float t[3],
                                              const float r[4]) {
  return this->get_or_create(id)->position_gizmo(
      state, local_toggle, *(minalg::float4 *)r, *(minalg::float3 *)t);
}

gizmo_result gizmo_context::rotationn_gizmo(bool local_toggle, uint32_t id,
                                            const float t[3],
                                            const float r[4]) {
  return this->get_or_create(id)->rotation_gizmo(
      state, local_toggle, *(minalg::float3 *)t, *(minalg::float4 *)r);
}

gizmo_result gizmo_context::scale_gizmo(bool local_toggle, bool uniform,
                                        uint32_t id, const float t[3],
                                        const float r[4], const float s[3]) {
  return this->get_or_create(id)->scale_gizmo(
      state, local_toggle, *(minalg::float4 *)&r, *(minalg::float3 *)&t,
      *(minalg::float3 *)&s, uniform);
}

} // namespace tinygizmo
