const std = @import("std");
const c = @import("c.zig");
const drawable = @import("drawable.zig");
const rdrag = @import("rdrag.zig");
const tinygizmo = @import("tinygizmo");

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

    gizmo: tinygizmo.System,

    pub fn init(
        allocator: std.mem.Allocator,
        camera: *c.Camera,
        scene: []*const drawable.Drawable,
    ) !@This() {
        return .{
            .positions = std.ArrayList(c.Vector3).init(allocator),
            .colors = std.ArrayList(c.Color).init(allocator),
            .indices = std.ArrayList(c_ushort).init(allocator),
            .camera = camera,
            .scene = scene,
            .gizmo = try tinygizmo.System.init(allocator),
        };
    }

    pub fn deinit(self: *@This()) void {
        self.gizmo.deinit();
        self.positions.deinit();
        self.colors.deinit();
        self.indices.deinit();
    }

    pub fn begin(self: *@This(), cursor: c.Vector2) void {
        _ = cursor; // autofix
        _ = self; // autofix
        // float best_t = std::numeric_limits<float>::infinity();
        // std::unordered_map<std::shared_ptr<Drawable>, RayState> ray_map;
        // for (auto &target : this->_scene) {
        //   tinygizmo::RigidTransform src{
        //       .orientation = *(tinygizmo::Quaternion *)&target->rotation,
        //       .position = *(tinygizmo::Float3 *)&target->position,
        //       .scale = *(tinygizmo::Float3 *)&target->scale,
        //   };
        //   auto [draw_scale, gizmo_transform, local_ray] =
        //       _active_state.gizmo_transform_and_local_ray(_local_toggle, src);
        //   // ray intersection
        //   auto [updated_state, t] = _visible->intersect(local_ray);
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
        //       this->_active = updated_state;
        //       this->_gizmo_target = target;
        //     }
        //   }
        // }
        //
        // if (this->_active) {
        //   // begin drag
        //   auto ray_state = ray_map[this->_gizmo_target];
        //   this->_drag_state = _visible->begin_gizmo(ray_state, _local_toggle);
        // }
    }

    pub fn end(self: *@This(), cursor: c.Vector2) void {
        _ = cursor; // autofix
        _ = self; // autofix
        // self._active = {};
        // self._gizmo_target = {};
        // self._drag_state = {};
    }

    pub fn drag(self: *@This(), state: rdrag.DragState, w: i32, h: i32, cursor: c.Vector2) void {
        _ = cursor; // autofix
        _ = h; // autofix
        _ = w; // autofix
        _ = state; // autofix
        _ = self; // autofix
        // for (auto &target : this->_scene) {
        //   if (this->_active && this->_gizmo_target == target) {
        //     _visible->drag(target, &_drag_state, _active, _active_state,
        //                    _local_toggle);
        //   }
        // }
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

        self.gizmo.gizmo_state = self.active_state;
        self.gizmo.local_toggle = self.local_toggle;
    }

    fn add_world_triangle(
        user: *anyopaque,
        rgba: tinygizmo.Float4,
        p0: tinygizmo.Float3,
        p1: tinygizmo.Float3,
        p2: tinygizmo.Float3,
    ) void {
        const self = @as(*TRSGizmo, @ptrCast(@alignCast(user)));
        const offset: c_ushort = @intCast(self.positions.items.len);
        const color = c.Color{
            .r = @intFromFloat(@max(0.0, rgba.x) * 255),
            .g = @intFromFloat(@max(0.0, rgba.y) * 255),
            .b = @intFromFloat(@max(0.0, rgba.z) * 255),
            .a = @intFromFloat(@max(0.0, rgba.w) * 255),
        };
        self.positions.append(.{ .x = p0.x, .y = p0.y, .z = p0.z }) catch @panic("append");
        self.positions.append(.{ .x = p1.x, .y = p1.y, .z = p1.z }) catch @panic("append");
        self.positions.append(.{ .x = p2.x, .y = p2.y, .z = p2.z }) catch @panic("append");
        self.colors.append(color) catch @panic("append");
        self.colors.append(color) catch @panic("append");
        self.colors.append(color) catch @panic("append");
        self.indices.append(offset + 0) catch @panic("append");
        self.indices.append(offset + 1) catch @panic("append");
        self.indices.append(offset + 2) catch @panic("append");
    }

    pub fn load(self: *@This(), dst: *drawable.Drawable) void {
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

            self.gizmo.translation_draw(
                self,
                add_world_triangle,
                null,
                modelMatrix,
            );
        }

        if (self.positions.items.len > 0 and self.indices.items.len > 0) {
            dst.load_slice(self.positions.items, self.colors.items, self.indices.items, true);
        }
    }
};
