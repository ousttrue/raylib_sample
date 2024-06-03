// *******************************************************************************************
// 
//    raylib [core] example - 3d camera first person
// 
//    Example originally created with raylib 1.3, last time updated with raylib 1.3
// 
//    Example licensed under an unmodified zlib/libpng license, which is an OSI-certified,
//    BSD-like license that allows static linking with closed source software
// 
//    Copyright (c) 2015-2023 Ramon Santamaria (@raysan5)
// 
// *******************************************************************************************

const raylib = @cImport({
    @cInclude("raylib.h");
    @cInclude("rcamera.h");
});

const MAX_COLUMNS = 20;

//------------------------------------------------------------------------------------
// Program main entry point
//------------------------------------------------------------------------------------
pub fn main() void {
    // Initialization
    //--------------------------------------------------------------------------------------
    const screenWidth = 800;
    const screenHeight = 450;

    raylib.InitWindow(screenWidth, screenHeight, "raylib [core] example - 3d camera first person");
    defer raylib.CloseWindow(); // Close window and OpenGL context

    // Define the camera to look into our 3d world (position, target, up vector)
    var camera = raylib.Camera{
        .position = .{ .x = 0.0, .y = 2.0, .z = 4.0 }, // Camera position
        .target = .{ .x = 0.0, .y = 2.0, .z = 0.0 }, // Camera looking at point
        .up = .{ .x = 0.0, .y = 1.0, .z = 0.0 }, // Camera up vector (rotation towards target)
        .fovy = 60.0, // Camera field-of-view Y
        .projection = raylib.CAMERA_PERSPECTIVE, // Camera projection type
    };

    var cameraMode = raylib.CAMERA_FIRST_PERSON;

    // Generates some random columns
    var heights = [_]f32{0.0} ** MAX_COLUMNS;
    var positions = [_]raylib.Vector3{.{}} ** MAX_COLUMNS;
    var colors = [_]raylib.Color{.{}} ** MAX_COLUMNS;

    for (0..MAX_COLUMNS) |i| {
        heights[i] = @floatFromInt(raylib.GetRandomValue(1, 12));
        positions[i] = .{
            .x = @floatFromInt(raylib.GetRandomValue(-15, 15)),
            .y = heights[i] / 2.0,
            .z = @floatFromInt(raylib.GetRandomValue(-15, 15)),
        };
        colors[i] = .{
            .r = @intCast(raylib.GetRandomValue(20, 255)),
            .g = @intCast(raylib.GetRandomValue(10, 55)),
            .b = 30,
            .a = 255,
        };
    }

    raylib.DisableCursor(); // Limit cursor to relative movement inside the window

    raylib.SetTargetFPS(60); // Set our game to run at 60 frames-per-second
    //--------------------------------------------------------------------------------------

    // Main game loop
    while (!raylib.WindowShouldClose()) // Detect window close button or ESC key
    {
        // Update
        //----------------------------------------------------------------------------------
        // Switch camera mode
        if (raylib.IsKeyPressed(raylib.KEY_ONE)) {
            cameraMode = raylib.CAMERA_FREE;
            camera.up = .{ .x = 0.0, .y = 1.0, .z = 0.0 }; // Reset roll
        }

        if (raylib.IsKeyPressed(raylib.KEY_TWO)) {
            cameraMode = raylib.CAMERA_FIRST_PERSON;
            camera.up = .{ .x = 0.0, .y = 1.0, .z = 0.0 }; // Reset roll
        }

        if (raylib.IsKeyPressed(raylib.KEY_THREE)) {
            cameraMode = raylib.CAMERA_THIRD_PERSON;
            camera.up = .{ .x = 0.0, .y = 1.0, .z = 0.0 }; // Reset roll
        }

        if (raylib.IsKeyPressed(raylib.KEY_FOUR)) {
            cameraMode = raylib.CAMERA_ORBITAL;
            camera.up = .{ .x = 0.0, .y = 1.0, .z = 0.0 }; // Reset roll
        }

        // Switch camera projection
        if (raylib.IsKeyPressed(raylib.KEY_P)) {
            if (camera.projection == raylib.CAMERA_PERSPECTIVE) {
                // Create isometric view
                cameraMode = raylib.CAMERA_THIRD_PERSON;
                // Note: The target distance is related to the render distance in the orthographic projection
                camera.position = .{ .x = 0.0, .y = 2.0, .z = -100.0 };
                camera.target = .{ .x = 0.0, .y = 2.0, .z = 0.0 };
                camera.up = .{ .x = 0.0, .y = 1.0, .z = 0.0 };
                camera.projection = raylib.CAMERA_ORTHOGRAPHIC;
                camera.fovy = 20.0; // near plane width in CAMERA_ORTHOGRAPHIC
                raylib.CameraYaw(&camera, -135 * raylib.DEG2RAD, true);
                raylib.CameraPitch(&camera, -45 * raylib.DEG2RAD, true, true, false);
            } else if (camera.projection == raylib.CAMERA_ORTHOGRAPHIC) {
                // Reset to default view
                cameraMode = raylib.CAMERA_THIRD_PERSON;
                camera.position = .{ .x = 0.0, .y = 2.0, .z = 10.0 };
                camera.target = .{ .x = 0.0, .y = 2.0, .z = 0.0 };
                camera.up = .{ .x = 0.0, .y = 1.0, .z = 0.0 };
                camera.projection = raylib.CAMERA_PERSPECTIVE;
                camera.fovy = 60.0;
            }
        }

        // Update camera computes movement internally depending on the camera mode
        // Some default standard keyboard/mouse inputs are hardcoded to simplify use
        // For advance camera controls, it's reecommended to compute camera movement manually
        raylib.UpdateCamera(&camera, cameraMode); // Update camera

        // /*
        //         // Camera PRO usage example (EXPERIMENTAL)
        //         // This new camera function allows custom movement/rotation values to be directly provided
        //         // as input parameters, with this approach, rcamera module is internally independent of raylib inputs
        //         UpdateCameraPro(&camera,
        //             .{
        //                 (IsKeyDown(KEY_W) || IsKeyDown(KEY_UP))*0.1f -      // Move forward-backward
        //                 (IsKeyDown(KEY_S) || IsKeyDown(KEY_DOWN))*0.1f,
        //                 (IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT))*0.1f -   // Move right-left
        //                 (IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT))*0.1f,
        //                 0.0f                                                // Move up-down
        //             },
        //             .{
        //                 GetMouseDelta().x*0.05f,                            // Rotation: yaw
        //                 GetMouseDelta().y*0.05f,                            // Rotation: pitch
        //                 0.0f                                                // Rotation: roll
        //             },
        //             GetMouseWheelMove()*2.0f);                              // Move to target (zoom)
        // */
        //----------------------------------------------------------------------------------

        // Draw
        //----------------------------------------------------------------------------------
        raylib.BeginDrawing();

        raylib.ClearBackground(raylib.RAYWHITE);

        raylib.BeginMode3D(camera);

        raylib.DrawPlane(.{ .x = 0.0, .y = 0.0, .z = 0.0 }, .{ .x = 32.0, .y = 32.0 }, raylib.LIGHTGRAY); // Draw ground
        raylib.DrawCube(.{ .x = -16.0, .y = 2.5, .z = 0.0 }, 1.0, 5.0, 32.0, raylib.BLUE); // Draw a blue wall
        raylib.DrawCube(.{ .x = 16.0, .y = 2.5, .z = 0.0 }, 1.0, 5.0, 32.0, raylib.LIME); // Draw a green wall
        raylib.DrawCube(.{ .x = 0.0, .y = 2.5, .z = 16.0 }, 32.0, 5.0, 1.0, raylib.GOLD); // Draw a yellow wall

        // Draw some cubes around
        for (0..MAX_COLUMNS) |i| {
            raylib.DrawCube(positions[i], 2.0, heights[i], 2.0, colors[i]);
            raylib.DrawCubeWires(positions[i], 2.0, heights[i], 2.0, raylib.MAROON);
        }

        // Draw player cube
        if (cameraMode == raylib.CAMERA_THIRD_PERSON) {
            raylib.DrawCube(camera.target, 0.5, 0.5, 0.5, raylib.PURPLE);
            raylib.DrawCubeWires(camera.target, 0.5, 0.5, 0.5, raylib.DARKPURPLE);
        }

        raylib.EndMode3D();

        // Draw info boxes
        raylib.DrawRectangle(5, 5, 330, 100, raylib.Fade(raylib.SKYBLUE, 0.5));
        raylib.DrawRectangleLines(5, 5, 330, 100, raylib.BLUE);

        raylib.DrawText("Camera controls:", 15, 15, 10, raylib.BLACK);
        raylib.DrawText("- Move keys: W, A, S, D, Space, Left-Ctrl", 15, 30, 10, raylib.BLACK);
        raylib.DrawText("- Look around: arrow keys or mouse", 15, 45, 10, raylib.BLACK);
        raylib.DrawText("- Camera mode keys: 1, 2, 3, 4", 15, 60, 10, raylib.BLACK);
        raylib.DrawText("- Zoom keys: num-plus, num-minus or mouse scroll", 15, 75, 10, raylib.BLACK);
        raylib.DrawText("- Camera projection key: P", 15, 90, 10, raylib.BLACK);

        raylib.DrawRectangle(600, 5, 195, 100, raylib.Fade(raylib.SKYBLUE, 0.5));
        raylib.DrawRectangleLines(600, 5, 195, 100, raylib.BLUE);

        raylib.DrawText("Camera status:", 610, 15, 10, raylib.BLACK);
        // DrawText(TextFormat("- Mode: %s", (cameraMode == CAMERA_FREE) ? "FREE" :
        //                                   (cameraMode == CAMERA_FIRST_PERSON) ? "FIRST_PERSON" :
        //                                   (cameraMode == CAMERA_THIRD_PERSON) ? "THIRD_PERSON" :
        //                                   (cameraMode == CAMERA_ORBITAL) ? "ORBITAL" : "CUSTOM"), 610, 30, 10, BLACK);
        // DrawText(TextFormat("- Projection: %s", (camera.projection == CAMERA_PERSPECTIVE) ? "PERSPECTIVE" :
        //                                         (camera.projection == CAMERA_ORTHOGRAPHIC) ? "ORTHOGRAPHIC" : "CUSTOM"), 610, 45, 10, BLACK);
        raylib.DrawText(
            raylib.TextFormat("- Position: (%06.3f, %06.3f, %06.3f)", camera.position.x, camera.position.y, camera.position.z),
            610,
            60,
            10,
            raylib.BLACK,
        );
        raylib.DrawText(
            raylib.TextFormat("- Target: (%06.3f, %06.3f, %06.3f)", camera.target.x, camera.target.y, camera.target.z),
            610,
            75,
            10,
            raylib.BLACK,
        );
        raylib.DrawText(
            raylib.TextFormat("- Up: (%06.3f, %06.3f, %06.3f)", camera.up.x, camera.up.y, camera.up.z),
            610,
            90,
            10,
            raylib.BLACK,
        );

        raylib.EndDrawing();
        //----------------------------------------------------------------------------------
    }
}

