const raygui = @cImport({
    @cInclude("raylib.h");

    // #define RAYGUI_IMPLEMENTATION
    @cInclude("raygui.h");
});

pub fn main() void {
    raygui.InitWindow(400, 200, "raygui - controls test suite");
    raygui.SetTargetFPS(60);

    var showMessageBox = false;

    while (!raygui.WindowShouldClose()) {
        // Draw
        //----------------------------------------------------------------------------------
        raygui.BeginDrawing();
        const style = raygui.GuiGetStyle(raygui.DEFAULT, raygui.BACKGROUND_COLOR);
        // @intCast: attempt to cast negative value to unsigned integer
        raygui.ClearBackground(raygui.GetColor(@bitCast(style)));

        if (raygui.GuiButton(raygui.Rectangle{
            .x = 24,
            .y = 24,
            .width = 120,
            .height = 30,
        }, "#191#Show Message") != 0) showMessageBox = true;

        if (showMessageBox) {
            const result = raygui.GuiMessageBox(raygui.Rectangle{
                .x = 85,
                .y = 70,
                .width = 250,
                .height = 100,
            }, "#191#Message Box", "Hi! This is a message!", "Nice;Cool");

            if (result >= 0) showMessageBox = false;
        }

        raygui.EndDrawing();
    }

    raygui.CloseWindow();
}
