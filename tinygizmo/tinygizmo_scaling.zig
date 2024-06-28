const std = @import("std");
const alg = @import("tinygizmo_alg.zig");
const geometrymesh = @import("tinygizmo_geometrymesh.zig");
const GeometryMesh = geometrymesh.GeometryMesh;
const state = @import("tinygizmo_state.zig");

pub const GizmoComponentType = enum {
    ScalingX,
    ScalingY,
    ScalingZ,
};

const mace_points = [_]alg.Float2{
    .{ .x = 0.25, .y = 0 },
    .{ .x = 0.25, .y = 0.05 },
    .{ .x = 1, .y = 0.05 },
    .{ .x = 1, .y = 0.1 },
    .{ .x = 1.25, .y = 0.1 },
    .{ .x = 1.25, .y = 0 },
};

const gizmo_components = [_]struct { GizmoComponentType, geometrymesh.GeometryMesh }{
    .{ .ScalingX, GeometryMesh.make_lathed(
        .{ .x = 1, .y = 0, .z = 0 },
        .{ .x = 0, .y = 1, .z = 0 },
        .{ .x = 0, .y = 0, .z = 1 },
        16,
        &mace_points,
        .{ .x = 1, .y = 0.5, .z = 0.5, .w = 1.0 },
        .{ .x = 1, .y = 0, .z = 0, .w = 1.0 },
        0,
    ) },
    .{ .ScalingY, GeometryMesh.make_lathed(
        .{ .x = 0, .y = 1, .z = 0 },
        .{ .x = 0, .y = 0, .z = 1 },
        .{ .x = 1, .y = 0, .z = 0 },
        16,
        &mace_points,
        .{ .x = 0.5, .y = 1, .z = 0.5, .w = 1.0 },
        .{ .x = 0, .y = 1, .z = 0, .w = 1.0 },
        0,
    ) },
    .{ .ScalingZ, GeometryMesh.make_lathed(
        .{ .x = 0, .y = 0, .z = 1 },
        .{ .x = 1, .y = 0, .z = 0 },
        .{ .x = 0, .y = 1, .z = 0 },
        16,
        &mace_points,
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
    uniform: bool,
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
            .uniform = uniform,
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

fn flush_to_zero(f: *alg.Float3) void {
    if (@abs(f.x) < 0.02)
        f.x = 0.0;
    if (@abs(f.y) < 0.02)
        f.y = 0.0;
    if (@abs(f.z) < 0.02)
        f.z = 0.0;
}

fn axis_scale_dragger(frame_state: state.FrameState, ray_state: state.RayState, axis: alg.Float3, src: alg.Transform) ?alg.Float3 {
    const plane_tangent = alg.Float3.cross(axis, src.position.sub(frame_state.ray.origin));
    const plane_normal = alg.Float3.cross(axis, plane_tangent);

    // Define the plane to contain the original position of the object
    const plane_point = src.position;

    // If an intersection exists between the ray and the plane, place the
    // object at that point
    const denom = alg.Float3.dot(frame_state.ray.direction, plane_normal);
    if (@abs(denom) == 0) {
        return null;
    }

    const t = alg.Float3.dot(plane_point.sub(frame_state.ray.origin), plane_normal) / denom;
    if (t < 0) {
        return null;
    }

    const distance = frame_state.ray.point(t);

    var offset_on_axis = (distance.sub(get_click_offset(ray_state))).mult_each(axis);
    flush_to_zero(&offset_on_axis);
    const new_scale = ray_state.transform.scale.add(offset_on_axis);

    const scale = if (ray_state.uniform)
        alg.Float3{
            .x = std.math.clamp(alg.Float3.dot(distance, new_scale), 0.01, 1000.0),
            .y = std.math.clamp(alg.Float3.dot(distance, new_scale), 0.01, 1000.0),
            .z = std.math.clamp(alg.Float3.dot(distance, new_scale), 0.01, 1000.0),
        }
    else
        alg.Float3{
            .x = std.math.clamp(new_scale.x, 0.01, 1000.0),
            .y = std.math.clamp(new_scale.y, 0.01, 1000.0),
            .z = std.math.clamp(new_scale.z, 0.01, 1000.0),
        };
    return scale;
}

pub fn drag(
    active_component: GizmoComponentType,
    frame: state.FrameState,
    ray_state: state.RayState,
    src: alg.Transform,
) alg.Transform {
    const _src = alg.Transform{
        .orientation = alg.Quaternion.identity,
        .position = src.position,
        .scale = src.scale,
    };
    const drag_scale = switch (active_component) {
        .ScalingX => axis_scale_dragger(frame, ray_state, alg.Float3.x_axis, _src),
        .ScalingY => axis_scale_dragger(frame, ray_state, alg.Float3.y_axis, _src),
        .ScalingZ => axis_scale_dragger(frame, ray_state, alg.Float3.z_axis, _src),
    };

    if (drag_scale) |scale| {
        var dst = src;
        dst.scale = scale;
        return dst;
    } else {
        return src;
    }
}
