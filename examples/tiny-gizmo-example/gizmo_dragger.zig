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

pub const RayState = struct {
    transform: tinygizmo.RigidTransform,
    draw_scale: f32,
    gizmo_transform: tinygizmo.RigidTransform,
    local_ray: tinygizmo.Ray,
    t: f32,
};

pub const GizmoModeType = enum {
    Translation,
    Rotation,
    Scaling,
};

pub const GizmoMode = union(GizmoModeType) {
    Translation: struct {
        fn draw(_: @This(), component: tinygizmo.GizmoComponent, user: *anyopaque, add_triangle: tinygizmo.AddTriangleFunc, modelMatrix: tinygizmo.Float4x4) void {
            tinygizmo.position_draw(user, add_triangle, component, modelMatrix);
        }

        fn intersect(_: @This(), gizmo_system: tinygizmo.System, local_ray: tinygizmo.Ray) struct { ?tinygizmo.GizmoComponent, f32 } {
            return gizmo_system.translation_intersect(local_ray);
        }

        fn begin_gizmo(_: @This(), ray_state: RayState, local_toggle: bool) tinygizmo.DragState {
            const ray = ray_state.local_ray.scaling(ray_state.draw_scale);
            var drag_state = tinygizmo.DragState{
                .click_offset = ray.point(ray_state.t),
                .original_position = ray_state.transform.position,
            };
            if (local_toggle) {
                drag_state.click_offset =
                    ray_state.gizmo_transform.transform_vector(drag_state.click_offset);
            }
            return drag_state;
        }

        fn drag(
            _: @This(),
            target: *drawable.Drawable,
            state: *tinygizmo.DragState,
            component: tinygizmo.GizmoComponent,
            frame: tinygizmo.FrameState,
            local_toggle: bool,
        ) void {
            const src = tinygizmo.RigidTransform{
                .orientation = target.rotation,
                .position = target.position,
                .scale = target.scale,
            };
            const position = tinygizmo.position_drag(state, frame, local_toggle, component, src);
            target.position = position;
        }
    },

    Rotation: struct {
        fn draw(
            _: @This(),
            component: tinygizmo.GizmoComponent,
            user: *anyopaque,
            add_triangle: tinygizmo.AddTriangleFunc,
            modelMatrix: tinygizmo.Float4x4,
        ) void {
            tinygizmo.rotation_draw(user, add_triangle, component, modelMatrix);
        }

        fn intersect(_: @This(), local_ray: tinygizmo.Ray) struct { ?tinygizmo.GizmoComponent, f32 } {
            _ = local_ray; // autofix
            // return tinygizmo.rotation_intersect(local_ray);
            return .{ null, 0 };
        }

        fn begin_gizmo(_: @This(), ray_state: RayState, _: bool) tinygizmo.DragState {
            const ray = ray_state.local_ray.scaling(ray_state.draw_scale);
            const drag_state = tinygizmo.DragState{
                .click_offset = ray_state.transform.transform_point(ray.origin.add(ray.direction.scale(ray_state.t))),
                .original_orientation = ray_state.transform.orientation,
            };
            return drag_state;
        }

        fn drag(
            _: @This(),
            target: *drawable.Drawable,
            state: tinygizmo.DragState,
            component: tinygizmo.GizmoComponent,
            frame: tinygizmo.FrameState,
            local_toggle: bool,
        ) void {
            const src = tinygizmo.RigidTransform{
                .orientation = target.rotation,
                .position = target.position,
                .scale = target.scale,
            };
            const rotation =
                tinygizmo.rotation_drag(state, frame, local_toggle, component, src);
            target.rotation = rotation;
        }
    },

    Scaling: struct {
        uniform: bool = false,

        fn draw(_: @This(), component: tinygizmo.GizmoComponent, user: anyopaque, add_triangle: tinygizmo.AddTriangleFunc, modelMatrix: tinygizmo.Float4x4) void {
            tinygizmo.scaling_draw(user, add_triangle, component, modelMatrix);
        }

        fn intersect(_: @This(), local_ray: tinygizmo.Ray) struct { ?tinygizmo.GizmoComponent, f32 } {
            _ = local_ray; // autofix
            // return tinygizmo.scaling_intersect(local_ray);
            return .{ null, 0 };
        }

        fn begin_gizmo(_: @This(), ray_state: RayState, _: bool) tinygizmo.DragState {
            const ray = ray_state.local_ray.scaling(ray_state.draw_scale);
            const drag_state = tinygizmo.DragState{
                .click_offset = ray_state.transform.transform_point(ray.origin.add(ray.direction.scale(ray_state.t))),
                .original_scale = ray_state.transform.scale,
            };
            return drag_state;
        }

        fn drag(
            self: @This(),
            target: *drawable.Drawable,
            state: *tinygizmo.DragState,
            component: tinygizmo.GizmoComponent,
            frame: tinygizmo.FrameState,
            local_toggle: bool,
        ) void {
            const src = tinygizmo.RigidTransform{
                .orientation = target.rotation,
                .position = target.position,
                .scale = target.scale,
            };
            const scale = tinygizmo.scaling_drag(state, frame, local_toggle, component, src, self.uniform);
            target.scale = scale;
        }
    },
};

pub const TRSGizmo = struct {
    positions: std.ArrayList(c.Vector3),
    colors: std.ArrayList(c.Color),
    indices: std.ArrayList(c_ushort),

    camera: *c.Camera,
    scene: []*drawable.Drawable,

    active_hotkey: Hotkey = .{},
    last_hotkey: Hotkey = .{},

    active_state: tinygizmo.FrameState = .{},
    last_state: tinygizmo.FrameState = .{},
    local_toggle: bool = true,

    visible: GizmoMode,
    gizmo_system: tinygizmo.System,
    ray_map: std.AutoHashMap(*drawable.Drawable, RayState),

    gizmo_target: ?*drawable.Drawable = null,
    active: ?tinygizmo.GizmoComponent = null,
    drag_state: tinygizmo.DragState = .{},

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
            .visible = .{ .Translation = .{} },
            .gizmo_system = try tinygizmo.System.init(allocator),
            .ray_map = std.AutoHashMap(*drawable.Drawable, RayState).init(allocator),
        };
    }

    pub fn deinit(self: *@This()) void {
        self.ray_map.deinit();
        self.gizmo_system.deinit();
        self.positions.deinit();
        self.colors.deinit();
        self.indices.deinit();
    }

    pub fn begin(self: *@This(), _: c.Vector2) void {
        var best_t = std.math.inf(f32);
        self.ray_map.clearRetainingCapacity();
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
            const draw_scale, const gizmo_transform, const local_ray = self.active_state.gizmo_transform_and_local_ray(
                self.local_toggle,
                src,
            );
            // ray intersection
            const updated_state, const t = switch (self.visible) {
                GizmoModeType.Translation => |x| x.intersect(self.gizmo_system, local_ray),
                GizmoModeType.Rotation => |x| x.intersect(local_ray),
                GizmoModeType.Scaling => |x| x.intersect(local_ray),
            };
            if (updated_state) |active| {
                self.ray_map.put(target, .{
                    .transform = src,
                    .draw_scale = draw_scale,
                    .gizmo_transform = gizmo_transform,
                    .local_ray = local_ray,
                    .t = t,
                }) catch @panic("put");
                if (t < best_t) {
                    best_t = t;
                    self.active = active;
                    self.gizmo_target = target;
                }
            }
        }

        if (self.gizmo_target) |target| {
            // begin drag
            if (self.ray_map.getEntry(target)) |entry| {
                _ = switch (self.visible) {
                    GizmoModeType.Translation => |x| self.drag_state = x.begin_gizmo(entry.value_ptr.*, self.local_toggle),
                    GizmoModeType.Rotation => |x| x.begin_gizmo(entry.value_ptr.*, self.local_toggle),
                    GizmoModeType.Scaling => |x| x.begin_gizmo(entry.value_ptr.*, self.local_toggle),
                };
                // self.drag_state = _visible->begin_gizmo(ray_state, _local_toggle);
            }
        }
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

        self.gizmo_system.gizmo_state = self.active_state;
        self.gizmo_system.local_toggle = self.local_toggle;
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

            self.gizmo_system.translation_draw(
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
