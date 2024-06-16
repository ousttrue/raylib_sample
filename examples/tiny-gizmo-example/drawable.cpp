#include "drawable.h"

inline Matrix TRS(const Vector3 &t, const Quaternion &r, const Vector3 &s) {
  return MatrixMultiply(
      MatrixMultiply(MatrixScale(s.x, s.y, s.z), QuaternionToMatrix(r)),
      MatrixTranslate(t.x, t.y, t.z));
}

void Drawable::load(std::span<const Vertex> vertices,
                    std::span<const uint32_t> indices, bool dynamic) {

  // vertices
  auto mesh_vertices = (Vector3 *)MemAlloc(vertices.size() * sizeof(Vector3));
  // mesh.normals = (float *)MemAlloc(mesh.vertexCount * 3 * sizeof(float));
  auto mesh_colors = (Color *)MemAlloc(vertices.size() * sizeof(Color));
  auto pos = 0;
  // auto nom = 0;
  auto col = 0;
  for (auto &v : vertices) {
    mesh_vertices[pos++] = {
        v.position.x,
        v.position.y,
        v.position.z,
    };

    // mesh.normals[nom++] = v.normal.x;
    // mesh.normals[nom++] = v.normal.y;
    // mesh.normals[nom++] = v.normal.z;

    mesh_colors[col++] = {
        static_cast<unsigned char>(std::max(0.0f, v.color.x) * 255),
        static_cast<unsigned char>(std::max(0.0f, v.color.y) * 255),
        static_cast<unsigned char>(std::max(0.0f, v.color.z) * 255),
        255,
    };
  }

  auto mesh_indices =
      (unsigned short *)MemAlloc(indices.size() * sizeof(unsigned short));
  auto index = 0;
  for (auto &i : indices) {
    mesh_indices[index++] = i;
  }

  load(vertices.size(), mesh_vertices, mesh_colors, indices.size(),
       mesh_indices, dynamic);
}

void Drawable::load(size_t vertexCount, const Vector3 *vertices,
                    const Color *colors, size_t indexCount,
                    const unsigned short *indices, bool dynamic) {
  // Upload mesh data from CPU (RAM) to GPU (VRAM) memory
  Mesh mesh = {
      .vertexCount = static_cast<int>(vertexCount),
      .triangleCount = static_cast<int>(indexCount / 3),
      .vertices = const_cast<float *>(&vertices[0].x),
      .colors = const_cast<unsigned char *>(&colors[0].r),
      .indices = const_cast<unsigned short *>(indices),
  };
  UploadMesh(&mesh, dynamic);
  this->model = LoadModelFromMesh(mesh);
}

void Drawable::draw() {
  auto _m = TRS(position, rotation, scale);
  rlPushMatrix();
  auto m = MatrixTranspose(_m);
  rlMultMatrixf(&m.m0);
  DrawModel(this->model, {0, 0, 0}, 1.0f, WHITE);
  rlPopMatrix();
}

inline void
transform_gizmo(tinygizmo::gizmo_context *gizmo_ctx,
                const tinygizmo::gizmo_state &state,
                const tinygizmo::AddTriangleFunc &add_world_triangle,
                transform_mode mode, bool uniform, const std::string &name,
                Vector3 &t, Quaternion &r, Vector3 &s) {

  auto id = tinygizmo::hash_fnv1a(name);
  auto gizmo = gizmo_ctx->get_or_create(id);
  // interaction_mode will only change on clicked
  if (state.has_clicked) {
    gizmo->active = nullptr;
  } else if (state.has_released) {
    gizmo->active = nullptr;
  }

  rigid_transform p(*(minalg::float4 *)&r.x, *(minalg::float3 *)&t.x,
                    *(minalg::float3 *)&s.x);
  switch (mode) {
  case transform_mode::translate: {
    auto result = gizmo->position_gizmo(state, add_world_triangle, p);
    if (result.active) {
      t = *(Vector3 *)&result.t;
    }
    break;
  }
  case transform_mode::rotate: {
    auto result = gizmo->rotation_gizmo(state, add_world_triangle, p);
    if (result.active) {
      r = *(Quaternion *)&result.r;
    }
    break;
  }
  case transform_mode::scale: {
    auto result = gizmo->scale_gizmo(state, add_world_triangle, p, uniform);
    if (result.active) {
      s = *(Vector3 *)&result.s;
    }
    break;
  }
  }
}

void GizmoDragger::hotkey(int w, int h, const Vector2 &cursor,
                          const struct hotkey &hotkey) {
  if (hotkey.hotkey_ctrl == true) {
    if (last_hotkey.hotkey_translate == false &&
        hotkey.hotkey_translate == true)
      mode = transform_mode::translate;
    else if (last_hotkey.hotkey_rotate == false && hotkey.hotkey_rotate == true)
      mode = transform_mode::rotate;
    else if (last_hotkey.hotkey_scale == false && hotkey.hotkey_scale == true)
      mode = transform_mode::scale;

    local_toggle = (!last_hotkey.hotkey_local && hotkey.hotkey_local)
                       ? !local_toggle
                       : local_toggle;
  }
  last_hotkey = active_hotkey;
  active_hotkey = hotkey;

  auto ray = GetMouseRay(cursor, *_camera);
  auto rot =
      QuaternionFromEuler(ray.direction.x, ray.direction.y, ray.direction.z);

  last_state = active_state;
  active_state = {
      .mouse_left = IsMouseButtonDown(MOUSE_BUTTON_LEFT),
      // optional flag to draw the gizmos at a constant screen-space
      // scale gizmo_state.screenspace_scale = 80.f; camera projection
      .viewport_size = {static_cast<float>(w), static_cast<float>(h)},
      .ray_origin = {ray.position.x, ray.position.y, ray.position.z},
      .ray_direction = {ray.direction.x, ray.direction.y, ray.direction.z},
      .cam_yfov = 1.0f,
      .cam_orientation = {rot.x, rot.y, rot.z, rot.w},
  };
  // tinygizmo::gizmo_state state(local_toggle, active_state, last_state);
}

void GizmoDragger::draw() {
  positions.clear();
  colors.clear();
  indices.clear();
  tinygizmo::AddTriangleFunc add_world_triangle =
      [self = this](
          const std::array<float, 4> &rgba, const std::array<float, 3> &p0,
          const std::array<float, 3> &p1, const std::array<float, 3> &p2) {
        //
        auto offset = self->positions.size();
        Color color{
            static_cast<unsigned char>(std::max(0.0f, rgba[0]) * 255),
            static_cast<unsigned char>(std::max(0.0f, rgba[1]) * 255),
            static_cast<unsigned char>(std::max(0.0f, rgba[2]) * 255),
            static_cast<unsigned char>(std::max(0.0f, rgba[3]) * 255),
        };
        self->positions.push_back({p0[0], p0[1], p0[2]});
        self->positions.push_back({p1[0], p1[1], p1[2]});
        self->positions.push_back({p2[0], p2[1], p2[2]});
        self->colors.push_back(color);
        self->colors.push_back(color);
        self->colors.push_back(color);
        self->indices.push_back(offset + 0);
        self->indices.push_back(offset + 1);
        self->indices.push_back(offset + 2);
      };

  // gizmo_ctx.begin_frame(active_state);
  // transform_gizmo(&gizmo_ctx, state, add_world_triangle, mode,
  //                 active_hotkey.hotkey_ctrl, "first-example-gizmo", a_t,
  //                 a_r, a_s);
  // transform_gizmo(&gizmo_ctx, state, add_world_triangle, mode,
  //                 active_hotkey.hotkey_ctrl, "second-example-gizmo", b_t,
  //                 b_r, b_s);

  // end_frame(state.drawlist, vertices, indices);
  // if (positions.size() && indices.size()) {
  //   assert(positions.size() == colors.size());
  //   // update gizmo mesh
  //   gizmo_mesh.load(positions.size(), positions.data(), colors.data(),
  //                   indices.size(), indices.data(), true);
  // }
}
