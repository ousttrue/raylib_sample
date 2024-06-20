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

const Scene = struct {
    cubePosition: layout.c.Vector3 = .{ .x = 0.0, .y = 0.0, .z = 0.0 },

    pub fn draw(
        self: @This(),
        rendertargets: []layout.RenderTarget,
        current: *const layout.RenderTarget,
    ) void {
        // frustom
        for (rendertargets) |*rendertarget| {
            if (rendertarget != current) {
                draw_frustum(zamath.Frustum.make(
                    rendertarget.orbit.transform_matrix,
                    rendertarget.projection.fovy,
                    rendertarget.viewport.width / rendertarget.viewport.height,
                    rendertarget.projection.z_near,
                    rendertarget.projection.z_far,
                ));

                // mouse ray
                const start, const end = rendertarget.mouse_near_far();
                c.DrawLine3D(tor(start), tor(end), c.YELLOW);
            }
        }

        // local scene
        c.DrawCube(self.cubePosition, 2.0, 2.0, 2.0, c.RED);
        c.DrawCubeWires(self.cubePosition, 2.0, 2.0, 2.0, c.MAROON);

        c.DrawGrid(10, 1.0);
    }
};

pub fn main() !void {
    const allocator = std.heap.page_allocator;

    c.SetConfigFlags(c.FLAG_WINDOW_RESIZABLE);
    c.InitWindow(1600, 1200, "experiment");
    defer c.CloseWindow();

    var root = try layout.Layout.make(allocator);
    defer root.deinit();

    const scene = Scene{};

    // Custom timming variables
    var previousTime: f64 = c.GetTime(); // Previous time measure
    var currentTime: f64 = 0.0; // Current time measure
    var updateDrawTime: f64 = 0.0; // Update + Draw time
    var waitTime: f64 = 0.0; // Wait time (if target fps required)
    var deltaTime: f64 = 0.0; // Frame time (Update + Draw + Wait time)
    const targetFPS = 60.0;

    while (!c.WindowShouldClose()) {
        if (targetFPS < 0) targetFPS = 0;

        const w = c.GetScreenWidth();
        const h = c.GetScreenHeight();
        const cursor = c.GetMousePosition();
        const cursor_delta = c.GetMouseDelta();
        const wheel = c.GetMouseWheelMoveV();

        root.update(w, h, wheel.y, cursor, cursor_delta);

        {
            c.BeginDrawing();
            // c.ClearBackground(c.RAYWHITE);
            for (root.rendertargets.items) |*rendertarget| {
                const texture = rendertarget.begin();

                c.DrawText(
                    c.TextFormat(
                        "x: %.0f, y: %.0f, width: %.0f, height: %.0f",
                        rendertarget.viewport.x,
                        rendertarget.viewport.y,
                        rendertarget.viewport.width,
                        rendertarget.viewport.height,
                    ),
                    0,
                    0,
                    20,
                    c.LIGHTGRAY,
                );

                {
                    rendertarget.begin_camera3D();
                    scene.draw(root.rendertargets.items, rendertarget);
                    rendertarget.end_camera3D();
                }

                rendertarget.end(texture);
            }

            c.DrawText(c.TextFormat("CURRENT FPS: %.0f", (1.0 / deltaTime)), c.GetScreenWidth() - 220, 40, 20, c.GREEN);
            c.EndDrawing();
        }

        // c.SwapScreenBuffer(); // Flip the back buffer to screen (front buffer)
        currentTime = c.GetTime();
        updateDrawTime = currentTime - previousTime;

        if (targetFPS > 0) // We want a fixed frame rate
        {
            waitTime = (1.0 / targetFPS) - updateDrawTime;
            if (waitTime > 0.0) {
                c.WaitTime(waitTime);
                currentTime = c.GetTime();
                deltaTime = currentTime - previousTime;
            }
        } else {
            deltaTime = updateDrawTime; // Framerate could be variable
        }

        previousTime = currentTime;
    }
}
