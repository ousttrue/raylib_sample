# zig raylib sample

- [zig](https://github.com/ziglang/zig)-0.12
- [raylib](https://github.com/raysan5/raylib)-5.0
- use @cImport
- [raygui](https://github.com/raysan5/raygui)-4.0

```zig
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
```

- lsp zls works !

## samples

- [raygizmo](https://github.com/alexeykarnachev/raygizmo)
- [RayGUI-timeline](https://github.com/MonstersGoBoom/RayGUI-timeline)
- [raylib-blender-camera](https://github.com/grplyler/raylib-blender-camera)

### WIP

- [BVHView](https://github.com/orangeduck/BVHView)
- [OritCamera](https://gist.github.com/JeffM2501/000787070aef421a00c02ae4cf799ea1)
