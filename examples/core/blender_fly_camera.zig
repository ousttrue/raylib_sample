const raylib = @cImport({
    @cInclude("raylib.h");
    @cInclude("rcamera_blender.h");
    @cInclude("rlgl.h");
});

// Utility to draw a grid with colors like blender
fn DrawGridEx(slices: i32, spacing: f32) void {
    const halfSlices = @divTrunc(slices, 2);

    raylib.rlBegin(raylib.RL_LINES);
    var i = -halfSlices;
    while (i <= halfSlices) : (i += 1) {
        if (i == 0) {
            raylib.rlColor3f(0.5, 0.5, 0.5);
            raylib.rlColor3f(0.5, 0.5, 0.5);
            raylib.rlColor3f(0.5, 0.5, 0.5);
            raylib.rlColor3f(0.5, 0.5, 0.5);
        } else {
            raylib.rlColor3f(0.3, 0.3, 0.3);
            raylib.rlColor3f(0.3, 0.3, 0.3);
            raylib.rlColor3f(0.3, 0.3, 0.3);
            raylib.rlColor3f(0.3, 0.3, 0.3);
        }

        raylib.rlVertex3f(
            @as(f32, @floatFromInt(i)) * spacing,
            0.0,
            @as(f32, @floatFromInt(-halfSlices)) * spacing,
        );
        raylib.rlVertex3f(
            @as(f32, @floatFromInt(i)) * spacing,
            0.0,
            @as(f32, @floatFromInt(halfSlices)) * spacing,
        );

        raylib.rlVertex3f(
            @as(f32, @floatFromInt(-halfSlices)) * spacing,
            0.0,
            @as(f32, @floatFromInt(i)) * spacing,
        );
        raylib.rlVertex3f(
            @as(f32, @floatFromInt(halfSlices)) * spacing,
            0.0,
            @as(f32, @floatFromInt(i)) * spacing,
        );
    }
    raylib.rlEnd();
}

pub fn main() void {
    const screenWidth = 800;
    const screenHeight = 450;

    raylib.InitWindow(screenWidth, screenHeight, "Blender Camera");
    defer raylib.CloseWindow();

    // Initialize the camera
    var bcam = raylib.CreateBlenderCamera();

    raylib.SetTargetFPS(60);
    raylib.DisableCursor();

    // Define the cube position
    const cubePosition = raylib.Vector3{ .x = 0.0, .y = 0.0, .z = 0.0 };

    while (!raylib.WindowShouldClose()) {
        // Update the camera
        raylib.BlenderCameraUpdate(&bcam);

        raylib.BeginDrawing();
        raylib.ClearBackground(raylib.BLENDER_DARK_GREY);

        raylib.BeginMode3D(bcam.camera);

        raylib.DrawCube(cubePosition, 2.0, 2.0, 2.0, raylib.BLENDER_GREY);
        raylib.DrawCubeWires(cubePosition, 2.0, 2.0, 2.0, raylib.ORANGE);

        DrawGridEx(20, 1.0);

        raylib.EndMode3D();

        if (bcam.freeFly) {
            raylib.DrawText("Blender Camera Mode: FREE_FLY", 10, 10, 20, raylib.BLENDER_GREY);
        } else {
            raylib.DrawText("Blender Camera Mode: GIMBAL_ORBIT", 10, 10, 20, raylib.BLENDER_GREY);
        }

        raylib.DrawFPS(10, screenHeight - 30);

        raylib.EndDrawing();
    }
}
