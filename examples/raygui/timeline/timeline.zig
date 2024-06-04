const std = @import("std");
const raygui = @cImport({
    @cInclude("raylib.h");
    @cInclude("raymath.h");

    // #define DEF_BORDER_WIDTH 1
    // @cDefine("RAYGUI_IMPLEMENTATION", "");
    @cInclude("raygui.h");
    // @cDefine("_TIMELINE_IMPL_", "");
    @cInclude("timeline.h");
});

var keys = [1]f32{0} ** 52;

var tracks = [_]raygui.track_t{
    .{ .name = "Track0", .nKeys = 4, .keys = &keys[0] },
    .{ .name = "Track1", .nKeys = 6, .keys = &keys[4] },
    .{ .name = "Track2", .nKeys = 6, .keys = &keys[10] },
    .{ .name = "Track3", .nKeys = 6, .keys = &keys[16] },
    .{ .name = "Track4", .nKeys = 2, .keys = &keys[24] },
    .{ .name = "Track5", .nKeys = 2, .keys = &keys[26] },
    .{ .name = "Track6", .nKeys = 2, .keys = &keys[28] },
    .{ .name = "Track7", .nKeys = 2, .keys = &keys[30] },
};

var testtimeline = raygui.timeline_t{
    .frame = 0,
    .minValue = 0.0,
    .maxValue = 255.0,
    .nTracks = 8,
    .cellHeight = 12,
    .tracks = &tracks[0],
    .bounds = .{ .x = 0, .y = 64, .width = 800, .height = 64 },
};

var testtimeline2 = raygui.timeline_t{
    .frame = 0,
    .minValue = 0.0,
    .maxValue = 255.0,
    .nTracks = 8,
    .cellHeight = 16,
    .tracks = &tracks[0],
    .bounds = .{ .x = 0, .y = 250, .width = 1000, .height = 128 },
};

pub fn main() void {
    raygui.SetConfigFlags(raygui.FLAG_WINDOW_RESIZABLE);
    raygui.InitWindow(320, 240, "raygui - portable window");
    defer raygui.CloseWindow(); // Close window and OpenGL context

    var prng = std.rand.DefaultPrng.init(33);
    const rnd = prng.random();
    for (0..32) |q| {
        keys[q] = rnd.float(f32);
    }

    // Main game loop
    while (!raygui.WindowShouldClose()) // Detect window close button or ESC key
    {
        raygui.BeginDrawing();
        raygui.ClearBackground(.{ .r = 32, .g = 32, .b = 32, .a = 32 });
        // draw top timeline
        raygui.Timeline(&testtimeline);
        // draw bottom
        testtimeline2.bounds.width = @as(f32, @floatFromInt(raygui.GetScreenWidth()));
        raygui.Timeline(&testtimeline2);
        raygui.EndDrawing();
    }
}
