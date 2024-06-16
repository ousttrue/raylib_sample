#pragma once
#include "drawable.h"
#include "rdrag.h"
#include <list>

class TranslationGizmoDragger : public Dragger {
  Camera *_camera;
  std::list<std::shared_ptr<Drawable>> _scene;

  std::vector<Vector3> positions;
  std::vector<Color> colors;
  std::vector<unsigned short> indices;

  // dragging active gizmo
  tinygizmo::drag_state drag_state;
  std::shared_ptr<tinygizmo::gizmo_mesh_component> gizmo;
  std::shared_ptr<Drawable> gizmo_target;

public:
  transform_mode mode = transform_mode::translate;
  hotkey active_hotkey{0};
  hotkey last_hotkey{0};
  tinygizmo::gizmo_application_state active_state;
  tinygizmo::gizmo_application_state last_state;
  bool local_toggle{true};

public:
  TranslationGizmoDragger(Camera *camera,
                          std::list<std::shared_ptr<Drawable>> &scene)
      : _camera(camera), _scene(scene) {}
  void begin(const Vector2 &cursor) override;
  void end(const Vector2 &end) override;
  void drag(const DragState &state, const Vector2 &cursor, int w,
            int h) override;
  void hotkey(int w, int h, const Vector2 &cursor, const hotkey &hotkey);
  void load(Drawable *drawable);
};
