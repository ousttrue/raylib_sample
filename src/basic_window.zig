const raylib = @cImport({
    @cInclude("raylib.h");
});

pub fn main() !void {
    raylib.InitWindow(800, 450, "raylib [core] example - basic window");

    while (!raylib.WindowShouldClose()) {
        raylib.BeginDrawing();
        raylib.ClearBackground(raylib.RAYWHITE);
        raylib.DrawText(
            "Congrats! You created your first window!",
            190,
            200,
            20,
            raylib.LIGHTGRAY,
        );
        raylib.EndDrawing();
    }

    raylib.CloseWindow();
}
