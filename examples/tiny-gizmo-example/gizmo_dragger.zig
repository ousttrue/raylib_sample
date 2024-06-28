const std = @import("std");
const c = @import("c.zig");
const drawable = @import("drawable.zig");
const rdrag = @import("rdrag.zig");
const tinygizmo = @import("tinygizmo");

pub const GizmoMode = enum {
    Translation,
    Rotation,
    Scaling,
};

pub const GizmoActiveComponent = union(GizmoMode) {
    Translation: ?tinygizmo.TranslationGizmo.GizmoComponentType,
    Rotation: ?tinygizmo.RotationGizmo.GizmoComponentType,
    Scaling: ?tinygizmo.ScalingGizmo.GizmoComponentType,
};

pub const Hotkey = struct {
    hotkey_ctrl: bool = false,
    hotkey_translate: bool = false,
    hotkey_rotate: bool = false,
    hotkey_scale: bool = false,
    hotkey_local: bool = false,
};

pub const TRSGizmo = struct {
    mode: GizmoActiveComponent = .{ .Translation = null },
    camera: *c.Camera,
    scene: []*drawable.Drawable,
    gizmo_target: ?*drawable.Drawable = null,

    current_hotkey: Hotkey = .{},
    last_hotkey: Hotkey = .{},
    current_state: tinygizmo.FrameState = .{},
    last_state: tinygizmo.FrameState = .{},
    local_toggle: bool = true,
    uniform: bool = true,

    ray_state: tinygizmo.RayState = .{},

    positions: std.ArrayList(c.Vector3),
    colors: std.ArrayList(c.Color),
    indices: std.ArrayList(c_ushort),

    pub fn init(
        allocator: std.mem.Allocator,
        camera: *c.Camera,
        scene: []*drawable.Drawable,
    ) !@This() {
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

    pub fn begin(self: *@This(), _: c.Vector2) void {
        var best_t = std.math.inf(f32);
        for (self.scene) |target| {
            //
            // ray intersection
            //
            switch (self.mode) {
                .Translation => {
                    if (tinygizmo.TranslationGizmo.intersect(
                        self.current_state,
                        self.local_toggle,
                        target.transform,
                    )) |intersection| {
                        const ray_state, const active_component = intersection;
                        if (ray_state.t < best_t) {
                            best_t = ray_state.t;
                            self.mode = .{ .Translation = active_component };
                            self.gizmo_target = target;
                            self.ray_state = ray_state;
                        }
                    }
                },

                .Rotation => {
                    if (tinygizmo.RotationGizmo.intersect(
                        self.current_state,
                        self.local_toggle,
                        target.transform,
                    )) |intersection| {
                        const ray_state, const active_component = intersection;
                        if (ray_state.t < best_t) {
                            best_t = ray_state.t;
                            self.mode = .{ .Rotation = active_component };
                            self.gizmo_target = target;
                            self.ray_state = ray_state;
                        }
                    }
                },

                .Scaling => {
                    if (tinygizmo.ScalingGizmo.intersect(
                        self.current_state,
                        self.local_toggle,
                        target.transform,
                        self.uniform,
                    )) |intersection| {
                        const ray_state, const active_component = intersection;
                        if (ray_state.t < best_t) {
                            best_t = ray_state.t;
                            self.mode = .{ .Scaling = active_component };
                            self.gizmo_target = target;
                            self.ray_state = ray_state;
                        }
                    }
                },
            }
        }
    }

    pub fn end(self: *@This(), _: c.Vector2) void {
        self.mode = switch (self.mode) {
            .Translation => .{ .Translation = null },
            .Rotation => .{ .Rotation = null },
            .Scaling => .{ .Scaling = null },
        };
        self.gizmo_target = null;
        self.ray_state = .{};
    }

    pub fn drag(
        self: *@This(),
        _: rdrag.DragState,
        _: i32,
        _: i32,
        _: c.Vector2,
    ) void {
        if (self.gizmo_target) |target| {
            switch (self.mode) {
                .Translation => |t| {
                    if (t) |active_component| {
                        target.transform = tinygizmo.TranslationGizmo.drag(
                            active_component,
                            self.current_state,
                            self.ray_state,
                            target.transform,
                        );
                    }
                },

                .Rotation => |r| {
                    if (r) |active_component| {
                        target.transform = tinygizmo.RotationGizmo.drag(
                            active_component,
                            self.current_state,
                            self.ray_state,
                            target.transform,
                        );
                    }
                },

                .Scaling => |s| {
                    if (s) |active_component| {
                        target.transform = tinygizmo.ScalingGizmo.drag(
                            active_component,
                            self.current_state,
                            self.ray_state,
                            target.transform,
                        );
                    }
                },
            }
        }
    }

    pub fn process_hotkey(self: *@This(), w: i32, h: i32, cursor: c.Vector2, hotkey: Hotkey) void {
        if (hotkey.hotkey_ctrl == true) {
            if (self.last_hotkey.hotkey_translate == false and
                hotkey.hotkey_translate == true)
            {
                self.mode = .{ .Translation = null };
            } else if (self.last_hotkey.hotkey_rotate == false and
                hotkey.hotkey_rotate == true)
            {
                self.mode = .{ .Rotation = null };
            } else if (self.last_hotkey.hotkey_scale == false and
                hotkey.hotkey_scale == true)
            {
                self.mode = .{ .Scaling = null };
            }

            self.local_toggle = if (!self.last_hotkey.hotkey_local and hotkey.hotkey_local)
                !self.local_toggle
            else
                self.local_toggle;
        }

        self.last_hotkey = self.current_hotkey;
        self.current_hotkey = hotkey;

        const ray = c.GetMouseRay(cursor, self.camera.*);
        const rot =
            c.QuaternionFromEuler(ray.direction.x, ray.direction.y, ray.direction.z);

        self.last_state = self.current_state;
        self.current_state = .{
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
    }

    fn add_triangle(
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
            const draw_scale, const p, const ray =
                self.current_state.gizmo_transform_and_local_ray(self.local_toggle, target.transform);
            _ = ray; // autofix

            const scaleMatrix =
                tinygizmo.Float4x4.scaling(draw_scale, draw_scale, draw_scale);
            const gizmoMatrix = p.matrix().mul(scaleMatrix);

            switch (self.mode) {
                .Translation => |_t| {
                    tinygizmo.TranslationGizmo.mesh(gizmoMatrix, self, add_triangle, _t);
                },

                .Rotation => |_r| {
                    tinygizmo.RotationGizmo.mesh(gizmoMatrix, self, add_triangle, _r);
                },

                .Scaling => |_s| {
                    tinygizmo.ScalingGizmo.mesh(gizmoMatrix, self, add_triangle, _s);
                },
            }
        }

        if (self.positions.items.len > 0 and self.indices.items.len > 0) {
            dst.load_slice(
                self.positions.items,
                self.colors.items,
                self.indices.items,
                true,
            );
        }
    }
};
