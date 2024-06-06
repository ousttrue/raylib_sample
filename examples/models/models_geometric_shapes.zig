// *******************************************************************************************
//
//    raylib [models] example - Draw some basic geometric shapes (cube, sphere, cylinder...)
//
//    Example originally created with raylib 1.0, last time updated with raylib 3.5
//
//    Example licensed under an unmodified zlib/libpng license, which is an OSI-certified,
//    BSD-like license that allows static linking with closed source software
//
//    Copyright (c) 2014-2024 Ramon Santamaria (@raysan5)
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

    raylib.InitWindow(screenWidth, screenHeight, "raylib [models] example - geometric shapes");
    defer raylib.CloseWindow(); // Close window and OpenGL context

    // Define the camera to look into our 3d world
    const camera = raylib.Camera{
        .position = .{ .x = 0.0, .y = 10.0, .z = 10.0 },
        .target = .{ .x = 0.0, .y = 0.0, .z = 0.0 },
        .up = .{ .x = 0.0, .y = 1.0, .z = 0.0 },
        .fovy = 45.0,
        .projection = raylib.CAMERA_PERSPECTIVE,
    };

    raylib.SetTargetFPS(60); // Set our game to run at 60 frames-per-second
    //--------------------------------------------------------------------------------------

    // Main game loop
    while (!raylib.WindowShouldClose()) // Detect window close button or ESC key
    {
        // Update
        //----------------------------------------------------------------------------------
        // TODO: Update your variables here
        //----------------------------------------------------------------------------------

        // Draw
        //----------------------------------------------------------------------------------
        raylib.BeginDrawing();

        raylib.ClearBackground(raylib.RAYWHITE);

        raylib.BeginMode3D(camera);

        raylib.DrawCube(.{ .x = -4.0, .y = 0.0, .z = 2.0 }, 2.0, 5.0, 2.0, raylib.RED);
        raylib.DrawCubeWires(.{ .x = -4.0, .y = 0.0, .z = 2.0 }, 2.0, 5.0, 2.0, raylib.GOLD);
        raylib.DrawCubeWires(.{ .x = -4.0, .y = 0.0, .z = -2.0 }, 3.0, 6.0, 2.0, raylib.MAROON);

        raylib.DrawSphere(.{ .x = -1.0, .y = 0.0, .z = -2.0 }, 1.0, raylib.GREEN);
        raylib.DrawSphereWires(.{ .x = 1.0, .y = 0.0, .z = 2.0 }, 2.0, 16, 16, raylib.LIME);

        raylib.DrawCylinder(.{ .x = 4.0, .y = 0.0, .z = -2.0 }, 1.0, 2.0, 3.0, 4, raylib.SKYBLUE);
        raylib.DrawCylinderWires(.{ .x = 4.0, .y = 0.0, .z = -2.0 }, 1.0, 2.0, 3.0, 4, raylib.DARKBLUE);
        raylib.DrawCylinderWires(.{ .x = 4.5, .y = -1.0, .z = 2.0 }, 1.0, 1.0, 2.0, 6, raylib.BROWN);

        raylib.DrawCylinder(.{ .x = 1.0, .y = 0.0, .z = -4.0 }, 0.0, 1.5, 3.0, 8, raylib.GOLD);
        raylib.DrawCylinderWires(.{ .x = 1.0, .y = 0.0, .z = -4.0 }, 0.0, 1.5, 3.0, 8, raylib.PINK);

        raylib.DrawCapsule(.{ .x = -3.0, .y = 1.5, .z = -4.0 }, .{ .x = -4.0, .y = -1.0, .z = -4.0 }, 1.2, 8, 8, raylib.VIOLET);
        raylib.DrawCapsuleWires(.{ .x = -3.0, .y = 1.5, .z = -4.0 }, .{ .x = -4.0, .y = -1.0, .z = -4.0 }, 1.2, 8, 8, raylib.PURPLE);

        raylib.DrawGrid(10, 1.0); // Draw a grid

        raylib.EndMode3D();

        raylib.DrawFPS(10, 10);

        raylib.EndDrawing();
        //----------------------------------------------------------------------------------
    }
}
