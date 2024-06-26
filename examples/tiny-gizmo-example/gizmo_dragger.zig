const std = @import("std");
const c = @import("c.zig");
const drawable = @import("drawable.zig");
const rdrag = @import("rdrag.zig");
const tinygizmo = @import("tinygizmo");

pub const TranslationGizmo = struct {

    // std::list<std::shared_ptr<Drawable>> _scene;
    // std::shared_ptr<Drawable> gizmo_target;
    active_component: ?*tinygizmo.GizmoComponent = null,
    // tinygizmo::drag_state drag_state;

    local_toggle: bool = true,
    gizmo_state: tinygizmo.FrameState = .{},

    pub fn begin(self: *@This(), cursor: c.Vector2) void {
        _ = cursor; // autofix
        _ = self; // autofix
        // float best_t = std::numeric_limits<float>::infinity();
        // std::unordered_map<std::shared_ptr<Drawable>, RayState> ray_map;
        // for (auto &target : this._scene) {
        //   minalg::rigid_transform src{
        //       .orientation = *(minalg::float4 *)&target.rotation,
        //       .position = *(minalg::float3 *)&target.position,
        //       .scale = *(minalg::float3 *)&target.scale,
        //   };
        //   auto [draw_scale, gizmo_transform, local_ray] =
        //       tinygizmo::gizmo_transform(gizmo_state, local_toggle, src);
        //   // ray intersection
        //   auto [updated_state, t] = tinygizmo::position_intersect(local_ray);
        //   if (updated_state) {
        //     ray_map.insert({target,
        //                     {
        //                         .transform = src,
        //                         .draw_scale = draw_scale,
        //                         .gizmo_transform = gizmo_transform,
        //                         .local_ray = local_ray,
        //                         .t = t,
        //                     }});
        //     if (t < best_t) {
        //       this.active = updated_state;
        //       this.gizmo_target = target;
        //     }
        //   }
        // }
        //
        // if (this.active) {
        //   // begin drag
        //   auto ray_state = ray_map[this.gizmo_target];
        //   auto ray = tinygizmo::scaling(ray_state.draw_scale, ray_state.local_ray);
        //   this.drag_state = {.original_position = ray_state.transform.position,
        //                       .click_offset = ray.point(ray_state.t)};
        //   if (local_toggle) {
        //     // click point in gizmo local
        //     this.drag_state.click_offset =
        //         ray_state.gizmo_transform.transform_vector(
        //             this.drag_state.click_offset);
        //   }
        // }
    }

    pub fn drag(
        self: *@This(),
        state: rdrag.DragState,
        w: i32,
        h: i32,
        cursor: c.Vector2,
    ) void {
        _ = cursor; // autofix
        _ = h; // autofix
        _ = w; // autofix
        _ = w; // autofix
        _ = state; // autofix
        _ = self; // autofix
    }

    pub fn end(self: *@This(), _: c.Vector2) void {
        _ = self; // autofix
        // self.active = .{};
        // self.gizmo_target = .{};
        // self.drag_state = .{};
    }

    //   virtual void draw(const tinygizmo::AddTriangleFunc &add_triangle,
    //                     const minalg::float4x4 &modelMatrix) = 0;
};

pub const Hotkey = struct {
    hotkey_ctrl: bool = false,
    hotkey_translate: bool = false,
    hotkey_rotate: bool = false,
    hotkey_scale: bool = false,
    hotkey_local: bool = false,
};

pub const TRSGizmo = struct {
    positions: std.ArrayList(c.Vector3),
    colors: std.ArrayList(c.Color),
    indices: std.ArrayList(c_ushort),

    camera: *c.Camera,
    scene: []*const drawable.Drawable,

    active_hotkey: Hotkey = .{},
    last_hotkey: Hotkey = .{},

    active_state: tinygizmo.FrameState = .{},
    last_state: tinygizmo.FrameState = .{},
    local_toggle: bool = true,

    //   std::shared_ptr<TranslationGizmo> _t,
    //   std::shared_ptr<RotationGizmo> _r,
    //   std::shared_ptr<ScalingGizmo> _s,
    translation: TranslationGizmo = .{},

    pub fn init(
        allocator: std.mem.Allocator,
        camera: *c.Camera,
        scene: []*const drawable.Drawable,
    ) @This() {
        return .{
            .positions = std.ArrayList(c.Vector3).init(allocator),
            .colors = std.ArrayList(c.Color).init(allocator),
            .indices = std.ArrayList(c_ushort).init(allocator),
            .camera = camera,
            .scene = scene,
        };
    }

    pub fn deinit(self: *@This()) void {
        self.positions.deinit();
        self.colors.deinit();
        self.indices.deinit();
    }

    pub fn begin(self: *@This(), cursor: c.Vector2) void {
        self.translation.begin(cursor);
    }

    pub fn end(self: *@This(), cursor: c.Vector2) void {
        self.translation.end(cursor);
    }

    pub fn drag(
        self: *@This(),
        state: rdrag.DragState,
        w: i32,
        h: i32,
        cursor: c.Vector2,
    ) void {
        self.translation.drag(state, w, h, cursor);
    }

    pub fn process_hotkey(self: *@This(), w: i32, h: i32, cursor: c.Vector2, hotkey: Hotkey) void {
        if (hotkey.hotkey_ctrl == true) {
            if (self.last_hotkey.hotkey_translate == false and
                hotkey.hotkey_translate == true)
            {
                // _active = _t;
            } else if (self.last_hotkey.hotkey_rotate == false and
                hotkey.hotkey_rotate == true)
            {
                // _active = _r;
            } else if (self.last_hotkey.hotkey_scale == false and
                hotkey.hotkey_scale == true)
            {
                // _active = _s;
            }

            self.local_toggle = if (!self.last_hotkey.hotkey_local and hotkey.hotkey_local)
                !self.local_toggle
            else
                self.local_toggle;
        }

        self.last_hotkey = self.active_hotkey;
        self.active_hotkey = hotkey;

        const ray = c.GetMouseRay(cursor, self.camera.*);
        const rot =
            c.QuaternionFromEuler(ray.direction.x, ray.direction.y, ray.direction.z);

        self.last_state = self.active_state;
        self.active_state = .{
            .mouse_down = c.IsMouseButtonDown(c.MOUSE_BUTTON_LEFT),
            // optional flag to draw the gizmos at a constant screen-space
            // scale gizmo_state.screenspace_scale = 80.f; camera projection
            .viewport_size = .{ .x = @floatFromInt(w), .y = @floatFromInt(h) },
            .ray = .{
                .origin = .{
                    .x = ray.position.x,
                    .y = ray.position.y,
                    .z = ray.position.z,
                },
                .direction = .{
                    .x = ray.direction.x,
                    .y = ray.direction.y,
                    .z = ray.direction.z,
                },
            },
            .cam_yfov = 1.0,
            .cam_orientation = .{ .x = rot.x, .y = rot.y, .z = rot.z, .w = rot.w },
        };

        self.translation.gizmo_state = self.active_state;
        self.translation.local_toggle = self.local_toggle;
    }

    fn add_world_triangle(
        user: *anyopaque,
        rgba: tinygizmo.Float4,
        p0: tinygizmo.Float3,
        p1: tinygizmo.Float3,
        p2: tinygizmo.Float3,
    ) void {
        const self = @as(*TRSGizmo, user);
        const offset = self._positions.size();
        const color = c.Color{
            .r = (std.math.max(0.0, rgba.x) * 255),
            .g = (std.math.max(0.0, rgba.y) * 255),
            .b = (std.math.max(0.0, rgba.z) * 255),
            .a = (std.math.max(0.0, rgba.w) * 255),
        };
        self._positions.push_back(.{ p0.x, p0.y, p0.z });
        self._positions.push_back(.{ p1.x, p1.y, p1.z });
        self._positions.push_back(.{ p2.x, p2.y, p2.z });
        self._colors.push_back(color);
        self._colors.push_back(color);
        self._colors.push_back(color);
        self._indices.push_back(offset + 0);
        self._indices.push_back(offset + 1);
        self._indices.push_back(offset + 2);
    }

    pub fn load(self: *@This(), _: *drawable.Drawable) void {
        self.positions.clearRetainingCapacity();
        self.colors.clearRetainingCapacity();
        self.indices.clearRetainingCapacity();

        for (self.scene) |target| {
            const src = tinygizmo.RigidTransform{
                .orientation = .{
                    .x = target.rotation.x,
                    .y = target.rotation.y,
                    .z = target.rotation.z,
                    .w = target.rotation.w,
                },
                .position = .{
                    .x = target.position.x,
                    .y = target.position.y,
                    .z = target.position.z,
                },
                .scale = .{
                    .x = target.scale.x,
                    .y = target.scale.y,
                    .z = target.scale.z,
                },
            };
            const draw_scale, const p, const ray =
                self.active_state.gizmo_transform_and_local_ray(self.local_toggle, src);
            _ = ray; // autofix

            const scaleMatrix =
                tinygizmo.Float4x4.scaling(draw_scale, draw_scale, draw_scale);
            const modelMatrix = p.matrix().mul(scaleMatrix);
            // _ = modelMatrix; // autofix

            tinygizmo.translation_draw(
                self,
                add_world_triangle,
                null,
                modelMatrix,
            );
        }

        // if (_positions.size() && _indices.size()) {
        //   drawable.load(_positions.size(), _positions.data(), _colors.data(),
        //                  _indices.size(), _indices.data(), true);
        // }
        //
    }
};
