#pragma once
#include "drawable.h"
#include "rdrag.h"
#include <list>
#include <tinygizmo_rotation.h>
#include <tinygizmo_scaling.h>
#include <tinygizmo_translation.h>

class Gizmo : public Dragger {
protected:
  std::list<std::shared_ptr<Drawable>> _scene;
  std::shared_ptr<Drawable> gizmo_target;
  std::shared_ptr<tinygizmo::gizmo_mesh_component> active;
  tinygizmo::drag_state drag_state;

public:
  Gizmo(std::list<std::shared_ptr<Drawable>> &scene) : _scene(scene) {}
  tinygizmo::gizmo_application_state gizmo_state;
  bool local_toggle;

  void end(const Vector2 &) override {
    this->active = {};
    this->gizmo_target = {};
    this->drag_state = {};
  }

  virtual void draw(const tinygizmo::AddTriangleFunc &add_triangle,
                    const minalg::float4x4 &modelMatrix) = 0;
};

class TranslationGizmo : public Gizmo {
public:
  using Gizmo::Gizmo;
  void begin(const Vector2 &cursor) override;
  void drag(const DragState &state, int w, int h,
            const Vector2 &cursor) override;
  void draw(const tinygizmo::AddTriangleFunc &add_triangle,
            const minalg::float4x4 &modelMatrix) override {
    tinygizmo::position_draw(add_triangle, active, modelMatrix);
  }
};

class RotationGizmo : public Gizmo {
public:
  using Gizmo::Gizmo;
  void begin(const Vector2 &cursor) override;
  void drag(const DragState &state, int w, int h,
            const Vector2 &cursor) override;
  void draw(const tinygizmo::AddTriangleFunc &add_triangle,
            const minalg::float4x4 &modelMatrix) override {
    tinygizmo::rotation_draw(add_triangle, active, modelMatrix);
  }
};

class ScalingGizmo : public Gizmo {
public:
  bool uniform = false;
  using Gizmo::Gizmo;
  void begin(const Vector2 &cursor) override;
  void drag(const DragState &state, int w, int h,
            const Vector2 &cursor) override;
  void draw(const tinygizmo::AddTriangleFunc &add_triangle,
            const minalg::float4x4 &modelMatrix) override {
    tinygizmo::scaling_draw(add_triangle, active, modelMatrix);
  }
};

struct hotkey {
  bool hotkey_ctrl = false;
  bool hotkey_translate = false;
  bool hotkey_rotate = false;
  bool hotkey_scale = false;
  bool hotkey_local = false;
};

class TRSGizmo : public Dragger {
  Camera *_camera;
  std::list<std::shared_ptr<Drawable>> _scene;

  hotkey active_hotkey{0};
  hotkey last_hotkey{0};
  tinygizmo::gizmo_application_state active_state;
  tinygizmo::gizmo_application_state last_state;
  bool local_toggle{true};

  std::shared_ptr<TranslationGizmo> _t;
  std::shared_ptr<RotationGizmo> _r;
  std::shared_ptr<ScalingGizmo> _s;
  std::shared_ptr<Gizmo> _active;

  std::vector<Vector3> positions;
  std::vector<Color> colors;
  std::vector<unsigned short> indices;

public:
  TRSGizmo(Camera *camera, std::list<std::shared_ptr<Drawable>> &scene)
      : _camera(camera), _scene(scene) {
    _t = std::make_shared<TranslationGizmo>(_scene);
    _r = std::make_shared<RotationGizmo>(_scene);
    _s = std::make_shared<ScalingGizmo>(_scene);
    _active = _t;
  }

  void begin(const Vector2 &cursor) override { _active->begin(cursor); }
  void end(const Vector2 &cursor) override { _active->end(cursor); }
  void drag(const DragState &state, int w, int h,
            const Vector2 &cursor) override {
    _active->drag(state, w, h, cursor);
  }

  void hotkey(int w, int h, const Vector2 &cursor, const hotkey &hotkey);
  void load(Drawable *drawable);
};
