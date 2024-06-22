const c = @import("c.zig");
const drwable = @import("drawable.zig");
const rdrag = @import("rdrag.zig");

pub const TRSGizmo = struct {
    _camera: *c.Camera,
    _scene: []*const drwable.Drawable,

    //   hotkey active_hotkey{0};
    //   hotkey last_hotkey{0};
    //   tinygizmo::gizmo_application_state active_state;
    //   tinygizmo::gizmo_application_state last_state;
    //   bool local_toggle{true};
    //
    //   std::shared_ptr<TranslationGizmo> _t;
    //   std::shared_ptr<RotationGizmo> _r;
    //   std::shared_ptr<ScalingGizmo> _s;
    //   std::shared_ptr<Gizmo> _active;
    //
    //   std::vector<Vector3> positions;
    //   std::vector<Color> colors;
    //   std::vector<unsigned short> indices;
    //
    // public:
    //   TRSGizmo(Camera *camera, std::list<std::shared_ptr<Drawable>> &scene)
    //       : _camera(camera), _scene(scene) {
    //     _t = std::make_shared<TranslationGizmo>(_scene);
    //     _r = std::make_shared<RotationGizmo>(_scene);
    //     _s = std::make_shared<ScalingGizmo>(_scene);
    //     _active = _t;
    //   }

    pub fn begin(self: *@This(), cursor: c.Vector2) void {
        _ = cursor; // autofix
        _ = self; // autofix
        //
        // self._active.begin(cursor);
    }
    pub fn end(self: *@This(), cursor: c.Vector2) void {
        _ = cursor; // autofix
        _ = self; // autofix
        //
        // self._active.end(cursor);
    }
    pub fn drag(self: *@This(), state: rdrag.DragState, w: i32, h: i32, cursor: c.Vector2) void {
        _ = cursor; // autofix
        _ = h; // autofix
        _ = w; // autofix
        _ = state; // autofix
        _ = self; // autofix
        //
        // self._active.drag(state, w, h, cursor);
    }

    //   void hotkey(int w, int h, const Vector2 &cursor, const hotkey &hotkey);
    //   void load(Drawable *drawable);
};
