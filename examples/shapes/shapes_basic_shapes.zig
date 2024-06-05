// *******************************************************************************************
//
//    raylib [shapes] example - Draw basic shapes 2d (rectangle, circle, line...)
//
//    Example originally created with raylib 1.0, last time updated with raylib 4.2
//
//    Example licensed under an unmodified zlib/libpng license, which is an OSI-certified,
//    BSD-like license that allows static linking with closed source software
//
//    Copyright (c) 2014-2023 Ramon Santamaria (@raysan5)
//
// *******************************************************************************************

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

    raylib.InitWindow(screenWidth, screenHeight, "raylib [shapes] example - basic shapes drawing");
    defer raylib.CloseWindow(); // Close window and OpenGL context

    var rotation: f32 = 0.0;

    raylib.SetTargetFPS(60); // Set our game to run at 60 frames-per-second
    //--------------------------------------------------------------------------------------

    // Main game loop
    while (!raylib.WindowShouldClose()) // Detect window close button or ESC key
    {
        // Update
        //----------------------------------------------------------------------------------
        rotation += 0.2;
        //----------------------------------------------------------------------------------

        // Draw
        //----------------------------------------------------------------------------------
        raylib.BeginDrawing();

        raylib.ClearBackground(raylib.RAYWHITE);

        raylib.DrawText("some basic shapes available on raylib", 20, 20, 20, raylib.DARKGRAY);

        // Circle shapes and lines
        raylib.DrawCircle(screenWidth / 5, 120, 35, raylib.DARKBLUE);
        raylib.DrawCircleGradient(screenWidth / 5, 220, 60, raylib.GREEN, raylib.SKYBLUE);
        raylib.DrawCircleLines(screenWidth / 5, 340, 80, raylib.DARKBLUE);

        // Rectangle shapes and lines
        raylib.DrawRectangle(screenWidth / 4 * 2 - 60, 100, 120, 60, raylib.RED);
        raylib.DrawRectangleGradientH(screenWidth / 4 * 2 - 90, 170, 180, 130, raylib.MAROON, raylib.GOLD);
        raylib.DrawRectangleLines(screenWidth / 4 * 2 - 40, 320, 80, 60, raylib.ORANGE); // NOTE: Uses QUADS internally, not lines

        // Triangle shapes and lines
        raylib.DrawTriangle(
            .{ .x = @divTrunc(screenWidth, 4.0) * 3.0, .y = 80.0 },
            .{ .x = @divTrunc(screenWidth, 4.0) * 3.0 - 60.0, .y = 150.0 },
            .{ .x = @divTrunc(screenWidth, 4.0) * 3.0 + 60.0, .y = 150.0 },
            raylib.VIOLET,
        );

        raylib.DrawTriangleLines(
            .{ .x = @divTrunc(screenWidth, 4.0) * 3.0, .y = 160.0 },
            .{ .x = @divTrunc(screenWidth, 4.0) * 3.0 - 20.0, .y = 230.0 },
            .{ .x = @divTrunc(screenWidth, 4.0) * 3.0 + 20.0, .y = 230.0 },
            raylib.DARKBLUE,
        );

        // Polygon shapes and lines
        raylib.DrawPoly(
            .{ .x = @divTrunc(screenWidth, 4.0) * 3, .y = 330 },
            6,
            80,
            rotation,
            raylib.BROWN,
        );
        raylib.DrawPolyLines(
            .{ .x = @divTrunc(screenWidth, 4.0) * 3, .y = 330 },
            6,
            90,
            rotation,
            raylib.BROWN,
        );
        raylib.DrawPolyLinesEx(
            .{ .x = @divTrunc(screenWidth, 4.0) * 3, .y = 330 },
            6,
            85,
            rotation,
            6,
            raylib.BEIGE,
        );

        // NOTE: We draw all LINES based shapes together to optimize internal drawing,
        // this way, all LINES are rendered in a single draw pass
        raylib.DrawLine(18, 42, screenWidth - 18, 42, raylib.BLACK);
        raylib.EndDrawing();
        //----------------------------------------------------------------------------------
    }
}
