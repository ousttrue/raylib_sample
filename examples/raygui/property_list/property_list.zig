// *******************************************************************************************
//
//    raygui - custom property list control
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
//    Copyright (c) 2020-2023 Vlad Adrian (@Demizdor) and Ramon Santamaria (@raysan5)
//
// *******************************************************************************************

const raygui = @cImport({
    @cInclude("raylib.h");

    @cDefine("RAYGUI_IMPLEMENTATION", "");
    @cInclude("raygui.h");

    @cUndef("RAYGUI_IMPLEMENTATION"); // Avoid including raygui implementation again

    @cDefine("GUI_PROPERTY_LIST_IMPLEMENTATION", "");
    @cInclude("dm_property_list.h");
});

//------------------------------------------------------------------------------------
// Program main entry point
//------------------------------------------------------------------------------------
pub fn main() void {
    // Initialization
    //---------------------------------------------------------------------------------------
    const screenWidth = 800;
    const screenHeight = 450;

    raygui.InitWindow(screenWidth, screenHeight, "raygui - property list");
    defer raygui.CloseWindow(); // Close window and OpenGL context

    const prop = [_]raygui.GuiDMProperty{
        raygui.PBOOL("Bool", 0, true),
        raygui.PSECTION("#102#SECTION", 0, 2),
        raygui.PINT("Int", 0, 123),
        raygui.PFLOAT("Float", 0, 0.99),
        // PTEXT("Text", 0, (char*)&(char[30]){"Hello!"}, 30),
        raygui.PSELECT("Select", 0, "ONE;TWO;THREE;FOUR", 0),
        raygui.PINT_RANGE("Int Range", 0, 32, 1, 0, 100),
        raygui.PRECT("Rect", 0, 0, 0, 100, 200),
        raygui.PVEC2("Vec2", 0, 20, 20),
        raygui.PVEC3("Vec3", 0, 12, 13, 14),
        raygui.PVEC4("Vec4", 0, 12, 13, 14, 15),
        raygui.PCOLOR("Color", 0, 0, 255, 0, 255),
    };
    defer raygui.GuiDMSaveProperties("test.props", prop, prop.len); // Save properties to `test.props` file at exit

    var focus = 0;
    var scroll = 0; // Needed by GuiDMPropertyList()

    raygui.GuiLoadStyleDefault();

    // Tweak the default raygui style a bit
    raygui.GuiSetStyle(raygui.LISTVIEW, raygui.LIST_ITEMS_HEIGHT, 24);
    raygui.GuiSetStyle(raygui.LISTVIEW, raygui.SCROLLBAR_WIDTH, 12);

    var gridMouseCell = raygui.Vector2{};

    raygui.SetTargetFPS(60);
    //--------------------------------------------------------------------------------------

    // Main game loop
    while (!raygui.WindowShouldClose()) // Detect window close button or ESC key
    {
        // Draw
        //----------------------------------------------------------------------------------
        raygui.BeginDrawing();

        raygui.ClearBackground(raygui.GetColor(@bitCast(raygui.GuiGetStyle(raygui.DEFAULT, raygui.BACKGROUND_COLOR))));

        _ = raygui.GuiGrid(raygui.Rectangle{
            .x = 0,
            .y = 0,
            .width = screenWidth,
            .height = screenHeight,
        }, "Property List", 20.0, 2, &gridMouseCell); // Draw a fancy grid

        raygui.GuiDMPropertyList(.{
            .x = (screenWidth - 180) / 2,
            .y = (screenHeight - 280) / 2,
            .width = 180,
            .height = 280,
        }, prop, prop.len, &focus, &scroll);

        if (prop[0].value.vbool >= 1) {
            raygui.DrawText(raygui.TextFormat("FOCUS:%i | SCROLL:%i | FPS:%i", focus, scroll, raygui.GetFPS()), prop[8].value.v2.x, prop[8].value.v2.y, 20, prop[11].value.vcolor);
        }

        raygui.EndDrawing();
        //----------------------------------------------------------------------------------
    }
}
