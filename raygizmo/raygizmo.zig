const std = @import("std");
pub const c = @cImport({
    @cInclude("raylib.h");
    @cInclude("rlgl.h");
    @cInclude("raymath.h");
});

pub const RGizmoState = enum {
    RGIZMO_STATE_COLD,

    RGIZMO_STATE_HOT,

    RGIZMO_STATE_HOT_ROT,
    RGIZMO_STATE_HOT_AXIS,
    RGIZMO_STATE_HOT_PLANE,

    RGIZMO_STATE_ACTIVE,

    RGIZMO_STATE_ACTIVE_ROT,
    RGIZMO_STATE_ACTIVE_AXIS,
    RGIZMO_STATE_ACTIVE_PLANE,
};

pub const Update = struct {
    translation: c.Vector3 = .{ .x = 0, .y = 0, .z = 0 },
    axis: c.Vector3 = .{ .x = 0, .y = 0, .z = 0 },
    angle: f32 = 0,
};

pub const View = struct {
    size: f32,
    handle_draw_thickness: f32,
    active_axis_draw_thickness: f32,
    axis_handle_length: f32,
    axis_handle_tip_length: f32,
    axis_handle_tip_radius: f32,
    plane_handle_offset: f32,
    plane_handle_size: f32,
};

pub const RGizmo = struct {
    update: Update = .{},
    view: View,
    state: RGizmoState = .RGIZMO_STATE_COLD,
};

const SHADER_VERT =
    \\#version 330
    \\in vec3 vertexPosition;
    \\in vec4 vertexColor;
    \\out vec4 fragColor;
    \\out vec3 fragPosition;
    \\uniform mat4 mvp;
    \\void main()
    \\{
    \\    fragColor = vertexColor;
    \\    fragPosition = vertexPosition;
    \\    gl_Position = mvp * vec4(vertexPosition, 1.0);
    \\}
;

const SHADER_FRAG =
    \\#version 330
    \\in vec4 fragColor;
    \\in vec3 fragPosition;
    \\uniform vec3 cameraPosition;
    \\uniform vec3 gizmoPosition;
    \\out vec4 finalColor;
    \\void main()
    \\{
    \\    vec3 r = normalize(fragPosition - gizmoPosition);
    \\    vec3 c = normalize(fragPosition - cameraPosition);
    \\    if (dot(r, c) > 0.1) discard;
    \\    finalColor = fragColor;
    \\}
;

const PICKING_FBO_WIDTH = 512;
const PICKING_FBO_HEIGHT = 512;

const X_AXIS = c.Vector3{ .x = 1.0, .y = 0.0, .z = 0.0 };
const Y_AXIS = c.Vector3{ .x = 0.0, .y = 1.0, .z = 0.0 };
const Z_AXIS = c.Vector3{ .x = 0.0, .y = 0.0, .z = 1.0 };

var IS_LOADED = false;
var SHADER: c.Shader = .{};
var SHADER_CAMERA_POSITION_LOC: c_int = 0;
var SHADER_GIZMO_POSITION_LOC: c_int = 0;

var PICKING_FBO: c_uint = 0;
var PICKING_TEXTURE: c_uint = 0;

const HandleId = enum {
    HANDLE_X,
    ROT_HANDLE_X,
    AXIS_HANDLE_X,
    PLANE_HANDLE_X,
    HANDLE_Y,
    ROT_HANDLE_Y,
    AXIS_HANDLE_Y,
    PLANE_HANDLE_Y,
    HANDLE_Z,
    ROT_HANDLE_Z,
    AXIS_HANDLE_Z,
    PLANE_HANDLE_Z,
};

const Handle = struct {
    position: c.Vector3,
    axis: c.Vector3,
    color: c.Color,
    distToCamera: f32,
};

const XYZColors = struct {
    x: c.Color,
    y: c.Color,
    z: c.Color,
};

const HandleColors = struct {
    rot: XYZColors,
    axis: XYZColors,
    plane: XYZColors,
};

const Handles = struct {
    arr: [3]Handle,
};

fn sort_handles(h0: Handle, h1: Handle, h2: Handle) Handles {
    if (h0.distToCamera < h1.distToCamera) {
        if (h1.distToCamera < h2.distToCamera) {
            return .{ .arr = .{ h0, h1, h2 } };
        } else if (h0.distToCamera < h2.distToCamera) {
            return .{ .arr = .{ h0, h2, h1 } };
        } else {
            return .{ .arr = .{ h2, h0, h1 } };
        }
    } else {
        if (h0.distToCamera < h2.distToCamera) {
            return .{ .arr = .{ h1, h0, h2 } };
        } else if (h1.distToCamera < h2.distToCamera) {
            return .{ .arr = .{ h1, h2, h0 } };
        } else {
            return .{ .arr = .{ h2, h1, h0 } };
        }
    }
}

fn get_xyz_colors(current_axis: c.Vector3, is_hot: bool) XYZColors {
    const x = if (is_hot and current_axis.x == 1.0) c.WHITE else c.RED;
    const y = if (is_hot and current_axis.y == 1.0) c.WHITE else c.GREEN;
    const z = if (is_hot and current_axis.z == 1.0) c.WHITE else c.BLUE;
    return .{ .x = x, .y = y, .z = z };
}

fn draw_gizmo(
    gizmo: *RGizmo,
    camera: c.Camera3D,
    position: c.Vector3,
    colors: HandleColors,
) void {
    const radius = gizmo.view.size * c.Vector3Distance(camera.position, position);

    c.BeginMode3D(camera);
    c.rlSetLineWidth(gizmo.view.handle_draw_thickness);
    c.rlDisableDepthTest();

    // ---------------------------------------------------------------
    // Draw plane handles
    {
        const offset = radius * gizmo.view.plane_handle_offset;
        const size = radius * gizmo.view.plane_handle_size;

        const px = c.Vector3Add(position, .{ .x = 0.0, .y = offset, .z = offset });
        const py = c.Vector3Add(position, .{ .x = offset, .y = 0.0, .z = offset });
        const pz = c.Vector3Add(position, .{ .x = offset, .y = offset, .z = 0.0 });

        const hx = Handle{
            .position = px,
            .axis = Z_AXIS,
            .color = colors.plane.x,
            .distToCamera = c.Vector3DistanceSqr(px, camera.position),
        };
        const hy = Handle{
            .position = py,
            .axis = Y_AXIS,
            .color = colors.plane.y,
            .distToCamera = c.Vector3DistanceSqr(py, camera.position),
        };
        const hz = Handle{
            .position = pz,
            .axis = X_AXIS,
            .color = colors.plane.z,
            .distToCamera = c.Vector3DistanceSqr(pz, camera.position),
        };
        const handles = sort_handles(hx, hy, hz);

        c.rlDisableBackfaceCulling();
        for (0..3) |i| {
            const h = &handles.arr[i];
            c.rlPushMatrix();
            c.rlTranslatef(h.position.x, h.position.y, h.position.z);
            c.rlRotatef(90.0, h.axis.x, h.axis.y, h.axis.z);
            c.DrawPlane(c.Vector3Zero(), c.Vector2Scale(c.Vector2One(), size), h.color);
            c.rlPopMatrix();
        }
    }

    // ---------------------------------------------------------------
    // Draw rotation handles
    {
        c.BeginShaderMode(SHADER);
        c.SetShaderValue(SHADER, SHADER_CAMERA_POSITION_LOC, &camera.position, c.SHADER_UNIFORM_VEC3);
        c.SetShaderValue(SHADER, SHADER_GIZMO_POSITION_LOC, &position, c.SHADER_UNIFORM_VEC3);
        c.DrawCircle3D(position, radius, Y_AXIS, 90.0, colors.rot.x);
        c.DrawCircle3D(position, radius, X_AXIS, 90.0, colors.rot.y);
        c.DrawCircle3D(position, radius, X_AXIS, 0.0, colors.rot.z);
        c.EndShaderMode();
    }

    // ---------------------------------------------------------------
    // Draw axis handles
    {
        const length = radius * gizmo.view.axis_handle_length;
        const tip_length = radius * gizmo.view.axis_handle_tip_length;
        const tip_radius = radius * gizmo.view.axis_handle_tip_radius;

        const px = c.Vector3Add(position, c.Vector3Scale(X_AXIS, length));
        const py = c.Vector3Add(position, c.Vector3Scale(Y_AXIS, length));
        const pz = c.Vector3Add(position, c.Vector3Scale(Z_AXIS, length));

        const hx = Handle{
            .position = px,
            .axis = X_AXIS,
            .color = colors.axis.x,
            .distToCamera = c.Vector3DistanceSqr(px, camera.position),
        };
        const hy = Handle{
            .position = py,
            .axis = Y_AXIS,
            .color = colors.axis.y,
            .distToCamera = c.Vector3DistanceSqr(py, camera.position),
        };
        const hz = Handle{
            .position = pz,
            .axis = Z_AXIS,
            .color = colors.axis.z,
            .distToCamera = c.Vector3DistanceSqr(pz, camera.position),
        };
        const handles = sort_handles(hx, hy, hz);

        for (0..3) |i| {
            const h = &handles.arr[i];
            const tip_end = c.Vector3Add(h.position, c.Vector3Scale(h.axis, tip_length));
            c.DrawLine3D(position, h.position, h.color);
            c.DrawCylinderEx(h.position, tip_end, tip_radius, 0.0, 16, h.color);
        }
    }
    c.EndMode3D();

    // ---------------------------------------------------------------
    // Draw long white line which represents current active axis
    if (gizmo.state == .RGIZMO_STATE_ACTIVE_ROT or gizmo.state == .RGIZMO_STATE_ACTIVE_AXIS) {
        c.BeginMode3D(camera);
        c.rlSetLineWidth(gizmo.view.active_axis_draw_thickness);
        const halfAxisLine = c.Vector3Scale(gizmo.update.axis, 1000.0);
        c.DrawLine3D(c.Vector3Subtract(position, halfAxisLine), c.Vector3Add(position, halfAxisLine), c.WHITE);
        c.EndMode3D();
    }

    // ---------------------------------------------------------------
    // Draw white line from the gizmo's center to the mouse cursor when rotating
    if (gizmo.state == .RGIZMO_STATE_ACTIVE_ROT) {
        c.rlSetLineWidth(gizmo.view.active_axis_draw_thickness);
        c.DrawLineV(c.GetWorldToScreen(position, camera), c.GetMousePosition(), c.WHITE);
    }
}

fn rgizmo_load() void {
    if (IS_LOADED) {
        c.TraceLog(c.LOG_WARNING, "RAYGIZMO: Gizmo is already loaded, skip");
        return;
    }

    // -------------------------------------------------------------------
    // Load shader
    SHADER = c.LoadShaderFromMemory(SHADER_VERT, SHADER_FRAG);
    SHADER_CAMERA_POSITION_LOC = c.GetShaderLocation(SHADER, "cameraPosition");
    SHADER_GIZMO_POSITION_LOC = c.GetShaderLocation(SHADER, "gizmoPosition");

    // -------------------------------------------------------------------
    // Load picking fbo
    PICKING_FBO = c.rlLoadFramebuffer(PICKING_FBO_WIDTH, PICKING_FBO_HEIGHT);
    if (PICKING_FBO == 0) {
        c.TraceLog(c.LOG_ERROR, "RAYGIZMO: Failed to create picking fbo");
        std.c.exit(1);
    }
    c.rlEnableFramebuffer(PICKING_FBO);

    PICKING_TEXTURE = c.rlLoadTexture(null, PICKING_FBO_WIDTH, PICKING_FBO_HEIGHT, c.RL_PIXELFORMAT_UNCOMPRESSED_R8G8B8A8, 1);
    c.rlActiveDrawBuffers(1);
    c.rlFramebufferAttach(PICKING_FBO, PICKING_TEXTURE, c.RL_ATTACHMENT_COLOR_CHANNEL0, c.RL_ATTACHMENT_TEXTURE2D, 0);
    if (!c.rlFramebufferComplete(PICKING_FBO)) {
        c.TraceLog(c.LOG_ERROR, "RAYGIZMO: Picking fbo is not complete");
        std.c.exit(1);
    }

    IS_LOADED = true;
    c.TraceLog(c.LOG_INFO, "RAYGIZMO: Gizmo loaded");
}

pub fn rgizmo_unload() void {
    if (!IS_LOADED) {
        c.TraceLog(c.LOG_WARNING, "RAYGIZMO: Gizmo is already unloaded, skip");
        return;
    }

    c.UnloadShader(SHADER);
    c.rlUnloadFramebuffer(PICKING_FBO);
    c.rlUnloadTexture(PICKING_TEXTURE);

    IS_LOADED = false;
    c.TraceLog(c.LOG_INFO, "RAYGIZMO: Gizmo unloaded");
}

pub fn rgizmo_create() RGizmo {
    if (!IS_LOADED) rgizmo_load();

    return .{
        .view = .{
            .size = 0.12,
            .handle_draw_thickness = 5.0,
            .active_axis_draw_thickness = 2.0,
            .axis_handle_length = 1.2,
            .axis_handle_tip_length = 0.3,
            .axis_handle_tip_radius = 0.1,
            .plane_handle_offset = 0.4,
            .plane_handle_size = 0.2,
        },
    };
}

pub fn rgizmo_update(gizmo: *RGizmo, camera: c.Camera3D, position: c.Vector3) void {
    if (!IS_LOADED) {
        c.TraceLog(c.LOG_ERROR, "RAYGIZMO: Gizmo is not loaded");
        std.c.exit(1);
    }

    // -------------------------------------------------------------------
    // Draw gizmo into the picking fbo for the mouse pixel-picking
    c.rlEnableFramebuffer(PICKING_FBO);
    c.rlViewport(0, 0, PICKING_FBO_WIDTH, PICKING_FBO_HEIGHT);
    c.rlClearColor(0, 0, 0, 0);
    c.rlClearScreenBuffers();
    c.rlDisableColorBlend();

    const colors = HandleColors{
        .rot = .{
            .x = .{ .r = @intFromEnum(HandleId.ROT_HANDLE_X), .g = 0, .b = 0, .a = 0 },
            .y = .{ .r = @intFromEnum(HandleId.ROT_HANDLE_Y), .g = 0, .b = 0, .a = 0 },
            .z = .{ .r = @intFromEnum(HandleId.ROT_HANDLE_Z), .g = 0, .b = 0, .a = 0 },
        },
        .axis = .{
            .x = .{ .r = @intFromEnum(HandleId.AXIS_HANDLE_X), .g = 0, .b = 0, .a = 0 },
            .y = .{ .r = @intFromEnum(HandleId.AXIS_HANDLE_Y), .g = 0, .b = 0, .a = 0 },
            .z = .{ .r = @intFromEnum(HandleId.AXIS_HANDLE_Z), .g = 0, .b = 0, .a = 0 },
        },
        .plane = .{
            .x = .{ .r = @intFromEnum(HandleId.PLANE_HANDLE_X), .g = 0, .b = 0, .a = 0 },
            .y = .{ .r = @intFromEnum(HandleId.PLANE_HANDLE_Y), .g = 0, .b = 0, .a = 0 },
            .z = .{ .r = @intFromEnum(HandleId.PLANE_HANDLE_Z), .g = 0, .b = 0, .a = 0 },
        },
    };

    draw_gizmo(gizmo, camera, position, colors);

    c.rlDisableFramebuffer();
    c.rlEnableColorBlend();
    c.rlViewport(0, 0, c.GetScreenWidth(), c.GetScreenHeight());

    // -------------------------------------------------------------------
    // Pick the pixel under the mouse cursor
    const mouse_position = c.GetMousePosition();
    const pixels = c.rlReadTexturePixels(
        PICKING_TEXTURE,
        PICKING_FBO_WIDTH,
        PICKING_FBO_HEIGHT,
        c.RL_PIXELFORMAT_UNCOMPRESSED_R8G8B8A8,
    );

    const x_fract = c.Clamp(mouse_position.x / @as(f32, @floatFromInt(c.GetScreenWidth())), 0.0, 1.0);
    const y_fract = c.Clamp(1.0 - (mouse_position.y / @as(f32, @floatFromInt(c.GetScreenHeight()))), 0.0, 1.0);
    const x = PICKING_FBO_WIDTH * x_fract;
    const y = PICKING_FBO_HEIGHT * y_fract;
    const idx: usize = @intFromFloat(4 * (y * PICKING_FBO_WIDTH + x));
    var picked_id: u8 = 0;
    if (pixels) |p| {
        picked_id = @as([*c]u8, @ptrCast(p))[idx];
    }

    std.c.free(pixels);

    // -------------------------------------------------------------------
    // Update gizmo
    gizmo.update.angle = 0.0;
    gizmo.update.translation = c.Vector3Zero();

    const is_lmb_down = c.IsMouseButtonDown(0);
    if (!is_lmb_down) gizmo.state = .RGIZMO_STATE_COLD;

    if (@intFromEnum(gizmo.state) < @intFromEnum(RGizmoState.RGIZMO_STATE_ACTIVE)) {
        if (picked_id < @intFromEnum(HandleId.HANDLE_Y)) {
            gizmo.update.axis = X_AXIS;
        } else if (picked_id < @intFromEnum(HandleId.HANDLE_Z)) {
            gizmo.update.axis = Y_AXIS;
        } else {
            gizmo.update.axis = Z_AXIS;
        }

        if (picked_id % 4 == 1) {
            gizmo.state = if (is_lmb_down) .RGIZMO_STATE_ACTIVE_ROT else .RGIZMO_STATE_HOT_ROT;
        } else if (picked_id % 4 == 2) {
            gizmo.state = if (is_lmb_down) .RGIZMO_STATE_ACTIVE_AXIS else .RGIZMO_STATE_HOT_AXIS;
        } else if (picked_id % 4 == 3) {
            gizmo.state = if (is_lmb_down) .RGIZMO_STATE_ACTIVE_PLANE else .RGIZMO_STATE_HOT_PLANE;
        }
    }

    const delta = c.GetMouseDelta();
    const is_mouse_moved = (@abs(delta.x) + @abs(delta.y)) > c.EPSILON;
    if (!is_mouse_moved) return;

    switch (gizmo.state) {
        .RGIZMO_STATE_ACTIVE_ROT => {
            const p1 = c.Vector2Subtract(c.GetMousePosition(), c.GetWorldToScreen(position, camera));
            const p0 = c.Vector2Subtract(p1, c.GetMouseDelta());

            // Get angle between two vectors:
            var angle: f32 = 0.0;
            const dot = c.Vector2DotProduct(c.Vector2Normalize(p1), c.Vector2Normalize(p0));
            if (1.0 - @abs(dot) > c.EPSILON) {
                angle = std.math.acos(dot);
                const z = p1.x * p0.y - p1.y * p0.x;

                if (@abs(z) < c.EPSILON) {
                    angle = 0.0;
                } else if (z <= 0) {
                    angle *= -1.0;
                }
            }

            // If we look at the gizmo from behind, we should flip the rotation
            if (c.Vector3DotProduct(gizmo.update.axis, position) > c.Vector3DotProduct(gizmo.update.axis, camera.position)) {
                angle *= -1;
            }

            gizmo.update.angle = angle;
        },

        .RGIZMO_STATE_ACTIVE_AXIS => {
            const p = c.Vector2Add(c.GetWorldToScreen(position, camera), c.GetMouseDelta());
            const r = c.GetMouseRay(p, camera);

            // Get two lines nearest point
            const line0point0 = camera.position;
            const line0point1 = c.Vector3Add(line0point0, r.direction);
            const line1point0 = position;
            const line1point1 = c.Vector3Add(line1point0, gizmo.update.axis);
            const vec0 = c.Vector3Subtract(line0point1, line0point0);
            const vec1 = c.Vector3Subtract(line1point1, line1point0);
            const plane_vec = c.Vector3Normalize(c.Vector3CrossProduct(vec0, vec1));
            const plane_normal = c.Vector3Normalize(c.Vector3CrossProduct(vec0, plane_vec));

            // Intersect line and plane
            const dot = c.Vector3DotProduct(plane_normal, vec1);
            if (@abs(dot) > c.EPSILON) {
                const w = c.Vector3Subtract(line1point0, line0point0);
                const k = -c.Vector3DotProduct(plane_normal, w) / dot;
                const isect = c.Vector3Add(line1point0, c.Vector3Scale(vec1, k));
                gizmo.update.translation = c.Vector3Subtract(isect, position);
            }
        },

        .RGIZMO_STATE_ACTIVE_PLANE => {
            const p = c.Vector2Add(c.GetWorldToScreen(position, camera), c.GetMouseDelta());
            const r = c.GetMouseRay(p, camera);

            // Collide ray and plane
            const denominator = r.direction.x * gizmo.update.axis.x + r.direction.y * gizmo.update.axis.y + r.direction.z * gizmo.update.axis.z;

            if (@abs(denominator) > c.EPSILON) {
                const t = ((position.x - r.position.x) * gizmo.update.axis.x + (position.y - r.position.y) * gizmo.update.axis.y + (position.z - r.position.z) * gizmo.update.axis.z) / denominator;

                if (t > 0) {
                    const _c = c.Vector3Add(r.position, c.Vector3Scale(r.direction, t));
                    gizmo.update.translation = c.Vector3Subtract(_c, position);
                }
            }
        },

        else => {},
    }
}

pub fn rgizmo_draw(gizmo: *RGizmo, camera: c.Camera3D, position: c.Vector3) void {
    if (!IS_LOADED) {
        c.TraceLog(c.LOG_ERROR, "RAYGIZMO: Gizmo is not loaded");
        std.c.exit(1);
    }

    const colors = HandleColors{
        .rot = get_xyz_colors(gizmo.update.axis, gizmo.state == .RGIZMO_STATE_HOT_ROT or gizmo.state == .RGIZMO_STATE_ACTIVE_ROT),
        .axis = get_xyz_colors(gizmo.update.axis, gizmo.state == .RGIZMO_STATE_HOT_AXIS or gizmo.state == .RGIZMO_STATE_ACTIVE_AXIS),
        .plane = get_xyz_colors(gizmo.update.axis, gizmo.state == .RGIZMO_STATE_HOT_PLANE or gizmo.state == .RGIZMO_STATE_ACTIVE_PLANE),
    };

    draw_gizmo(gizmo, camera, position, colors);
}

pub fn rgizmo_get_tranform(gizmo: RGizmo, position: c.Vector3) c.Matrix {
    const translation = c.MatrixTranslate(gizmo.update.translation.x, gizmo.update.translation.y, gizmo.update.translation.z);

    const rotation = c.MatrixMultiply(c.MatrixMultiply(c.MatrixTranslate(-position.x, -position.y, -position.z), c.MatrixRotate(gizmo.update.axis, gizmo.update.angle)), c.MatrixTranslate(position.x, position.y, position.z));

    const transform = c.MatrixMultiply(translation, rotation);

    return transform;
}
