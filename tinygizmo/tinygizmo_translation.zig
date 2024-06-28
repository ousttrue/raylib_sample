const alg = @import("tinygizmo_alg.zig");
const state = @import("tinygizmo_state.zig");

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
    _ = p; // autofix
    _ = local_toggle; // autofix
    _ = frame; // autofix}
    return null;
}

pub fn drag(
    active_component: GizmoComponentType,
    frame: state.FrameState,
    ray: state.RayState,
    src: alg.Transform,
) alg.Transform {
    _ = src; // autofix
    _ = ray; // autofix
    _ = ray; // autofix
    _ = frame; // autofix
    _ = active_component; // autofix}
    return .{};
}
