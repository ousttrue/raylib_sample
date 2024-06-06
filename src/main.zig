const std = @import("std");
const raylib = @cImport({
    @cInclude("raylib.h");
    @cInclude("rcamera.h");
    @cInclude("raymath.h");
});

const OribtCamera = struct {
    yawDegree: i32 = 0,
    pitchDegree: i32 = -40,
    distance: f32 = 10,
    shiftX: f32 = 0,
    shiftY: f32 = 0,

    // prevMousePos: raylib.Vector2 = .{ .x = 0, .y = 0 },
    const Self = @This();
    fn MouseUpdateCamera(
        self: *Self,
        camera: *raylib.Camera3D,
        width: f32,
        height: f32,
    ) void {
        _ = width; // autofix
        const delta = raylib.GetMouseDelta();

        // mouse wheel
        const wheel = raylib.GetMouseWheelMoveV();
        if (wheel.y > 0) {
            self.distance *= 0.9;
        } else if (wheel.y < 0) {
            self.distance *= 1.1;
        }

        // camera shift
        if (raylib.IsMouseButtonDown(raylib.MOUSE_BUTTON_MIDDLE)) {
            const d = raylib.Vector3Distance(camera.target, camera.position);
            const speed = d * std.math.tan(camera.fovy * 0.5) * 2.0 / height;
            self.shiftX += delta.x * speed;
            self.shiftY += delta.y * speed;
        }

        // yaw pitch
        if (raylib.IsMouseButtonDown(raylib.MOUSE_BUTTON_RIGHT)) {
            self.yawDegree -= @intFromFloat(delta.x);
            self.pitchDegree -= @intFromFloat(delta.y);
            if (self.pitchDegree > 89) {
                self.pitchDegree = 89;
            } else if (self.pitchDegree < -89) {
                self.pitchDegree = -89;
            }
        }

        const pitch = raylib.MatrixRotateX(
            @as(f32, @floatFromInt(self.pitchDegree)) * raylib.DEG2RAD,
        );
        const yaw = raylib.MatrixRotateY(
            @as(f32, @floatFromInt(-self.yawDegree)) * raylib.DEG2RAD,
        );
        const translation = raylib.MatrixTranslate(
            -self.shiftX,
            -self.shiftY,
            --self.distance,
        );

        const view = raylib.MatrixMultiply(
            raylib.MatrixMultiply(yaw, pitch),
            translation,
        );

        const viewInverse = raylib.MatrixInvert(view);

        camera.position = .{
            .x = viewInverse.m12,
            .y = viewInverse.m13,
            .z = viewInverse.m14,
        };
        const forward = raylib.Vector3{
            .x = viewInverse.m8,
            .y = viewInverse.m9,
            .z = viewInverse.m10,
        };
        camera.target = raylib.Vector3Add(
            camera.position,
            raylib.Vector3Scale(forward, self.distance),
        );
    }
};

pub fn main() !void {
    raylib.InitWindow(1024, 786, "experiment");
    defer raylib.CloseWindow();

    var camera = raylib.Camera3D{
        .position = .{ .x = 0.0, .y = 10.0, .z = 10.0 }, // Camera position
        .target = .{ .x = 0.0, .y = 0.0, .z = 0.0 }, // Camera looking at point
        .up = .{ .x = 0.0, .y = 1.0, .z = 0.0 }, // Camera up vector (rotation towards target)
        .fovy = 45.0, // Camera field-of-view Y
        .projection = raylib.CAMERA_PERSPECTIVE, // Camera mode type
    };

    var orbit = OribtCamera{};

    const cubePosition = raylib.Vector3{ .x = 0.0, .y = 0.0, .z = 0.0 };

    while (!raylib.WindowShouldClose()) {
        orbit.MouseUpdateCamera(
            &camera,
            @floatFromInt(raylib.GetScreenWidth()),
            @floatFromInt(raylib.GetScreenHeight()),
        );

        raylib.BeginDrawing();
        raylib.ClearBackground(raylib.RAYWHITE);
        raylib.DrawText(
            raylib.TextFormat(
                "yaw: %d, pitch: %d, shift: %.3f, %.3f, distance: %.3f",
                orbit.yawDegree,
                orbit.pitchDegree,
                orbit.shiftX,
                orbit.shiftY,
                orbit.distance,
            ),
            190,
            200,
            20,
            raylib.LIGHTGRAY,
        );

        {
            raylib.BeginMode3D(camera);

            raylib.DrawCube(cubePosition, 2.0, 2.0, 2.0, raylib.RED);
            raylib.DrawCubeWires(cubePosition, 2.0, 2.0, 2.0, raylib.MAROON);

            raylib.DrawGrid(10, 1.0);

            raylib.EndMode3D();
        }

        raylib.EndDrawing();
    }
}
