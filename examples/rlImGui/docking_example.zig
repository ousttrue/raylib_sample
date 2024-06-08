// *******************************************************************************************
//
//    raylib-extras [ImGui] example - Docking example
//
// 	This is an example of using the ImGui docking features that are part of docking branch
// 	You must replace the default imgui with the code from the docking branch for this to work
// 	https://github.com/ocornut/imgui/tree/docking
//
//    Copyright (c) 2024 Jeffery Myers
//
// *******************************************************************************************/

const c = @cImport({
    @cInclude("raylib.h");
    @cInclude("raymath.h");

    // @cInclude( "imgui.h");
    @cDefine("CIMGUI_DEFINE_ENUMS_AND_STRUCTS", "");
    // @cDefine("IMGUI_DISABLE_OBSOLETE_FUNCTIONS", "1");
    @cDefine("IMGUI_HAS_DOCK", "");
    @cInclude("cimgui.h");
    @cInclude("rlImGui.h");
});

pub fn main() void {
    // Initialization
    //--------------------------------------------------------------------------------------
    const screenWidth = 1280;
    const screenHeight = 800;

    c.SetConfigFlags(c.FLAG_MSAA_4X_HINT | c.FLAG_VSYNC_HINT | c.FLAG_WINDOW_RESIZABLE);
    c.InitWindow(screenWidth, screenHeight, "raylib-Extras [ImGui] example - Docking");
    defer c.CloseWindow(); // Close window and OpenGL context

    c.SetTargetFPS(144);
    c.rlImGuiSetup(true);
    defer c.rlImGuiShutdown();

    var run = true;

    var showDemoWindow = true;

    // if the linked ImGui has docking, enable it.
    // this will only be true if you use the docking branch of ImGui.
    // #ifdef IMGUI_HAS_DOCK
    c.igGetIO().*.ConfigFlags |= c.ImGuiConfigFlags_DockingEnable;
    // #endif

    // Main game loop
    while (!c.WindowShouldClose() and run) // Detect window close button or ESC key, or a quit from the menu
    {
        c.BeginDrawing();
        c.ClearBackground(c.DARKGRAY);

        // start ImGui content
        c.rlImGuiBegin();

        // 		// if you want windows to dock to the viewport, call this.
        // #ifdef IMGUI_HAS_DOCK
        _ = c.igDockSpaceOverViewport(0, null, 0, null);
        // #endif

        // show a simple menu bar
        if (c.igBeginMainMenuBar()) {
            if (c.igBeginMenu("File", true)) {
                if (c.igMenuItem_Bool("Quit", null, true, true))
                    run = false;

                c.igEndMenu();
            }

            if (c.igBeginMenu("Window", true)) {
                if (c.igMenuItem_Bool("Demo Window", null, showDemoWindow, true))
                    showDemoWindow = !showDemoWindow;

                c.igEndMenu();
            }
            c.igEndMainMenuBar();
        }

        // show some windows

        if (showDemoWindow)
            c.igShowDemoWindow(&showDemoWindow);

        if (c.igBegin("Test Window", null, 0)) {
            c.igTextUnformatted("Another window", null);
        }
        c.igEnd();

        // end ImGui Content
        c.rlImGuiEnd();

        c.EndDrawing();
        //----------------------------------------------------------------------------------
    }
}
