const std = @import("std");

const zamath = @import("zamath.zig");
const layout = @import("layout.zig");
const c = layout.c;

fn to_raylib(v: zamath.Vec3) c.Vector3 {
    return .{
        .x = v.x,
        .y = v.y,
        .z = v.z,
    };
}

const Scene = struct {
    cubePosition: layout.c.Vector3 = .{ .x = 0.0, .y = 0.0, .z = 0.0 },

    pub fn draw(
        self: @This(),
        rendertargets: []layout.RenderTarget,
        current: *const layout.RenderTarget,
    ) void {
        c.rlDrawRenderBatchActive(); // Update and draw internal render batch

        {
            c.rlMatrixMode(c.RL_PROJECTION);
            c.rlPushMatrix();
            c.rlLoadIdentity();
            c.rlMultMatrixf(&current.projection.matrix.m00);
        }
        {
            c.rlMatrixMode(c.RL_MODELVIEW);
            c.rlLoadIdentity();
            c.rlMultMatrixf(&current.orbit.view_matrix.m00);
        }

        c.rlEnableDepthTest(); // Enable DEPTH_TEST for 3D

        {
            for (rendertargets) |rendertarget| {
                if (&rendertarget != current) {
                    const b = rendertarget.orbit.position();
                    const e = b.add(rendertarget.orbit.forward().scale(rendertarget.projection.z_far));
                    c.DrawLine3D(.{
                        .x = b.x,
                        .y = b.y,
                        .z = b.z,
                    }, .{
                        .x = e.x,
                        .y = e.y,
                        .z = e.z,
                    }, c.DARKBLUE);
                }
            }

            c.DrawCube(self.cubePosition, 2.0, 2.0, 2.0, c.RED);
            c.DrawCubeWires(self.cubePosition, 2.0, 2.0, 2.0, c.MAROON);

            c.DrawGrid(10, 1.0);
        }

        c.EndMode3D();
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

    while (!c.WindowShouldClose()) {
        const w = c.GetScreenWidth();
        const h = c.GetScreenHeight();
        const cursor = c.GetMousePosition();
        root.update(w, h, @intFromFloat(cursor.x), @intFromFloat(cursor.y));

        {
            c.BeginDrawing();
            // c.ClearBackground(c.RAYWHITE);
            for (root.rendertargets.items) |*rendertarget| {
                const texture = rendertarget.begin(root.active == rendertarget);
                scene.draw(root.rendertargets.items, rendertarget);
                rendertarget.end(texture);
            }
            c.EndDrawing();
        }
    }
}
