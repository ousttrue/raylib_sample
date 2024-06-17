#pragma once
#include "minalg.hpp"

namespace minalg {

struct rigid_transform {
  minalg::float4 orientation = {0, 0, 0, 1};
  minalg::float3 position = {0, 0, 0};
  minalg::float3 scale = {1, 1, 1};

  bool uniform_scale() const {
    return scale.x == scale.y && scale.x == scale.z;
  }
  minalg::float4x4 matrix() const {
    return {{qxdir(orientation) * scale.x, 0},
            {qydir(orientation) * scale.y, 0},
            {qzdir(orientation) * scale.z, 0},
            {position, 1}};
  }
  minalg::float3 transform_vector(const minalg::float3 &vec) const {
    return qrot(orientation, vec * scale);
  }
  minalg::float3 transform_point(const minalg::float3 &p) const {
    return position + transform_vector(p);
  }
  minalg::float3 detransform_point(const minalg::float3 &p) const {
    return detransform_vector(p - position);
  }
  minalg::float3 detransform_vector(const minalg::float3 &vec) const {
    return qrot(qinv(orientation), vec) / scale;
  }
};

} // namespace minalg
