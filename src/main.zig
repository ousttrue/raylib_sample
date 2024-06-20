const std = @import("std");

const zamath = @import("zamath.zig");
const layout = @import("layout.zig");
const c = layout.c;

fn tor(v: zamath.Vec3) c.Vector3 {
    return .{
        .x = v.x,
        .y = v.y,
        .z = v.z,
    };
}

fn draw_frustum(frustum: zamath.Frustum) void {
    c.DrawLine3D(tor(frustum.near_left_top), tor(frustum.near_left_bottom), c.DARKBLUE);
    c.DrawLine3D(tor(frustum.near_left_bottom), tor(frustum.near_right_bottom), c.DARKBLUE);
    c.DrawLine3D(tor(frustum.near_right_bottom), tor(frustum.near_right_top), c.DARKBLUE);
    c.DrawLine3D(tor(frustum.near_right_top), tor(frustum.near_left_top), c.DARKBLUE);

    c.DrawLine3D(tor(frustum.near_left_top), tor(frustum.far_left_top), c.DARKBLUE);
    c.DrawLine3D(tor(frustum.near_left_bottom), tor(frustum.far_left_bottom), c.DARKBLUE);
    c.DrawLine3D(tor(frustum.near_right_bottom), tor(frustum.far_right_bottom), c.DARKBLUE);
    c.DrawLine3D(tor(frustum.near_right_top), tor(frustum.far_right_top), c.DARKBLUE);

    c.DrawLine3D(tor(frustum.far_left_top), tor(frustum.far_left_bottom), c.DARKBLUE);
    c.DrawLine3D(tor(frustum.far_left_bottom), tor(frustum.far_right_bottom), c.DARKBLUE);
    c.DrawLine3D(tor(frustum.far_right_bottom), tor(frustum.far_right_top), c.DARKBLUE);
    c.DrawLine3D(tor(frustum.far_right_top), tor(frustum.far_left_top), c.DARKBLUE);
}

pub fn begin_camera3D(projection: *f32, view: *f32) void {
    c.rlDrawRenderBatchActive(); // Update and draw internal render batch

    {
        c.rlMatrixMode(c.RL_PROJECTION);
        c.rlPushMatrix();
        c.rlLoadIdentity();
        c.rlMultMatrixf(projection);
    }
    {
        c.rlMatrixMode(c.RL_MODELVIEW);
        c.rlLoadIdentity();
        c.rlMultMatrixf(view);
    }

    c.rlEnableDepthTest(); // Enable DEPTH_TEST for 3D
}

pub fn end_camera3D() void {
    c.EndMode3D();
}

const Scene = struct {
    cubePosition: layout.c.Vector3 = .{ .x = 0.0, .y = 0.0, .z = 0.0 },

    pub fn draw(self: @This()) void {
        // frustom
        // for (rendertargets) |*rendertarget| {
        //     if (rendertarget != current) {
        //         draw_frustum(rendertarget.frustum());
        //
        //         // mouse ray
        //         const start, const end = rendertarget.mouse_near_far();
        //         c.DrawLine3D(tor(start), tor(end), c.YELLOW);
        //     }
        // }

        // local scene
        c.DrawCube(self.cubePosition, 2.0, 2.0, 2.0, c.RED);
        c.DrawCubeWires(self.cubePosition, 2.0, 2.0, 2.0, c.MAROON);

        c.DrawGrid(10, 1.0);
    }
};

pub fn main() !void {
    // const allocator = std.heap.page_allocator;

    c.SetConfigFlags(c.FLAG_VSYNC_HINT | c.FLAG_WINDOW_RESIZABLE);
    c.InitWindow(1600, 1200, "experiment");
    defer c.CloseWindow();

    c.SetTargetFPS(60);
    c.rlImGuiSetup(true);
    defer c.rlImGuiShutdown();

    // var root = try layout.Layout.make(allocator);
    // defer root.deinit();

    const scene = Scene{};

    // Custom timming variables
    var previousTime: f64 = c.GetTime(); // Previous time measure
    var currentTime: f64 = 0.0; // Current time measure
    var updateDrawTime: f64 = 0.0; // Update + Draw time
    // var waitTime: f64 = 0.0; // Wait time (if target fps required)
    var deltaTime: f64 = 0.0; // Frame time (Update + Draw + Wait time)
    const targetFPS = 60.0;

    var camera = zamath.Camera{};
    var camera_projection = zamath.CameraProjection{};
    var camera_orbit = zamath.CameraOrbit{};
    camera.view_matrix, camera.transform_matrix = camera_orbit.calc_matrix();

    while (!c.WindowShouldClose()) {
        if (targetFPS < 0) targetFPS = 0;

        const w = c.GetScreenWidth();
        const h = c.GetScreenHeight();
        const cursor = c.GetMousePosition();
        const cursor_delta = c.GetMouseDelta();
        const wheel = c.GetMouseWheelMoveV();

        // root.update(w, h, wheel.y, cursor, cursor_delta);
        if (camera.set_viewport_cursor(
            0,
            0,
            @floatFromInt(w),
            @floatFromInt(h),
            cursor.x,
            cursor.y,
        )) {
            camera.projection_matrix = camera_projection.calc_matrix(camera.viewport);
        }

        var active = false;
        if (wheel.y != 0) {
            camera_orbit.dolly(wheel.y);
            active = true;
        }
        if (c.IsMouseButtonDown(c.MOUSE_BUTTON_RIGHT)) {
            camera_orbit.yawpitch(cursor_delta.x, cursor_delta.y);
            active = true;
        }
        if (c.IsMouseButtonDown(c.MOUSE_BUTTON_MIDDLE)) {
            camera_orbit.shift(cursor_delta.x, cursor_delta.y, camera.viewport, camera_projection.fovy);
            active = true;
        }
        if (active) {
            camera.view_matrix, camera.transform_matrix = camera_orbit.calc_matrix();
        }

        {
            c.BeginDrawing();
            c.ClearBackground(c.RAYWHITE);
            // for (root.rendertargets.items) |*rendertarget| {
            //     const texture = rendertarget.begin();
            //
            //     c.DrawText(
            //         c.TextFormat(
            //             "x: %.0f, y: %.0f, width: %.0f, height: %.0f",
            //             rendertarget.camera.viewport.x,
            //             rendertarget.camera.viewport.y,
            //             rendertarget.camera.viewport.width,
            //             rendertarget.camera.viewport.height,
            //         ),
            //         0,
            //         0,
            //         20,
            //         c.LIGHTGRAY,
            //     );
            //
            //     rendertarget.end(texture);
            // }
            {
                begin_camera3D(&camera.projection_matrix.m00, &camera.view_matrix.m00);
                scene.draw();
                end_camera3D();
            }

            c.DrawText(c.TextFormat("CURRENT FPS: %.0f", (1.0 / deltaTime)), c.GetScreenWidth() - 220, 40, 20, c.GREEN);

            {
                c.rlImGuiBegin();

                // show ImGui Content
                var open = true;
                c.igShowDemoWindow(&open);

                // end ImGui Content
                c.rlImGuiEnd();
            }

            c.EndDrawing();
        }

        // c.SwapScreenBuffer(); // Flip the back buffer to screen (front buffer)
        currentTime = c.GetTime();
        updateDrawTime = currentTime - previousTime;

        // if (targetFPS > 0) // We want a fixed frame rate
        // {
        //     waitTime = (1.0 / targetFPS) - updateDrawTime;
        //     if (waitTime > 0.0) {
        //         c.WaitTime(waitTime);
        //         currentTime = c.GetTime();
        //         deltaTime = currentTime - previousTime;
        //     }
        // } else {
        deltaTime = updateDrawTime; // Framerate could be variable
        // }

        previousTime = currentTime;
    }
}
