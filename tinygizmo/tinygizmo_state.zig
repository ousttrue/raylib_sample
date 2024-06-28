const std = @import("std");
const alg = @import("tinygizmo_alg.zig");

pub const FrameState = struct {
    mouse_down: bool = false,
    // If > 0.f, the gizmos are drawn scale-invariant with a screenspace value
    // defined here
    screenspace_scale: f32 = 0,
    viewport_size: alg.Float2 = .{ .x = 0, .y = 0 },
    ray: alg.Ray = .{ .origin = .{ .x = 0, .y = 0, .z = 0 }, .direction = .{ .x = 0, .y = 0, .z = 0 } },
    cam_yfov: f32 = 0,
    cam_orientation: alg.Quaternion = .{},

    // This will calculate a scale constant based on the number of screenspace
    // pixels passed as pixel_scale.
    pub fn calc_scale_screenspace(self: @This(), position: alg.Float3) f32 {
        const dist = position.sub(self.ray.origin).length();
        return std.math.tan(self.cam_yfov) * dist *
            (self.screenspace_scale / self.viewport_size.y);
    }

    pub fn scale_screenspace(self: @This(), position: alg.Float3) f32 {
        return if (self.screenspace_scale > 0.0) self.calc_scale_screenspace(position) else 1.0;
    }

    pub fn gizmo_transform_and_local_ray(
        self: @This(),
        local_toggle: bool,
        src: alg.Transform,
    ) struct { f32, alg.Transform, alg.Ray } {
        const draw_scale = self.scale_screenspace(src.position);
        const p = alg.Transform{
            .orientation = if (local_toggle) src.orientation else alg.Quaternion{},
            .position = src.position,
        };
        var ray = self.ray.detransform(p);
        ray = ray.descale(draw_scale);
        return .{ draw_scale, p, ray };
    }
};

pub const RayState = struct {
    local_toggle: bool = false,
    uniform: bool = false,
    transform: alg.Transform = .{},
    draw_scale: f32 = 0,
    gizmo_transform: alg.Transform = .{},
    local_ray: alg.Ray = .{
        .origin = .{ .x = 0, .y = 0, .z = 0 },
        .direction = .{ .x = 0, .y = 0, .z = 0 },
    },
    t: f32 = 0,
};
