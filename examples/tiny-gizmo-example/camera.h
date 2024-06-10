#pragma once

struct camera {
  float yfov;
  float near_clip;
  float far_clip;
  linalg::aliases::float3 position;
  Camera3D camera;
  float pitch;
  float yaw;

  linalg::aliases::float4 rotation() const {
    return qmul(rotation_quat(linalg::aliases::float3(0, 1, 0), yaw),
                rotation_quat(linalg::aliases::float3(1, 0, 0), pitch));
  }

  linalg::aliases::float4x4 get_view_matrix() const {
    return mul(rotation_matrix(qconj(rotation())),
               translation_matrix(-position));
  }

  linalg::aliases::float4x4
  get_projection_matrix(const float aspectRatio) const {
    return linalg::perspective_matrix(yfov, aspectRatio, near_clip, far_clip);
  }

  linalg::aliases::float4x4 get_viewproj_matrix(const float aspectRatio) const {
    return mul(get_projection_matrix(aspectRatio), get_view_matrix());
  }

  void shift(float timestep) {
    auto bf = IsKeyDown(KEY_W);
    auto bl = IsKeyDown(KEY_A);
    auto bb = IsKeyDown(KEY_S);
    auto br = IsKeyDown(KEY_D);

    auto orientation = this->rotation();
    linalg::aliases::float3 move;
    if (bf)
      move -= qzdir(orientation);
    if (bl)
      move -= qxdir(orientation);
    if (bb)
      move += qzdir(orientation);
    if (br)
      move += qxdir(orientation);
    if (length2(move) > 0) {
      this->position += normalize(move) * (timestep * 10);
    }
  }
};
