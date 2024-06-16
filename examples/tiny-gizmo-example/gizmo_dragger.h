#pragma once
#include "drawable.h"
#include "rdrag.h"
#include <list>

class TranslationGizmoDragger : public Dragger {
  // dragging active gizmo
  tinygizmo::drag_state drag_state;
  std::shared_ptr<Drawable> gizmo_target;
  std::shared_ptr<tinygizmo::gizmo_mesh_component> active;
  std::list<std::shared_ptr<Drawable>> _scene;

public:
  tinygizmo::gizmo_state gizmo_state;
  TranslationGizmoDragger(std::list<std::shared_ptr<Drawable>> &scene)
      : _scene(scene) {}

  void begin(const Vector2 &cursor) override;
  void end(const Vector2 &end) override;
  void drag(const DragState &state, const Vector2 &cursor, int w,
            int h) override;
};

class GizmoManager {
  Camera *_camera;
  std::list<std::shared_ptr<Drawable>> _scene;

  hotkey active_hotkey{0};
  hotkey last_hotkey{0};
  transform_mode mode = transform_mode::translate;
  tinygizmo::gizmo_application_state active_state;
  tinygizmo::gizmo_application_state last_state;
  bool local_toggle{true};

  std::vector<Vector3> positions;
  std::vector<Color> colors;
  std::vector<unsigned short> indices;

  std::shared_ptr<TranslationGizmoDragger> _translation;

public:
  GizmoManager(Camera *camera, std::list<std::shared_ptr<Drawable>> &scene)
      : _camera(camera), _scene(scene) {}

  std::shared_ptr<Dragger> make_translation() {
    _translation = std::make_shared<TranslationGizmoDragger>(_scene);
    return _translation;
  }

  void hotkey(int w, int h, const Vector2 &cursor, const hotkey &hotkey);
  void load(Drawable *drawable);
};
