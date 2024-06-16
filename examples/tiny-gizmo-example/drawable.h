#pragma once
#include "rdrag.h"
#include <list>
#include <raylib.h>
#include <raymath.h>
#include <rlgl.h>
#include <span>
#include <tiny-gizmo.h>

struct Vertex {
  Vector3 position;
  Vector3 color;
};

struct Drawable {
  std::string name;
  Model model = {};
  Vector3 position = {0, 0, 0};
  Quaternion rotation = {0, 0, 0, 1};
  Vector3 scale = {1, 1, 1};

  // Generate a simple triangle mesh from code
  void load(std::span<const Vertex> vertices, std::span<const uint32_t> indices,
            bool dynamic);

  void load(size_t vertexCount, const Vector3 *vertices, const Color *colors,
            size_t indexCount, const unsigned short *indices, bool dynamic);

  void draw();
};

enum class transform_mode {
  translate,
  rotate,
  scale,
};

struct hotkey {
  bool hotkey_ctrl = false;
  bool hotkey_translate = false;
  bool hotkey_rotate = false;
  bool hotkey_scale = false;
  bool hotkey_local = false;
};
