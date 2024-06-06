// *******************************************************************************************
//
//    raylib [core] example - World to screen
//
//    Example originally created with raylib 1.3, last time updated with raylib 1.4
//
//    Example licensed under an unmodified zlib/libpng license, which is an OSI-certified,
//    BSD-like license that allows static linking with closed source software
//
//    Copyright (c) 2015-2024 Ramon Santamaria (@raysan5)
//
// *******************************************************************************************/

const raylib = @cImport({
    @cInclude("raylib.h");
});

//------------------------------------------------------------------------------------
// Program main entry point
//------------------------------------------------------------------------------------
pub fn main() void {
    // Initialization
    //--------------------------------------------------------------------------------------
    const screenWidth = 800;
    const screenHeight = 450;

    raylib.InitWindow(screenWidth, screenHeight, "raylib [core] example - core world screen");
    defer raylib.CloseWindow(); // Close window and OpenGL context

    // Define the camera to look into our 3d world
    var camera = raylib.Camera{
        .position = .{ .x = 10.0, .y = 10.0, .z = 10.0 }, // Camera position
        .target = .{ .x = 0.0, .y = 0.0, .z = 0.0 }, // Camera looking at point
        .up = .{ .x = 0.0, .y = 1.0, .z = 0.0 }, // Camera up vector (rotation towards target)
        .fovy = 45.0, // Camera field-of-view Y
        .projection = raylib.CAMERA_PERSPECTIVE, // Camera projection type
    };

    const cubePosition = raylib.Vector3{ .x = 0.0, .y = 0.0, .z = 0.0 };

    raylib.DisableCursor(); // Limit cursor to relative movement inside the window

    raylib.SetTargetFPS(60); // Set our game to run at 60 frames-per-second
    //--------------------------------------------------------------------------------------

    // Main game loop
    while (!raylib.WindowShouldClose()) // Detect window close button or ESC key
    {
        // Update
        //----------------------------------------------------------------------------------
        raylib.UpdateCamera(&camera, raylib.CAMERA_THIRD_PERSON);

        // Calculate cube screen space position (with a little offset to be in top)
        const cubeScreenPosition = raylib.GetWorldToScreen(.{
            .x = cubePosition.x,
            .y = cubePosition.y + 2.5,
            .z = cubePosition.z,
        }, camera);
        //----------------------------------------------------------------------------------

        // Draw
        //----------------------------------------------------------------------------------
        raylib.BeginDrawing();

        raylib.ClearBackground(raylib.RAYWHITE);

        raylib.BeginMode3D(camera);

        raylib.DrawCube(cubePosition, 2.0, 2.0, 2.0, raylib.RED);
        raylib.DrawCubeWires(cubePosition, 2.0, 2.0, 2.0, raylib.MAROON);

        raylib.DrawGrid(10, 1.0);

        raylib.EndMode3D();

        raylib.DrawText(
            "Enemy: 100 / 100",
            @as(i32, @intFromFloat(cubeScreenPosition.x - @as(f32, @floatFromInt(@divTrunc(raylib.MeasureText("Enemy: 100/100", 20), 2))))),
            @as(i32, @intFromFloat(cubeScreenPosition.y)),
            20,
            raylib.BLACK,
        );

        raylib.DrawText(
            raylib.TextFormat("Cube position in screen space coordinates: [%i, %i]", cubeScreenPosition.x, cubeScreenPosition.y),
            10,
            10,
            20,
            raylib.LIME,
        );
        raylib.DrawText(
            "Text 2d should be always on top of the cube",
            10,
            40,
            20,
            raylib.GRAY,
        );

        raylib.EndDrawing();
        //----------------------------------------------------------------------------------
    }
}
