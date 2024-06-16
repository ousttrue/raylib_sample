#pragma once
#include <memory>
#include <raylib.h>

/// drag system
/// l: t, r, s: t, r, s gizmo and selector
///   scene から active な transform を選択する
///   選択された transform を gizmo で操作する
///   keyboard shortcut で T, R, S 切り替え
///
///   on_selected(transform)
///   on_drag_begin
///   on_drag
///   on_drag_end
///
/// m: camera shift
///
/// r: camera yaw, pitch
///

struct DragState {
  Vector2 cursor_begin;
};

class Dragger {
public:
  virtual ~Dragger(){};

  virtual void begin(const Vector2 &cursor) { ; }

  virtual void end(const Vector2 &end) {
    //
  }

  virtual void drag(const DragState &state, const Vector2 &cursor, int w,
                    int h) {}
};

struct Drag {
  std::shared_ptr<Dragger> draggable;
  DragState state;
  bool _button = false;

  void process(int w, int h, const Vector2 &cursor, bool button) {
    if (_button != button) {
      if (button) {
        state = {
            .cursor_begin = cursor,
        };
        if (draggable) {
          draggable->begin(cursor);
        }
      } else {
        if (draggable) {
          draggable->end(cursor);
        }
      }
    } else {
      if (button) {
        DrawText(TextFormat("[%.0f, %.0f]", state.cursor_begin.x,
                            state.cursor_begin.y),
                 state.cursor_begin.x + 10, state.cursor_begin.y, 10, BLACK);
        DrawLine(state.cursor_begin.x, state.cursor_begin.y, cursor.x, cursor.y,
                 BLACK);
        DrawText(TextFormat("[%.0f, %.0f]", cursor.x, cursor.y), cursor.x + 10,
                 cursor.y, 10, BLACK);
        if (draggable) {
          draggable->drag(state, cursor, w, h);
        }
      } else {
        // nothing
      }
    }
    _button = button;
  }
};
