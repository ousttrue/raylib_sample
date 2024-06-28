const alg = @import("tinygizmo_alg.zig");
const state = @import("tinygizmo_state.zig");

pub const GizmoComponentType = enum {
    ScalingX,
    ScalingY,
    ScalingZ,
};

pub fn mesh(
    modelMatrix: alg.Float4x4,
    user: *anyopaque,
    add_triangle: alg.AddTriangleFunc,
    active_component: ?GizmoComponentType,
) void {
    _ = user; // autofix
    _ = active_component; // autofix
    _ = active_component; // autofix
    _ = add_triangle; // autofix
    _ = modelMatrix; // autofix
    //
}

pub fn intersect(
    frame: state.FrameState,
    local_toggle: bool,
    p: alg.Transform,
    uniform: bool,
) ?struct { state.RayState, GizmoComponentType } {
    _ = uniform; // autofix
    _ = p; // autofix
    _ = local_toggle; // autofix
    _ = frame; // autofix
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
    _ = frame; // autofix
    _ = active_component; // autofix
    return .{};
}
