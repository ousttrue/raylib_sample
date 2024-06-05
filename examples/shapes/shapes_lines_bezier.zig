// *******************************************************************************************
//
//    raylib [shapes] example - Cubic-bezier lines
//
//    Example originally created with raylib 1.7, last time updated with raylib 1.7
//
//    Example licensed under an unmodified zlib/libpng license, which is an OSI-certified,
//    BSD-like license that allows static linking with closed source software
//
//    Copyright (c) 2017-2023 Ramon Santamaria (@raysan5)
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

    raylib.SetConfigFlags(raylib.FLAG_MSAA_4X_HINT);
    raylib.InitWindow(screenWidth, screenHeight, "raylib [shapes] example - cubic-bezier lines");
    defer raylib.CloseWindow(); // Close window and OpenGL context

    var startPoint = raylib.Vector2{ .x = 30, .y = 30 };
    var endPoint = raylib.Vector2{ .x = screenWidth - 30, .y = screenHeight - 30 };
    var moveStartPoint = false;
    var moveEndPoint = false;

    raylib.SetTargetFPS(60); // Set our game to run at 60 frames-per-second
    //--------------------------------------------------------------------------------------

    // Main game loop
    while (!raylib.WindowShouldClose()) // Detect window close button or ESC key
    {
        // Update
        //----------------------------------------------------------------------------------
        const mouse = raylib.GetMousePosition();

        if (raylib.CheckCollisionPointCircle(mouse, startPoint, 10.0) and raylib.IsMouseButtonDown(raylib.MOUSE_BUTTON_LEFT)) {
            moveStartPoint = true;
        } else if (raylib.CheckCollisionPointCircle(mouse, endPoint, 10.0) and raylib.IsMouseButtonDown(raylib.MOUSE_BUTTON_LEFT)) {
            moveEndPoint = true;
        }

        if (moveStartPoint) {
            startPoint = mouse;
            if (raylib.IsMouseButtonReleased(raylib.MOUSE_BUTTON_LEFT)) moveStartPoint = false;
        }

        if (moveEndPoint) {
            endPoint = mouse;
            if (raylib.IsMouseButtonReleased(raylib.MOUSE_BUTTON_LEFT)) moveEndPoint = false;
        }
        //----------------------------------------------------------------------------------

        // Draw
        //----------------------------------------------------------------------------------
        raylib.BeginDrawing();

        raylib.ClearBackground(raylib.RAYWHITE);

        raylib.DrawText("MOVE START-END POINTS WITH MOUSE", 15, 20, 20, raylib.GRAY);

        // Draw line Cubic Bezier, in-out interpolation (easing), no control points
        raylib.DrawLineBezier(startPoint, endPoint, 4.0, raylib.BLUE);

        // Draw start-end spline circles with some details
        raylib.DrawCircleV(
            startPoint,
            if (raylib.CheckCollisionPointCircle(mouse, startPoint, 10.0)) 14 else 8,
            if (moveStartPoint) raylib.RED else raylib.BLUE,
        );
        raylib.DrawCircleV(
            endPoint,
            if (raylib.CheckCollisionPointCircle(mouse, endPoint, 10.0)) 14 else 8,
            if (moveEndPoint) raylib.RED else raylib.BLUE,
        );

        raylib.EndDrawing();
        //----------------------------------------------------------------------------------
    }
}
