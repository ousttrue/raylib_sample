// *******************************************************************************************
//
//    raylib [core] example - 3d cmaera split screen
//
//    Example originally created with raylib 3.7, last time updated with raylib 4.0
//
//    Example contributed by Jeffery Myers (@JeffM2501) and reviewed by Ramon Santamaria (@raysan5)
//
//    Example licensed under an unmodified zlib/libpng license, which is an OSI-certified,
//    BSD-like license that allows static linking with closed source software
//
//    Copyright (c) 2021-2023 Jeffery Myers (@JeffM2501)
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

    raylib.InitWindow(screenWidth, screenHeight, "raylib [core] example - 3d camera split screen");
    defer raylib.CloseWindow(); // Close window and OpenGL context

    // Setup player 1 camera and screen
    var cameraPlayer1 = raylib.Camera{
        .fovy = 45.0,
        .up = .{ .y = 1.0 },
        .target = .{ .y = 1.0 },
        .position = .{
            .z = -3.0,
            .y = 1.0,
        },
    };

    const screenPlayer1 = raylib.LoadRenderTexture(screenWidth / 2, screenHeight);
    defer raylib.UnloadRenderTexture(screenPlayer1); // Unload render texture

    // Setup player two camera and screen
    var cameraPlayer2 = raylib.Camera{
        .fovy = 45.0,
        .up = .{ .y = 1.0 },
        .target = .{ .y = 3.0 },
        .position = .{
            .x = -3.0,
            .y = 3.0,
        },
    };

    const screenPlayer2 = raylib.LoadRenderTexture(screenWidth / 2, screenHeight);
    defer raylib.UnloadRenderTexture(screenPlayer2); // Unload render texture

    // Build a flipped rectangle the size of the split view to use for drawing later
    const splitScreenRect = raylib.Rectangle{
        .x = 0.0,
        .y = 0.0,
        .width = @as(f32, @floatFromInt(screenPlayer1.texture.width)),
        .height = @as(f32, @floatFromInt(-screenPlayer1.texture.height)),
    };

    // Grid data
    const count: i32 = 5;
    const spacing = 4.0;

    raylib.SetTargetFPS(60); // Set our game to run at 60 frames-per-second
    //--------------------------------------------------------------------------------------

    // Main game loop
    while (!raylib.WindowShouldClose()) // Detect window close button or ESC key
    {
        // Update
        //----------------------------------------------------------------------------------
        // If anyone moves this frame, how far will they move based on the time since the last frame
        // this moves thigns at 10 world units per second, regardless of the actual FPS
        const offsetThisFrame = 10.0 * raylib.GetFrameTime();

        // Move Player1 forward and backwards (no turning)
        if (raylib.IsKeyDown(raylib.KEY_W)) {
            cameraPlayer1.position.z += offsetThisFrame;
            cameraPlayer1.target.z += offsetThisFrame;
        } else if (raylib.IsKeyDown(raylib.KEY_S)) {
            cameraPlayer1.position.z -= offsetThisFrame;
            cameraPlayer1.target.z -= offsetThisFrame;
        }

        // Move Player2 forward and backwards (no turning)
        if (raylib.IsKeyDown(raylib.KEY_UP)) {
            cameraPlayer2.position.x += offsetThisFrame;
            cameraPlayer2.target.x += offsetThisFrame;
        } else if (raylib.IsKeyDown(raylib.KEY_DOWN)) {
            cameraPlayer2.position.x -= offsetThisFrame;
            cameraPlayer2.target.x -= offsetThisFrame;
        }
        //----------------------------------------------------------------------------------

        // Draw
        //----------------------------------------------------------------------------------
        // Draw Player1 view to the render texture
        raylib.BeginTextureMode(screenPlayer1);
        raylib.ClearBackground(raylib.SKYBLUE);

        raylib.BeginMode3D(cameraPlayer1);

        // Draw scene: grid of cube trees on a plane to make a "world"
        raylib.DrawPlane(.{ .x = 0, .y = 0, .z = 0 }, .{ .x = 50, .y = 50 }, raylib.BEIGE); // Simple world plane

        {
            var x = @as(f32, -count) * spacing;
            while (x <= @as(f32, count) * spacing) : (x += spacing) {
                var z = @as(f32, -count) * spacing;
                while (z <= @as(f32, count) * spacing) : (z += spacing) {
                    raylib.DrawCube(.{ .x=x, .y=1.5, .z=z }, 1, 1, 1, raylib.LIME);
                    raylib.DrawCube(.{ .x=x, .y=0.5, .z=z }, 0.25, 1, 0.25, raylib.BROWN);
                }
            }
        }

        // Draw a cube at each player's position
        raylib.DrawCube(cameraPlayer1.position, 1, 1, 1, raylib.RED);
        raylib.DrawCube(cameraPlayer2.position, 1, 1, 1, raylib.BLUE);

        raylib.EndMode3D();

        raylib.DrawRectangle(0, 0, @divTrunc(raylib.GetScreenWidth(), 2), 40, raylib.Fade(raylib.RAYWHITE, 0.8));
        raylib.DrawText("PLAYER1: W/S to move", 10, 10, 20, raylib.MAROON);

        raylib.EndTextureMode();

        // Draw Player2 view to the render texture
        raylib.BeginTextureMode(screenPlayer2);
        raylib.ClearBackground(raylib.SKYBLUE);

        raylib.BeginMode3D(cameraPlayer2);

        // Draw scene: grid of cube trees on a plane to make a "world"
        raylib.DrawPlane(.{ .x = 0, .y = 0, .z = 0 }, .{ .x = 50, .y = 50 }, raylib.BEIGE); // Simple world plane

        {
            var x = @as(f32, -count) * spacing;
            while (x <= @as(f32, count) * spacing) : (x += spacing) {
                var z = @as(f32, -count) * spacing;
                while (z <= @as(f32, count) * spacing) : (z += spacing) {
                    raylib.DrawCube(.{ .x = x, .y = 1.5, .z = z }, 1, 1, 1, raylib.LIME);
                    raylib.DrawCube(.{ .x = x, .y = 0.5, .z = z }, 0.25, 1, 0.25, raylib.BROWN);
                }
            }
        }

        // Draw a cube at each player's position
        raylib.DrawCube(cameraPlayer1.position, 1, 1, 1, raylib.RED);
        raylib.DrawCube(cameraPlayer2.position, 1, 1, 1, raylib.BLUE);

        raylib.EndMode3D();

        raylib.DrawRectangle(0, 0, @divTrunc(raylib.GetScreenWidth(), 2), 40, raylib.Fade(raylib.RAYWHITE, 0.8));
        raylib.DrawText("PLAYER2: UP/DOWN to move", 10, 10, 20, raylib.DARKBLUE);

        raylib.EndTextureMode();

        // Draw both views render textures to the screen side by side
        raylib.BeginDrawing();
        raylib.ClearBackground(raylib.BLACK);

        raylib.DrawTextureRec(screenPlayer1.texture, splitScreenRect, .{ .x = 0, .y = 0 }, raylib.WHITE);
        raylib.DrawTextureRec(screenPlayer2.texture, splitScreenRect, .{ .x = screenWidth / 2.0, .y = 0 }, raylib.WHITE);

        raylib.DrawRectangle(
            @divTrunc(raylib.GetScreenWidth(), 2) - 2,
            0,
            4,
            raylib.GetScreenHeight(),
            raylib.LIGHTGRAY,
        );
        raylib.EndDrawing();
    }
}
