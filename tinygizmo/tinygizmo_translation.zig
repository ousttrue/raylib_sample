const alg = @import("tinygizmo_alg.zig");
const state = @import("tinygizmo_state.zig");
const std = @import("std");

pub const GizmoComponentType = enum {
    TranslationX,
    TranslationY,
    TranslationZ,
    TranslationXY,
    TranslationYZ,
    TranslationZX,
    TranslationView,
};

const arrow_points = [_]alg.Float2{
    .{ .x = 0.25, .y = 0 },
    .{ .x = 0.25, .y = 0.05 },
    .{ .x = 1, .y = 0.05 },
    .{ .x = 1, .y = 0.10 },
    .{ .x = 1.2, .y = 0 },
};

const gizmo_components = [_]struct { GizmoComponentType, alg.GeometryMesh }{
    .{ .TranslationX, alg.GeometryMesh.make_lathed(
        .{ .x = 1, .y = 0, .z = 0 },
        .{ .x = 0, .y = 1, .z = 0 },
        .{ .x = 0, .y = 0, .z = 1 },
        16,
        &arrow_points,
        0,
        .{ .x = 1, .y = 0.5, .z = 0.5, .w = 1.0 },
        .{ .x = 1, .y = 0, .z = 0, .w = 1.0 },
    ) },

    .{ .TranslationY, alg.GeometryMesh.make_lathed(
        .{ .x = 0, .y = 1, .z = 0 },
        .{ .x = 0, .y = 0, .z = 1 },
        .{ .x = 1, .y = 0, .z = 0 },
        16,
        &arrow_points,
        0,
        .{ .x = 0.5, .y = 1, .z = 0.5, .w = 1.0 },
        .{ .x = 0, .y = 1, .z = 0, .w = 1.0 },
    ) },

    .{ .TranslationZ, alg.GeometryMesh.make_lathed(
        .{ .x = 0, .y = 0, .z = 1 },
        .{ .x = 1, .y = 0, .z = 0 },
        .{ .x = 0, .y = 1, .z = 0 },
        16,
        &arrow_points,
        0,
        .{ .x = 0.5, .y = 0.5, .z = 1, .w = 1.0 },
        .{ .x = 0, .y = 0, .z = 1, .w = 1.0 },
    ) },
    .{ .TranslationYZ, alg.GeometryMesh.make_box(
        .{ .x = -0.01, .y = 0.25, .z = 0.25 },
        .{ .x = 0.01, .y = 0.75, .z = 0.75 },
        .{ .x = 0.5, .y = 1, .z = 1, .w = 0.5 },
        .{ .x = 0, .y = 1, .z = 1, .w = 0.6 },
    ) },
    .{ .TranslationZX, alg.GeometryMesh.make_box(
        .{ .x = 0.25, .y = -0.01, .z = 0.25 },
        .{ .x = 0.75, .y = 0.01, .z = 0.75 },
        .{ .x = 1, .y = 0.5, .z = 1, .w = 0.5 },
        .{ .x = 1, .y = 0, .z = 1, .w = 0.6 },
    ) },
    .{ .TranslationXY, alg.GeometryMesh.make_box(
        .{ .x = 0.25, .y = 0.25, .z = -0.01 },
        .{ .x = 0.75, .y = 0.75, .z = 0.01 },
        .{ .x = 1, .y = 1, .z = 0.5, .w = 0.5 },
        .{ .x = 1, .y = 1, .z = 0, .w = 0.6 },
    ) },
    .{ .TranslationView, alg.GeometryMesh.make_box(
        .{ .x = -0.05, .y = -0.05, .z = -0.05 },
        .{ .x = 0.05, .y = 0.05, .z = 0.05 },
        .{ .x = 0.9, .y = 0.9, .z = 0.9, .w = 0.25 },
        .{ .x = 1, .y = 1, .z = 1, .w = 0.35 },
    ) },
};

pub fn mesh(
    modelMatrix: alg.Float4x4,
    user: *anyopaque,
    add_triangle: alg.AddTriangleFunc,
    active_component: ?GizmoComponentType,
) void {
    for (gizmo_components) |entry| {
        const component, const geometry = entry;
        const color = if (component == active_component) geometry.base_color else geometry.highlight_color;
        geometry.add_triangles(user, add_triangle, modelMatrix, color);
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
    //   DragState drag_state = {
    var click_offset = ray.point(ray_state.t);
    //       .original_position = ray_state.transform.position,
    //   };
    if (ray_state.local_toggle) {
        click_offset = ray_state.gizmo_transform.transform_vector(click_offset);
    }
    //   return drag_state;
    return click_offset;
}

fn plane_drag(plane_normal: alg.Float3, frame: state.FrameState, ray_state: state.RayState, _: alg.Transform) ?alg.Float3 {
    const plane =
        alg.Plane.from_normal_and_position(plane_normal, ray_state.transform.position);
    if (frame.ray.intersect_plane(plane)) |t| {
        return frame.ray.point(t).sub(get_click_offset(ray_state));
    } else {
        return null;
    }
}

fn axis_drag(axis: alg.Float3, active_state: state.FrameState, ray_state: state.RayState, src: alg.Transform) ?alg.Float3 {
    // First apply a plane translation dragger with a plane that contains the
    // desired axis and is oriented to face the camera
    const plane_tangent =
        alg.Float3.cross(axis, src.position.sub(active_state.ray.origin));
    const plane_normal = alg.Float3.cross(axis, plane_tangent);
    const plane =
        alg.Plane.from_normal_and_position(plane_normal, ray_state.transform.position);
    if (active_state.ray.intersect_plane(plane)) |t| {
        const dst = active_state.ray.point(t);

        // Constrain object motion to be along the desired axis
        return ray_state.transform.position.add(axis.scale(alg.Float3.dot(dst.sub(ray_state.transform.position), axis))).sub(get_click_offset(ray_state));
    } else {
        return null;
    }
}

pub fn drag(
    active_component: GizmoComponentType,
    frame: state.FrameState,
    ray_state: state.RayState,
    src: alg.Transform,
) alg.Transform {
    const drag_position = switch (active_component) {
        .TranslationX => blk: {
            const axis = if (ray_state.local_toggle) src.orientation.xdir() else alg.Float3.x_axis;
            break :blk axis_drag(axis, frame, ray_state, src);
        },

        .TranslationY => blk: {
            const axis = if (ray_state.local_toggle) src.orientation.ydir() else alg.Float3.y_axis;
            break :blk axis_drag(axis, frame, ray_state, src);
        },

        .TranslationZ => blk: {
            const axis = if (ray_state.local_toggle) src.orientation.zdir() else alg.Float3.z_axis;
            break :blk axis_drag(axis, frame, ray_state, src);
        },

        .TranslationXY => blk: {
            const normal =
                if (ray_state.local_toggle) src.orientation.zdir() else alg.Float3.z_axis;
            break :blk plane_drag(normal, frame, ray_state, src);
        },

        .TranslationYZ => blk: {
            const normal =
                if (ray_state.local_toggle) src.orientation.xdir() else alg.Float3.x_axis;
            break :blk plane_drag(normal, frame, ray_state, src);
        },

        .TranslationZX => blk: {
            const normal =
                if (ray_state.local_toggle) src.orientation.ydir() else alg.Float3.y_axis;
            break :blk plane_drag(normal, frame, ray_state, src);
        },

        .TranslationView => blk: {
            const normal = frame.cam_orientation.zdir().negate();
            break :blk plane_drag(normal, frame, ray_state, src);
        },
    };

    if (drag_position) |position| {
        var dst = src;
        dst.position = position;
        return dst;
    } else {
        return src;
    }
}
