const std = @import("std");
const alg = @import("tinygizmo_alg.zig");
const geometrymesh = @import("tinygizmo_geometrymesh.zig");
const GeometryMesh = geometrymesh.GeometryMesh;
const state = @import("tinygizmo_state.zig");

pub const GizmoComponentType = enum {
    RotationX,
    RotationY,
    RotationZ,
};

const ring_points = [_]alg.Float2{
    .{ .x = 0.025, .y = 1 },
    .{ .x = -0.025, .y = 1 },
    .{ .x = -0.025, .y = 1 },
    .{ .x = -0.025, .y = 1.1 },
    .{ .x = -0.025, .y = 1.1 },
    .{ .x = 0.025, .y = 1.1 },
    .{ .x = 0.025, .y = 1.1 },
    .{ .x = 0.025, .y = 1 },
};

const gizmo_components = [_]struct { GizmoComponentType, GeometryMesh }{
    .{ .RotationX, GeometryMesh.make_lathed(
        .{ .x = 1, .y = 0, .z = 0 },
        .{ .x = 0, .y = 1, .z = 0 },
        .{ .x = 0, .y = 0, .z = 1 },
        32,
        &ring_points,
        .{ .x = 1, .y = 0.5, .z = 0.5, .w = 1.0 },
        .{ .x = 1, .y = 0, .z = 0, .w = 1.0 },
        0.003,
    ) },
    .{ .RotationY, GeometryMesh.make_lathed(
        .{ .x = 0, .y = 1, .z = 0 },
        .{ .x = 0, .y = 0, .z = 1 },
        .{ .x = 1, .y = 0, .z = 0 },
        32,
        &ring_points,
        .{ .x = 0.5, .y = 1, .z = 0.5, .w = 1.0 },
        .{ .x = 0, .y = 1, .z = 0, .w = 1.0 },
        -0.003,
    ) },
    .{ .RotationZ, GeometryMesh.make_lathed(
        .{ .x = 0, .y = 0, .z = 1 },
        .{ .x = 1, .y = 0, .z = 0 },
        .{ .x = 0, .y = 1, .z = 0 },
        32,
        &ring_points,
        .{ .x = 0.5, .y = 0.5, .z = 1, .w = 1.0 },
        .{ .x = 0, .y = 0, .z = 1, .w = 1.0 },
        0,
    ) },
};

pub fn mesh(
    modelMatrix: alg.Float4x4,
    user: *anyopaque,
    add_triangle: geometrymesh.AddTriangleFunc,
    active_component: ?GizmoComponentType,
) void {
    for (gizmo_components) |entry| {
        const component, const geometry = entry;
        geometry.add_triangles(
            user,
            add_triangle,
            modelMatrix,
            if (component == active_component) geometry.base_color else geometry.highlight_color,
        );
    }
}

pub fn intersect(
    frame: state.FrameState,
    local_toggle: bool,
    p: alg.Transform,
) ?struct { state.RayState, GizmoComponentType } {
    const draw_scale, const gizmo_transform, const local_ray = frame.gizmo_transform_and_local_ray(local_toggle, p);
    var best_t = std.math.inf(f32);
    var active_component: ?GizmoComponentType = null;
    for (gizmo_components) |entry| {
        const component, const geometry = entry;
        const t = geometry.intersect(local_ray);
        if (t < best_t) {
            active_component = component;
            best_t = t;
        }
    }
    if (active_component) |component| {
        return .{ .{
            .local_toggle = local_toggle,
            .uniform = false,
            .transform = p,
            .draw_scale = draw_scale,
            .gizmo_transform = gizmo_transform,
            .local_ray = local_ray,
            .t = best_t,
        }, component };
    } else {
        return null;
    }
}

fn get_click_offset(ray_state: state.RayState) alg.Float3 {
    const ray = ray_state.local_ray.scaling(ray_state.draw_scale);
    return ray_state.transform.transform_point(ray.origin.add(ray.direction.scale(ray_state.t)));
}

fn axis_rotation_dragger(
    frame_state: state.FrameState,
    ray_state: state.RayState,
    axis: alg.Float3,
    _: alg.Transform,
) ?alg.Quaternion {
    const start_orientation = if (ray_state.local_toggle) ray_state.transform.orientation else alg.Quaternion{ .x = 0, .y = 0, .z = 0, .w = 1 };

    const original_pose = alg.Transform{
        .orientation = start_orientation,
        .position = ray_state.transform.position,
    };
    const the_axis = original_pose.transform_vector(axis);
    const the_plane = alg.Plane.from_normal_and_position(the_axis, get_click_offset(ray_state));

    if (frame_state.ray.intersect_plane(the_plane)) |t| {
        const center_of_rotation =
            ray_state.transform.position.add(the_axis.scale(alg.Float3.dot(the_axis, get_click_offset(ray_state).sub(ray_state.transform.position))));
        const arm1 = (get_click_offset(ray_state).sub(center_of_rotation)).normalize();
        const arm2 = frame_state.ray.point(t).sub(center_of_rotation).normalize();

        const d = alg.Float3.dot(arm1, arm2);
        if (d > 0.999) {
            return null;
        }

        const angle = std.math.acos(d);
        if (angle < 0.001) {
            return null;
        }

        const a = alg.Float3.cross(arm1, arm2).normalize();
        return alg.Quaternion.from_axis_angle(a, angle).mul(start_orientation);
    } else {
        return null;
    }
}

pub fn drag(
    active_component: GizmoComponentType,
    frame_state: state.FrameState,
    ray_state: state.RayState,
    src: alg.Transform,
) alg.Transform {
    const rotation = switch (active_component) {
        .RotationX => axis_rotation_dragger(frame_state, ray_state, .{ .x = 1, .y = 0, .z = 0 }, src),
        .RotationY => axis_rotation_dragger(frame_state, ray_state, .{ .x = 0, .y = 1, .z = 0 }, src),
        .RotationZ => axis_rotation_dragger(frame_state, ray_state, .{ .x = 0, .y = 0, .z = 1 }, src),
    };
    if (rotation) |r| {
        var dst = src;
        dst.orientation = r;
        return dst;
    } else {
        return src;
    }
}
