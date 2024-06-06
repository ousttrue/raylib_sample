// /*******************************************************************************************
// *
// *   raygui - controls test suite
// *
// *   TEST CONTROLS:
// *       - GuiDropdownBox()
// *       - GuiCheckBox()
// *       - GuiSpinner()
// *       - GuiValueBox()
// *       - GuiTextBox()
// *       - GuiButton()
// *       - GuiComboBox()
// *       - GuiListView()
// *       - GuiToggleGroup()
// *       - GuiColorPicker()
// *       - GuiSlider()
// *       - GuiSliderBar()
// *       - GuiProgressBar()
// *       - GuiColorBarAlpha()
// *       - GuiScrollPanel()
// *
// *
// *   DEPENDENCIES:
// *       raylib 4.5          - Windowing/input management and drawing
// *       raygui 3.5          - Immediate-mode GUI controls with custom styling and icons
// *
// *   COMPILATION (Windows - MinGW):
// *       gcc -o $(NAME_PART).exe $(FILE_NAME) -I../../src -lraylib -lopengl32 -lgdi32 -std=c99
// *
// *   LICENSE: zlib/libpng
// *
// *   Copyright (c) 2016-2024 Ramon Santamaria (@raysan5)
// *
// **********************************************************************************************/

const raygui = @cImport({
    @cInclude("raylib.h");
    // //#define RAYGUI_DEBUG_RECS_BOUNDS
    // //#define RAYGUI_DEBUG_TEXT_BOUNDS
    //
    // #define RAYGUI_IMPLEMENTATION
    // //#define RAYGUI_CUSTOM_ICONS     // It requires providing gui_icons.h in the same directory
    // //#include "gui_icons.h"          // External icons data provided, it can be generated with rGuiIcons tool
    @cInclude("raygui.h");
});

// // raygui embedded styles
// #include "../styles/style_cyber.h"             // raygui style: cyber
// #include "../styles/style_jungle.h"            // raygui style: jungle
// #include "../styles/style_lavanda.h"           // raygui style: lavanda
// #include "../styles/style_dark.h"              // raygui style: dark
// #include "../styles/style_bluish.h"            // raygui style: bluish
// #include "../styles/style_terminal.h"          // raygui style: terminal
//

//------------------------------------------------------------------------------------
// Program main entry point
//------------------------------------------------------------------------------------
pub fn main() void {
    // Initialization
    //---------------------------------------------------------------------------------------
    const screenWidth = 960;
    const screenHeight = 560;

    raygui.InitWindow(screenWidth, screenHeight, "raygui - controls test suite");
    defer raygui.CloseWindow(); // Close window and OpenGL context
    raygui.SetExitKey(0);

    // GUI controls initialization
    //----------------------------------------------------------------------------------
    var dropdownBox000Active: i32 = 0;
    var dropDown000EditMode = false;

    var dropdownBox001Active: i32 = 0;
    var dropDown001EditMode = false;

    var spinner001Value: i32 = 0;
    var spinnerEditMode = false;

    var valueBox002Value: i32 = 0;
    var valueBoxEditMode = false;

    // const textBoxText: [64]u8 = "Text box";
    // var textBoxEditMode = false;

    // const textBoxMultiText: [1024]u8 = "Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat.\n\nDuis aute irure dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur.\n\nThisisastringlongerthanexpectedwithoutspacestotestcharbreaksforthosecases,checkingifworkingasexpected.\n\nExcepteur sint occaecat cupidatat non proident, sunt in culpa qui officia deserunt mollit anim id est laborum.";
    // var textBoxMultiEditMode = false;

    var listViewScrollIndex: i32 = 0;
    var listViewActive: i32 = -1;

    // var listViewExScrollIndex: i32 = 0;
    // var listViewExActive: i32 = 2;
    // var listViewExFocus: i32 = -1;
    // const listViewExList: [8][]const u8 = .{ "This", "is", "a", "list view", "with", "disable", "elements", "amazing!" };

    var colorPickerValue = raygui.RED;

    var sliderValue: f32 = 50.0;
    // var sliderBarValue: f32 = 60;
    var progressValue: f32 = 0.1;

    var forceSquaredChecked = false;

    var alphaValue: f32 = 0.5;

    //     //int comboBoxActive = 1;
    var visualStyleActive: i32 = 0;
    var prevVisualStyleActive: i32 = 0;

    var toggleGroupActive: i32 = 0;
    var toggleSliderActive: i32 = 0;

    // var viewScroll = raygui.Vector2{};
    //----------------------------------------------------------------------------------

    // Custom GUI font loading
    //Font font = LoadFontEx("fonts/rainyhearts16.ttf", 12, 0, 0);
    //GuiSetFont(font);

    var exitWindow = false;
    var showMessageBox = false;

    // const textInput = [1]u8{0} ** 256;
    // const textInputFileName = [1]u8{0} ** 256;
    var showTextInputBox = false;

    var alpha: f32 = 1.0;

    // DEBUG: Testing how those two properties affect all controls!
    //GuiSetStyle(DEFAULT, TEXT_PADDING, 0);
    //GuiSetStyle(DEFAULT, TEXT_ALIGNMENT, TEXT_ALIGN_CENTER);

    raygui.SetTargetFPS(60);
    //--------------------------------------------------------------------------------------

    // Main game loop
    while (!exitWindow) // Detect window close button or ESC key
    {
        // Update
        //----------------------------------------------------------------------------------
        exitWindow = raygui.WindowShouldClose();

        if (raygui.IsKeyPressed(raygui.KEY_ESCAPE)) showMessageBox = !showMessageBox;

        if (raygui.IsKeyDown(raygui.KEY_LEFT_CONTROL) and raygui.IsKeyPressed(raygui.KEY_S)) showTextInputBox = true;

        if (raygui.IsFileDropped()) {
            const droppedFiles = raygui.LoadDroppedFiles();

            if ((droppedFiles.count > 0) and raygui.IsFileExtension(droppedFiles.paths[0], ".rgs")) raygui.GuiLoadStyle(droppedFiles.paths[0]);

            raygui.UnloadDroppedFiles(droppedFiles); // Clear internal buffers
        }

        //alpha -= 0.002f;
        if (alpha < 0.0) alpha = 0.0;
        if (raygui.IsKeyPressed(raygui.KEY_SPACE)) alpha = 1.0;

        raygui.GuiSetAlpha(alpha);

        //progressValue += 0.002f;
        if (raygui.IsKeyPressed(raygui.KEY_LEFT)) {
            progressValue -= 0.1;
        } else if (raygui.IsKeyPressed(raygui.KEY_RIGHT)) {
            progressValue += 0.1;
        }
        if (progressValue > 1.0) {
            progressValue = 1.0;
        } else if (progressValue < 0.0) {
            progressValue = 0.0;
        }

        if (visualStyleActive != prevVisualStyleActive) {
            raygui.GuiLoadStyleDefault();

            // switch (visualStyleActive) {
            //     // case 0: break;      // Default style
            //     1 => raygui.GuiLoadStyleJungle(),
            //     2 => raygui.GuiLoadStyleLavanda(),
            //     3 => raygui.GuiLoadStyleDark(),
            //     4 => raygui.GuiLoadStyleBluish(),
            //     5 => raygui.GuiLoadStyleCyber(),
            //     6 => raygui.GuiLoadStyleTerminal(),
            //     // default: break;
            //     else => {},
            // }

            raygui.GuiSetStyle(raygui.LABEL, raygui.TEXT_ALIGNMENT, raygui.TEXT_ALIGN_LEFT);

            prevVisualStyleActive = visualStyleActive;
        }
        //----------------------------------------------------------------------------------

        // Draw
        //----------------------------------------------------------------------------------
        raygui.BeginDrawing();

        // raygui.ClearBackground(raygui.GetColor(@intCast(raygui.GuiGetStyle(raygui.DEFAULT, raygui.BACKGROUND_COLOR))));

        // raygui: controls drawing
        //----------------------------------------------------------------------------------
        // Check all possible events that require GuiLock
        if (dropDown000EditMode or dropDown001EditMode) raygui.GuiLock();

        // First GUI column
        //GuiSetStyle(CHECKBOX, TEXT_ALIGNMENT, TEXT_ALIGN_LEFT);
        _ = raygui.GuiCheckBox(.{ .x = 25, .y = 108, .width = 15, .height = 15 }, "FORCE CHECK!", &forceSquaredChecked);

        raygui.GuiSetStyle(raygui.TEXTBOX, raygui.TEXT_ALIGNMENT, raygui.TEXT_ALIGN_CENTER);
        raygui.GuiSetStyle(raygui.VALUEBOX, raygui.TEXT_ALIGNMENT, raygui.TEXT_ALIGN_LEFT);
        if (raygui.GuiSpinner(.{ .x = 25, .y = 135, .width = 125, .height = 30 }, null, &spinner001Value, 0, 100, spinnerEditMode) != 0) spinnerEditMode = !spinnerEditMode;
        if (raygui.GuiValueBox(.{ .x = 25, .y = 175, .width = 125, .height = 30 }, null, &valueBox002Value, 0, 100, valueBoxEditMode) != 0) valueBoxEditMode = !valueBoxEditMode;
        raygui.GuiSetStyle(raygui.TEXTBOX, raygui.TEXT_ALIGNMENT, raygui.TEXT_ALIGN_LEFT);
        // if (raygui.GuiTextBox(.{ .x=25, .y=215, .width=125, .height=30 }, textBoxText, 64, textBoxEditMode)) textBoxEditMode = !textBoxEditMode;

        raygui.GuiSetStyle(raygui.BUTTON, raygui.TEXT_ALIGNMENT, raygui.TEXT_ALIGN_CENTER);

        if (raygui.GuiButton(.{ .x = 25, .y = 255, .width = 125, .height = 30 }, raygui.GuiIconText(raygui.ICON_FILE_SAVE, "Save File")) != 0) {
            showTextInputBox = true;
        }

        _ = raygui.GuiGroupBox(.{ .x = 25, .y = 310, .width = 125, .height = 150 }, "STATES");
        //GuiLock();
        raygui.GuiSetState(raygui.STATE_NORMAL);
        if (raygui.GuiButton(.{ .x = 30, .y = 320, .width = 115, .height = 30 }, "NORMAL") != 0) {}
        raygui.GuiSetState(raygui.STATE_FOCUSED);
        if (raygui.GuiButton(.{ .x = 30, .y = 355, .width = 115, .height = 30 }, "FOCUSED") != 0) {}
        raygui.GuiSetState(raygui.STATE_PRESSED);
        if (raygui.GuiButton(.{ .x = 30, .y = 390, .width = 115, .height = 30 }, "#15#PRESSED") != 0) {}
        raygui.GuiSetState(raygui.STATE_DISABLED);
        if (raygui.GuiButton(.{ .x = 30, .y = 425, .width = 115, .height = 30 }, "DISABLED") != 0) {}
        raygui.GuiSetState(raygui.STATE_NORMAL);
        //GuiUnlock();

        _ = raygui.GuiComboBox(.{ .x = 25, .y = 480, .width = 125, .height = 30 }, "default;Jungle;Lavanda;Dark;Bluish;Cyber;Terminal", &visualStyleActive);

        // NOTE: GuiDropdownBox must draw after any other control that can be covered on unfolding
        raygui.GuiUnlock();
        raygui.GuiSetStyle(raygui.DROPDOWNBOX, raygui.TEXT_PADDING, 4);
        raygui.GuiSetStyle(raygui.DROPDOWNBOX, raygui.TEXT_ALIGNMENT, raygui.TEXT_ALIGN_LEFT);
        if (raygui.GuiDropdownBox(.{ .x = 25, .y = 65, .width = 125, .height = 30 }, "#01#ONE;#02#TWO;#03#THREE;#04#FOUR", &dropdownBox001Active, dropDown001EditMode) != 0) dropDown001EditMode = !dropDown001EditMode;
        raygui.GuiSetStyle(raygui.DROPDOWNBOX, raygui.TEXT_ALIGNMENT, raygui.TEXT_ALIGN_CENTER);
        raygui.GuiSetStyle(raygui.DROPDOWNBOX, raygui.TEXT_PADDING, 0);

        if (raygui.GuiDropdownBox(.{ .x = 25, .y = 25, .width = 125, .height = 30 }, "ONE;TWO;THREE", &dropdownBox000Active, dropDown000EditMode) != 0) {
            dropDown000EditMode = !dropDown000EditMode;
        }

        // Second GUI column
        _ = raygui.GuiListView(.{ .x = 165, .y = 25, .width = 140, .height = 124 }, "Charmander;Bulbasaur;#18#Squirtel;Pikachu;Eevee;Pidgey", &listViewScrollIndex, &listViewActive);
        // _ = raygui.GuiListViewEx(.{ .x = 165, .y = 162, .width = 140, .height = 184 }, listViewExList, 8, &listViewExScrollIndex, &listViewExActive, &listViewExFocus);

        //GuiToggle((Rectangle){ .x=165, .y=400, .width=140, .height=25 }, "#1#ONE", &toggleGroupActive);
        _ = raygui.GuiToggleGroup(.{ .x = 165, .y = 360, .width = 140, .height = 24 }, "#1#ONE\n#3#TWO\n#8#THREE\n#23#", &toggleGroupActive);
        //GuiDisable();
        raygui.GuiSetStyle(raygui.SLIDER, raygui.SLIDER_PADDING, 2);
        _ = raygui.GuiToggleSlider(.{ .x = 165, .y = 480, .width = 140, .height = 30 }, "ON;OFF", &toggleSliderActive);
        raygui.GuiSetStyle(raygui.SLIDER, raygui.SLIDER_PADDING, 0);

        // Third GUI column
        _ = raygui.GuiPanel(.{ .x = 320, .y = 25, .width = 225, .height = 140 }, "Panel Info");
        _ = raygui.GuiColorPicker(.{ .x = 320, .y = 185, .width = 196, .height = 192 }, null, &colorPickerValue);

        //GuiDisable();
        _ = raygui.GuiSlider(.{ .x = 355, .y = 400, .width = 165, .height = 20 }, "TEST", raygui.TextFormat("%2.2f", sliderValue), &sliderValue, -50, 100);
        // _ = raygui.GuiSliderBar(.{ .x = 320, .y = 430, .width = 200, .height = 20 }, null, raygui.TextFormat("%i", @as(i32, sliderBarValue)), &sliderBarValue, 0, 100);

        // raygui.GuiProgressBar(.{ .x = 320, .y = 460, .width = 200, .height = 20 }, null, raygui.TextFormat("%i%%", @as(i32, progressValue * 100)), &progressValue, 0.0, 1.0);
        raygui.GuiEnable();

        // NOTE: View rectangle could be used to perform some scissor test
        // var view = raygui.Rectangle{};
        // raygui.GuiScrollPanel(.{ .x = 560, .y = 25, .width = 102, .height = 354 }, null, .{ 560, 25, 300, 1200 }, &viewScroll, &view);

        var mouseCell = raygui.Vector2{};
        _ = raygui.GuiGrid(.{ .x = 560, .y = 25 + 180 + 195, .width = 100, .height = 120 }, null, 20, 3, &mouseCell);

        _ = raygui.GuiColorBarAlpha(.{ .x = 320, .y = 490, .width = 200, .height = 30 }, null, &alphaValue);

        raygui.GuiSetStyle(raygui.DEFAULT, raygui.TEXT_ALIGNMENT_VERTICAL, raygui.TEXT_ALIGN_TOP); // WARNING: Word-wrap does not work as expected in case of no-top alignment
        raygui.GuiSetStyle(raygui.DEFAULT, raygui.TEXT_WRAP_MODE, raygui.TEXT_WRAP_WORD); // WARNING: If wrap mode enabled, text editing is not supported
        // if (raygui.GuiTextBox(.{ .x=678, .y=25, .width=258, .height=492 }, textBoxMultiText, 1024, textBoxMultiEditMode)) {
        //     textBoxMultiEditMode = !textBoxMultiEditMode;
        // }
        raygui.GuiSetStyle(raygui.DEFAULT, raygui.TEXT_WRAP_MODE, raygui.TEXT_WRAP_NONE);
        raygui.GuiSetStyle(raygui.DEFAULT, raygui.TEXT_ALIGNMENT_VERTICAL, raygui.TEXT_ALIGN_MIDDLE);

        raygui.GuiSetStyle(raygui.DEFAULT, raygui.TEXT_ALIGNMENT, raygui.TEXT_ALIGN_LEFT);
        // raygui.GuiStatusBar(.{ .x = 0, .y = @as(f32, raygui.GetScreenHeight()) - 20, .width = @as(f32, raygui.GetScreenWidth()), .height = 20 }, "This is a status bar");
        raygui.GuiSetStyle(raygui.DEFAULT, raygui.TEXT_ALIGNMENT, raygui.TEXT_ALIGN_CENTER);
        //GuiSetStyle(STATUSBAR, TEXT_INDENTATION, 20);

        if (showMessageBox) {
            raygui.DrawRectangle(0, 0, raygui.GetScreenWidth(), raygui.GetScreenHeight(), raygui.Fade(raygui.RAYWHITE, 0.8));
            // const result = raygui.GuiMessageBox(.{ @as(f32, raygui.GetScreenWidth()) / 2 - 125, @as(f32, raygui.GetScreenHeight()) / 2 - 50, 250, 100 }, raygui.GuiIconText(raygui.ICON_EXIT, "Close Window"), "Do you really want to exit?", "Yes;No");

            // if ((result == 0) or (result == 2)) {
            //     showMessageBox = false;
            // } else if (result == 1) {
            //     exitWindow = true;
            // }
        }

        if (showTextInputBox) {
            raygui.DrawRectangle(0, 0, raygui.GetScreenWidth(), raygui.GetScreenHeight(), raygui.Fade(raygui.RAYWHITE, 0.8));
            // const result = raygui.GuiTextInputBox(.{ @as(f32, raygui.GetScreenWidth()) / 2 - 120, @as(f32, raygui.GetScreenHeight()) / 2 - 60, 240, 140 }, raygui.GuiIconText(raygui.ICON_FILE_SAVE, "Save file as..."), "Introduce output file name:", "Ok;Cancel", textInput, 255, null);

            // if (result == 1) {
            //     // TODO: Validate textInput value and save
            //
            //     raygui.TextCopy(textInputFileName, textInput);
            // }
            //
            // if ((result == 0) || (result == 1) || (result == 2)) {
            //     showTextInputBox = false;
            //     raygui.TextCopy(textInput, "");
            // }
        }
        //----------------------------------------------------------------------------------

        raygui.EndDrawing();
        //----------------------------------------------------------------------------------
    }
}
