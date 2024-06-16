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

class GizmoDragger : public Dragger {
  Camera *_camera;
  std::list<std::shared_ptr<Drawable>> _scene;

  tinygizmo::gizmo_context gizmo_ctx;

  std::vector<Vector3> positions;
  std::vector<Color> colors;
  std::vector<unsigned short> indices;

public:
  transform_mode mode = transform_mode::translate;
  hotkey active_hotkey{0};
  hotkey last_hotkey{0};
  tinygizmo::gizmo_application_state active_state;
  tinygizmo::gizmo_application_state last_state;
  bool local_toggle{true};

public:
  GizmoDragger(Camera *camera, std::list<std::shared_ptr<Drawable>> &scene)
      : _camera(camera), _scene(scene) {}

  void hotkey(int w, int h, const Vector2 &cursor, const hotkey &hotkey);

  void begin(const Vector2 &cursor) override {}

  void end(const Vector2 &end) override {}

  void drag(const DragState &state, const Vector2 &cursor, int w,
            int h) override {}

  void draw();
};
