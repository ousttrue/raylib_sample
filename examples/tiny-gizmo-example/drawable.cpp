#include "drawable.h"
#include <unordered_map>

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

