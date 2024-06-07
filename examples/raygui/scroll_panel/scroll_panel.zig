// *******************************************************************************************
//
//    raygui - Controls test
//
//    TEST CONTROLS:
//        - GuiScrollPanel()
//
//    DEPENDENCIES:
//        raylib 4.0  - Windowing/input management and drawing.
//        raygui 3.0  - Immediate-mode GUI controls.
//
//    COMPILATION (Windows - MinGW):
//        gcc -o $(NAME_PART).exe $(FILE_NAME) -I../../src -lraylib -lopengl32 -lgdi32 -std=c99
//
//    COMPILATION (Linux - gcc):
//        gcc -o $(NAME_PART) $(FILE_NAME) -I../../src -lraylib -lGL -lm -lpthread -ldl -lrt -lX11 -std=c99
//
//    LICENSE: zlib/libpng
//
//    Copyright (c) 2019-2024 Vlad Adrian (@Demizdor) and Ramon Santamaria (@raysan5)
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
    const screenHeight = 450;

    raygui.InitWindow(screenWidth, screenHeight, "raygui - GuiScrollPanel()");
    defer raygui.CloseWindow(); // Close window and OpenGL context

    const panelRec = raygui.Rectangle{ .x = 20, .y = 40, .width = 200, .height = 150 };
    var panelContentRec = raygui.Rectangle{ .width = 340, .height = 340 };
    var panelView = raygui.Rectangle{};
    var panelScroll = raygui.Vector2{ .x = 99, .y = -20 };

    var showContentArea = true;

    raygui.SetTargetFPS(60);
    //---------------------------------------------------------------------------------------

    // Main game loop
    while (!raygui.WindowShouldClose()) // Detect window close button or ESC key
    {
        // Update
        //----------------------------------------------------------------------------------
        // TODO: Implement required update logic
        //----------------------------------------------------------------------------------

        // Draw
        //----------------------------------------------------------------------------------
        raygui.BeginDrawing();

        raygui.ClearBackground(raygui.RAYWHITE);

        raygui.DrawText(raygui.TextFormat("[%f, %f]", panelScroll.x, panelScroll.y), 4, 4, 20, raygui.RED);

        _ = raygui.GuiScrollPanel(panelRec, null, panelContentRec, &panelScroll, &panelView);

        _ = raygui.BeginScissorMode(
            @intFromFloat(panelView.x),
            @intFromFloat(panelView.y),
            @intFromFloat(panelView.width),
            @intFromFloat(panelView.height),
        );
        _ = raygui.GuiGrid(
            .{
                .x = panelRec.x + panelScroll.x,
                .y = panelRec.y + panelScroll.y,
                .width = panelContentRec.width,
                .height = panelContentRec.height,
            },
            null,
            16,
            3,
            null,
        );
        raygui.EndScissorMode();

        if (showContentArea) {
            raygui.DrawRectangle(
                @intFromFloat(panelRec.x + panelScroll.x),
                @intFromFloat(panelRec.y + panelScroll.y),
                @intFromFloat(panelContentRec.width),
                @intFromFloat(panelContentRec.height),
                raygui.Fade(raygui.RED, 0.1),
            );
        }

        DrawStyleEditControls();

        _ = raygui.GuiCheckBox(.{ .x = 565, .y = 80, .width = 20, .height = 20 }, "SHOW CONTENT AREA", &showContentArea);

        _ = raygui.GuiSliderBar(
            .{ .x = 590, .y = 385, .width = 145, .height = 15 },
            "WIDTH",
            raygui.TextFormat("%i", panelContentRec.width),
            &panelContentRec.width,
            1,
            600,
        );

        // raygui.GuiSliderBar(.{ 590, 410, 145, 15 }, "HEIGHT", TextFormat("%i", panelContentRec.height), &panelContentRec.height, 1, 400);

        raygui.EndDrawing();
        //----------------------------------------------------------------------------------
    }
}

// Draw and process scroll bar style edition controls
fn DrawStyleEditControls() void {
    // ScrollPanel style controls
    //----------------------------------------------------------
    _ = raygui.GuiGroupBox(.{ .x = 550, .y = 170, .width = 220, .height = 205 }, "SCROLLBAR STYLE");

    var style = raygui.GuiGetStyle(raygui.SCROLLBAR, raygui.BORDER_WIDTH);
    _ = raygui.GuiLabel(.{ .x = 555, .y = 195, .width = 110, .height = 10 }, "BORDER_WIDTH");
    _ = raygui.GuiSpinner(.{ .x = 670, .y = 190, .width = 90, .height = 20 }, null, &style, 0, 6, false);
    raygui.GuiSetStyle(raygui.SCROLLBAR, raygui.BORDER_WIDTH, style);

    style = raygui.GuiGetStyle(raygui.SCROLLBAR, raygui.ARROWS_SIZE);
    _ = raygui.GuiLabel(.{ .x = 555, .y = 220, .width = 110, .height = 10 }, "ARROWS_SIZE");
    _ = raygui.GuiSpinner(.{ .x = 670, .y = 215, .width = 90, .height = 20 }, null, &style, 4, 14, false);
    raygui.GuiSetStyle(raygui.SCROLLBAR, raygui.ARROWS_SIZE, style);

    style = raygui.GuiGetStyle(raygui.SCROLLBAR, raygui.SLIDER_PADDING);
    _ = raygui.GuiLabel(.{ .x = 555, .y = 245, .width = 110, .height = 10 }, "SLIDER_PADDING");
    _ = raygui.GuiSpinner(.{ .x = 670, .y = 240, .width = 90, .height = 20 }, null, &style, 0, 14, false);
    raygui.GuiSetStyle(raygui.SCROLLBAR, raygui.SLIDER_PADDING, style);

    const scrollBarArrows = raygui.GuiGetStyle(raygui.SCROLLBAR, raygui.ARROWS_VISIBLE);
    var isScrollBarArrows = scrollBarArrows != 0;
    _ = raygui.GuiCheckBox(.{ .x = 565, .y = 280, .width = 20, .height = 20 }, "ARROWS_VISIBLE", &isScrollBarArrows);
    raygui.GuiSetStyle(raygui.SCROLLBAR, raygui.ARROWS_VISIBLE, scrollBarArrows);

    style = raygui.GuiGetStyle(raygui.SCROLLBAR, raygui.SLIDER_PADDING);
    _ = raygui.GuiLabel(.{ .x = 555, .y = 325, .width = 110, .height = 10 }, "SLIDER_PADDING");
    _ = raygui.GuiSpinner(.{ .x = 670, .y = 320, .width = 90, .height = 20 }, null, &style, 0, 14, false);
    raygui.GuiSetStyle(raygui.SCROLLBAR, raygui.SLIDER_PADDING, style);

    style = raygui.GuiGetStyle(raygui.SCROLLBAR, raygui.SLIDER_WIDTH);
    _ = raygui.GuiLabel(.{ .x = 555, .y = 350, .width = 110, .height = 10 }, "SLIDER_WIDTH");
    _ = raygui.GuiSpinner(.{ .x = 670, .y = 345, .width = 90, .height = 20 }, null, &style, 2, 100, false);
    raygui.GuiSetStyle(raygui.SCROLLBAR, raygui.SLIDER_WIDTH, style);

    const text = if (raygui.GuiGetStyle(raygui.LISTVIEW, raygui.SCROLLBAR_SIDE) == raygui.SCROLLBAR_LEFT_SIDE) "SCROLLBAR: LEFT" else "SCROLLBAR: RIGHT";
    const toggleScrollBarSide = raygui.GuiGetStyle(raygui.LISTVIEW, raygui.SCROLLBAR_SIDE);
    var isToggleScrollBarSide = toggleScrollBarSide != 0;
    _ = raygui.GuiToggle(.{ .x = 560, .y = 110, .width = 200, .height = 35 }, text, &isToggleScrollBarSide);
    raygui.GuiSetStyle(raygui.LISTVIEW, raygui.SCROLLBAR_SIDE, toggleScrollBarSide);
    //----------------------------------------------------------

    // ScrollBar style controls
    //----------------------------------------------------------
    _ = raygui.GuiGroupBox(.{ .x = 550, .y = 20, .width = 220, .height = 135 }, "SCROLLPANEL STYLE");

    style = raygui.GuiGetStyle(raygui.LISTVIEW, raygui.SCROLLBAR_WIDTH);
    _ = raygui.GuiLabel(.{ .x = 555, .y = 35, .width = 110, .height = 10 }, "SCROLLBAR_WIDTH");
    _ = raygui.GuiSpinner(.{ .x = 670, .y = 30, .width = 90, .height = 20 }, null, &style, 6, 30, false);
    raygui.GuiSetStyle(raygui.LISTVIEW, raygui.SCROLLBAR_WIDTH, style);

    style = raygui.GuiGetStyle(raygui.DEFAULT, raygui.BORDER_WIDTH);
    _ = raygui.GuiLabel(.{ .x = 555, .y = 60, .width = 110, .height = 10 }, "BORDER_WIDTH");
    _ = raygui.GuiSpinner(.{ .x = 670, .y = 55, .width = 90, .height = 20 }, null, &style, 0, 20, false);
    raygui.GuiSetStyle(raygui.DEFAULT, raygui.BORDER_WIDTH, style);
    //----------------------------------------------------------
}
