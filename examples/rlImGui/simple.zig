// *******************************************************************************************
//
//    raylib-extras [ImGui] example - Simple Integration
//
// 	This is a simple ImGui Integration
// 	It is done using C++ but with C style code
// 	It can be done in C as well if you use the C ImGui wrapper
// 	https://github.com/cimgui/cimgui
//
//    Copyright (c) 2021 Jeffery Myers
//
// *******************************************************************************************/

const raylib = @cImport({
    @cInclude("raylib.h");
    @cInclude("raymath.h");
    // #include "imgui.h"
    @cInclude("rlImGui.h");
});

pub fn main() void {
    // Initialization
    //--------------------------------------------------------------------------------------
    const screenWidth = 1280;
    const screenHeight = 800;

    raylib.SetConfigFlags(raylib.FLAG_MSAA_4X_HINT | raylib.FLAG_VSYNC_HINT | raylib.FLAG_WINDOW_RESIZABLE);
    raylib.InitWindow(screenWidth, screenHeight, "raylib-Extras [ImGui] example - simple ImGui Demo");
    defer raylib.CloseWindow(); // Close window and OpenGL context

    raylib.SetTargetFPS(144);
    raylib.rlImGuiSetup(true);
    defer raylib.rlImGuiShutdown();

    // Main game loop
    while (!raylib.WindowShouldClose()) // Detect window close button or ESC key
    {
        raylib.BeginDrawing();
        raylib.ClearBackground(raylib.DARKGRAY);

        // start ImGui Conent
        raylib.rlImGuiBegin();
        //
        // 		// show ImGui Content
        // 		bool open = true;
        // 		ImGui::ShowDemoWindow(&open);

        // end ImGui Content
        raylib.rlImGuiEnd();

        raylib.EndDrawing();
        //----------------------------------------------------------------------------------
    }
}
