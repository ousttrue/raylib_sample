#pragma once
#include <raylib.h>
#include <raymath.h>
#include <rlgl.h>
#include <span>

struct Drawable {
  Model model = {};

  // Generate a simple triangle mesh from code
  template <typename V, typename I>
  void load(std::span<const V> vertices, std::span<const I> indices,
            bool dynamic) {

    Mesh mesh = {0};

    // vertices
    mesh.vertexCount = vertices.size();
    mesh.vertices = (float *)MemAlloc(mesh.vertexCount * 3 * sizeof(float));
    // mesh.normals = (float *)MemAlloc(mesh.vertexCount * 3 * sizeof(float));
    mesh.colors =
        (unsigned char *)MemAlloc(mesh.vertexCount * 4 * sizeof(unsigned char));
    auto pos = 0;
    // auto nom = 0;
    auto col = 0;
    for (auto &v : vertices) {
      mesh.vertices[pos++] = v.position.x;
      mesh.vertices[pos++] = v.position.y;
      mesh.vertices[pos++] = v.position.z;

      // mesh.normals[nom++] = v.normal.x;
      // mesh.normals[nom++] = v.normal.y;
      // mesh.normals[nom++] = v.normal.z;

      mesh.colors[col++] = static_cast<unsigned char>(v.color.x * 255);
      mesh.colors[col++] = static_cast<unsigned char>(v.color.y * 255);
      mesh.colors[col++] = static_cast<unsigned char>(v.color.z * 255);
      mesh.colors[col++] = 255;
    }

    mesh.triangleCount = indices.size() / 3;
    mesh.indices = (unsigned short *)MemAlloc(mesh.triangleCount * 3 *
                                              sizeof(unsigned short));
    auto index = 0;
    for (auto &i : indices) {
      mesh.indices[index++] = i;
    }

    // Upload mesh data from CPU (RAM) to GPU (VRAM) memory
    UploadMesh(&mesh, dynamic);
    this->model = LoadModelFromMesh(mesh);
  }

  void draw(const Matrix &_m) {
    rlPushMatrix();
    auto m = MatrixTranspose(_m);
    rlMultMatrixf(&m.m0);
    DrawModel(this->model, {0, 0, 0}, 1.0f, WHITE);
    rlPopMatrix();
  }
};
