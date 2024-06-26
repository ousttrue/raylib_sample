#pragma once
#include "tinygizmo_alg.h"
#include <raylib.h>
#include <raymath.h>
#include <rlgl.h>
#include <span>
#include <string>

struct Vertex {
  Vector3 position;
  Vector3 color;
};

struct Drawable {
  std::string name;
  Model model = {};
  tinygizmo::Transform transform = {
      .orientation = {0, 0, 0, 1},
      .position = {0, 0, 0},
      .scale = {1, 1, 1},
  };

  // Generate a simple triangle mesh from code
  void load(std::span<const Vertex> vertices, std::span<const uint32_t> indices,
            bool dynamic);

  void load(size_t vertexCount, const Vector3 *vertices, const Color *colors,
            size_t indexCount, const unsigned short *indices, bool dynamic);

  void draw();
};
