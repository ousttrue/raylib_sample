// *******************************************************************************************
//
//    raylib [shaders] example - texture tiling
//
//    Example demonstrates how to tile a texture on a 3D model using raylib.
//
//    Example contributed by Luis Almeida (@luis605) and reviewed by Ramon Santamaria (@raysan5)
//
//    Example licensed under an unmodified zlib/libpng license, which is an OSI-certified,
//    BSD-like license that allows static linking with closed source software
//
//    Copyright (c) 2023 Luis Almeida (@luis605)
//
// *******************************************************************************************

const raylib = @cImport({
    @cInclude("raylib.h");
});

const GLSL_VERSION: c_int = 330;

//------------------------------------------------------------------------------------
// Program main entry point
//------------------------------------------------------------------------------------
pub fn main() void {
    // Initialization
    //--------------------------------------------------------------------------------------
    const screenWidth = 800;
    const screenHeight = 450;

    raylib.InitWindow(screenWidth, screenHeight, "raylib [shaders] example - texture tiling");
    defer raylib.CloseWindow(); // Close window and OpenGL context

    // Define the camera to look into our 3d world
    var camera = raylib.Camera3D{
        .position = .{ .x = 4.0, .y = 4.0, .z = 4.0 }, // Camera position
        .target = .{ .x = 0.0, .y = 0.5, .z = 0.0 }, // Camera looking at point
        .up = .{ .x = 0.0, .y = 1.0, .z = 0.0 }, // Camera up vector (rotation towards target)
        .fovy = 45.0, // Camera field-of-view Y
        .projection = raylib.CAMERA_PERSPECTIVE, // Camera projection type
    };

    // Load a cube model
    const cube = raylib.GenMeshCube(1.0, 1.0, 1.0);
    const model = raylib.LoadModelFromMesh(cube);
    defer raylib.UnloadModel(model); // Unload model

    // Load a texture and assign to cube model
    const texture = raylib.LoadTexture("resources/cubicmap_atlas.png");
    defer raylib.UnloadTexture(texture); // Unload texture
    model.materials[0].maps[raylib.MATERIAL_MAP_DIFFUSE].texture = texture;

    // Set the texture tiling using a shader
    const tiling = [2]f32{ 3.0, 3.0 };
    const shader = raylib.LoadShader(0, raylib.TextFormat("resources/shaders/glsl%i/tiling.fs", GLSL_VERSION));
    raylib.SetShaderValue(shader, raylib.GetShaderLocation(shader, "tiling"), &tiling, raylib.SHADER_UNIFORM_VEC2);
    model.materials[0].shader = shader;
    defer raylib.UnloadShader(shader); // Unload shader

    raylib.DisableCursor(); // Limit cursor to relative movement inside the window

    raylib.SetTargetFPS(60); // Set our game to run at 60 frames-per-second
    //--------------------------------------------------------------------------------------

    // Main game loop
    while (!raylib.WindowShouldClose()) // Detect window close button or ESC key
    {
        // Update
        //----------------------------------------------------------------------------------
        raylib.UpdateCamera(&camera, raylib.CAMERA_FREE);

        if (raylib.IsKeyPressed('Z')) camera.target = .{ .x = 0.0, .y = 0.5, .z = 0.0 };
        //----------------------------------------------------------------------------------

        // Draw
        //----------------------------------------------------------------------------------
        raylib.BeginDrawing();

        raylib.ClearBackground(raylib.RAYWHITE);

        raylib.BeginMode3D(camera);

        raylib.BeginShaderMode(shader);
        raylib.DrawModel(model, .{ .x = 0.0, .y = 0.0, .z = 0.0 }, 2.0, raylib.WHITE);
        raylib.EndShaderMode();

        raylib.DrawGrid(10, 1.0);

        raylib.EndMode3D();

        raylib.DrawText("Use mouse to rotate the camera", 10, 10, 20, raylib.DARKGRAY);

        raylib.EndDrawing();
        //----------------------------------------------------------------------------------
    }
}
