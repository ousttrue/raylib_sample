// This is free and unencumbered software released into the public domain.
// For more information, please refer to <http://unlicense.org>

#include "tiny-gizmo_impl.hpp"

namespace tinygizmo {

gizmo_context::gizmo_context() { impl = new gizmo_context_impl(); };
gizmo_context::~gizmo_context() { delete impl; }

void gizmo_context::begin_frame(const gizmo_application_state &state) {
  impl->begin_frame(state);
}

std::tuple<std::span<draw_vertex>, std::span<uint32_t>>
gizmo_context::end_frame() {
  return impl->end_frame();
}

gizmo_result gizmo_context::translation_gizmo(bool local_toggle,
                                              const std::string &name,
                                              const float t[3],
                                              const float r[4]) {
  return this->impl->position_gizmo(local_toggle, name, *(minalg::float4 *)r,
                                    *(minalg::float3 *)t);
}

gizmo_result gizmo_context::rotationn_gizmo(bool local_toggle,
                                            const std::string &name,
                                            const float t[3],
                                            const float r[4]) {
  return this->impl->rotation_gizmo(local_toggle, name, *(minalg::float3 *)t,
                                    *(minalg::float4 *)r);
}

gizmo_result gizmo_context::scale_gizmo(bool local_toggle, bool uniform,
                                        const std::string &name,
                                        const float t[3], const float r[4],
                                        const float s[3]) {
  return this->impl->scale_gizmo(local_toggle, name, *(minalg::float4 *)&r,
                                 *(minalg::float3 *)&t, *(minalg::float3 *)&s,
                                 uniform);
}

} // namespace tinygizmo
