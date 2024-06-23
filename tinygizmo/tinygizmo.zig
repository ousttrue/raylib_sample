const std = @import("std");
pub const ray = @import("ray.zig");
pub const minalg = @import("minalg.zig");
pub const rigid_transform = @import("rigid_transform.zig");

pub const gizmo_application_state = struct {
    mouse_left: bool = false,
    // If > 0.f, the gizmos are drawn scale-invariant with a screenspace value
    // defined here
    screenspace_scale: f32 = 0.0,
    // World-scale units used for snapping translation
    snap_translation: f32 = 0.0,
    // World-scale units used for snapping scale
    snap_scale: f32 = 0.0,
    // Radians used for snapping rotation quaternions (i.e. PI/8 or PI/16)
    snap_rotation: f32 = 0.0,
    // 3d viewport used to render the view
    viewport_size: [2]f32 = .{ 0, 0 },
    ray: ray.Ray = .{ .origin = .{}, .direction = .{} },
    cam_yfov: f32 = 0,
    cam_rotation: minalg.Quaternion = .{},

    // // This will calculate a scale constant based on the number of screenspace

    // pixels passed as pixel_scale.
    fn calc_scale_screenspace(self: @This(), position: minalg.Float3) f32 {
        const dist = position.subtract(self.ray.origin).length();
        return std.math.tan(self.cam_yfov) * dist *
            (self.screenspace_scale / self.viewport_size[1]);
    }

    pub fn scale_screenspace(self: @This(), position: minalg.Float3) f32 {
        return if (self.screenspace_scale > 0.0) self.calc_scale_screenspace(position) else 1.0;
    }
};

// fn detransform(p: rigid_transform.RigidTransform, r: ray.Ray) ray.Ray {
//     return .{
//         .origin = p.detransform_point(r.origin),
//         .direction = p.detransform_vector(r.direction),
//     };
// }

fn scaling(scale: f32, r: ray.Ray) ray.Ray {
    return .{
        .origin = r.origin * scale,
        .direction = r.direction * scale,
    };
}

fn descale(scale: f32, r: ray.Ray) ray.Ray {
    return .{
        .origin = r.origin.scale(1.0 / scale),
        .direction = r.direction.scale(1.0 / scale),
    };
}

// pub fn gizmo_transform(
//     active_state: gizmo_application_state,
//     local_toggle: bool,
//     src: rigid_transform.RigidTransform,
// ) struct { f32, rigid_transform.RigidTransform, ray.Ray } {
//     const draw_scale = active_state.scale_screenspace(src.position);
//     const p = rigid_transform.RigidTransform{
//         .orientation = if (local_toggle) src.orientation else minalg.Quaternion{},
//         .position = src.position,
//     };
//     var r = detransform(p, active_state.ray);
//     r = descale(draw_scale, r);
//     return .{ draw_scale, p, r };
// }
