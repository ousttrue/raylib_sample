// *******************************************************************************************
//
//    raygui - portable window
//
//    DEPENDENCIES:
//        raylib 4.0  - Windowing/input management and drawing.
//        raygui 3.0  - Immediate-mode GUI controls.
//
//    COMPILATION (Windows - MinGW):
//        gcc -o $(NAME_PART).exe $(FILE_NAME) -I../../src -lraylib -lopengl32 -lgdi32 -std=c99
//
//    LICENSE: zlib/libpng
//
//    Copyright (c) 2016-2024 Ramon Santamaria (@raysan5)
//
// *********************************************************************************************/

const raygui = @cImport({
    @cInclude("raylib.h");
    // #define RAYGUI_IMPLEMENTATION
    @cInclude("raygui.h");
});

//------------------------------------------------------------------------------------
// Program main entry point
//------------------------------------------------------------------------------------
pub fn main() void {
    // Initialization
    //---------------------------------------------------------------------------------------
    const screenWidth = 800;
    const screenHeight = 600;

    raygui.SetConfigFlags(raygui.FLAG_WINDOW_UNDECORATED);
    raygui.InitWindow(screenWidth, screenHeight, "raygui - portable window");
    defer raygui.CloseWindow(); // Close window and OpenGL context

    // General variables
    var mousePosition = raygui.Vector2{};
    var windowPosition = raygui.Vector2{ .x = 500, .y = 200 };
    var panOffset = mousePosition;
    var dragWindow = false;

    raygui.SetWindowPosition(@intFromFloat(windowPosition.x), @intFromFloat(windowPosition.y));

    var exitWindow = false;

    raygui.SetTargetFPS(60);
    //--------------------------------------------------------------------------------------

    // Main game loop
    while (!exitWindow and !raygui.WindowShouldClose()) // Detect window close button or ESC key
    {
        // Update
        //----------------------------------------------------------------------------------
        mousePosition = raygui.GetMousePosition();

        if (raygui.IsMouseButtonPressed(raygui.MOUSE_LEFT_BUTTON) and !dragWindow) {
            if (raygui.CheckCollisionPointRec(mousePosition, .{ .width = screenWidth, .height = 20 })) {
                dragWindow = true;
                panOffset = mousePosition;
            }
        }

        if (dragWindow) {
            windowPosition.x += (mousePosition.x - panOffset.x);
            windowPosition.y += (mousePosition.y - panOffset.y);

            raygui.SetWindowPosition(@intFromFloat(windowPosition.x), @intFromFloat(windowPosition.y));

            if (raygui.IsMouseButtonReleased(raygui.MOUSE_LEFT_BUTTON)) dragWindow = false;
        }
        //----------------------------------------------------------------------------------

        // Draw
        //----------------------------------------------------------------------------------
        raygui.BeginDrawing();

        raygui.ClearBackground(raygui.RAYWHITE);

        exitWindow = raygui.GuiWindowBox(.{ .width = screenWidth, .height = screenHeight }, "#198# PORTABLE WINDOW") != 0;

        raygui.DrawText(raygui.TextFormat("Mouse Position: [ %.0f, %.0f ]", mousePosition.x, mousePosition.y), 10, 40, 10, raygui.DARKGRAY);
        raygui.DrawText(raygui.TextFormat("Window Position: [ %.0f, %.0f ]", windowPosition.x, windowPosition.y), 10, 60, 10, raygui.DARKGRAY);

        raygui.EndDrawing();
        //----------------------------------------------------------------------------------
    }
}
