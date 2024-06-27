#pragma once
#include "drawable.h"
#include "rdrag.h"
#include <list>
#include <tinygizmo.h>

enum class GizmoMode {
  Translation,
  Rotation,
  Scaling,
};

struct Hotkey {
  bool hotkey_ctrl = false;
  bool hotkey_translate = false;
  bool hotkey_rotate = false;
  bool hotkey_scale = false;
  bool hotkey_local = false;
};

class TRSGizmo : public Dragger {
  GizmoMode _visible = GizmoMode::Translation;
  Camera *_camera;
  std::list<std::shared_ptr<Drawable>> _scene;
  std::shared_ptr<Drawable> _gizmo_target;

  Hotkey _current_hotkey = {0};
  Hotkey _last_hotkey = {0};
  tinygizmo::FrameState _current_state;
  tinygizmo::FrameState _last_state;
  bool _local_toggle = true;
  bool _uniform = true;

  std::optional<tinygizmo::TranslationGizmo::GizmoComponentType> _t = {};
  std::optional<tinygizmo::RotationGizmo::GizmoComponentType> _r = {};
  std::optional<tinygizmo::ScalingGizmo::GizmoComponentType> _s = {};
  tinygizmo::RayState _ray_state;

  std::vector<Vector3> _positions;
  std::vector<Color> _colors;
  std::vector<unsigned short> _indices;

public:
  TRSGizmo(Camera *camera, std::list<std::shared_ptr<Drawable>> &scene)
      : _camera(camera), _scene(scene) {}

  void begin(const Vector2 &cursor) override;
  void end(const Vector2 &cursor) override;
  void drag(const DragState &state, int w, int h,
            const Vector2 &cursor) override;

  void hotkey(int w, int h, const Vector2 &cursor, const Hotkey &hotkey);
  void load(Drawable *drawable);
};
