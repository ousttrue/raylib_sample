// *******************************************************************************************
//
//    raylib [shapes] example - draw ring (with gui options)
//
//    Example originally created with raylib 2.5, last time updated with raylib 2.5
//
//    Example contributed by Vlad Adrian (@demizdor) and reviewed by Ramon Santamaria (@raysan5)
//
//    Example licensed under an unmodified zlib/libpng license, which is an OSI-certified,
//    BSD-like license that allows static linking with closed source software
//
//    Copyright (c) 2018-2023 Vlad Adrian (@demizdor) and Ramon Santamaria (@raysan5)
//
// *******************************************************************************************

const raylib = @cImport({
    @cInclude("raylib.h");

    // #define RAYGUI_IMPLEMENTATION
    @cInclude("raygui.h"); // Required for GUI controls
});

//------------------------------------------------------------------------------------
// Program main entry point
//------------------------------------------------------------------------------------
pub fn main() void {
    // Initialization
    //--------------------------------------------------------------------------------------
    const screenWidth = 800;
    const screenHeight = 450;

    raylib.InitWindow(screenWidth, screenHeight, "raylib [shapes] example - draw ring");
    defer raylib.CloseWindow(); // Close window and OpenGL context

    const center = raylib.Vector2{
        .x = @as(f32, @floatFromInt(raylib.GetScreenWidth() - 300)) / 2.0,
        .y = @as(f32, @floatFromInt(raylib.GetScreenHeight())) / 2.0,
    };

    var innerRadius: f32 = 80.0;
    var outerRadius: f32 = 190.0;

    var startAngle: f32 = 0.0;
    var endAngle: f32 = 360.0;
    var segments: f32 = 0.0;

    var drawRing = true;
    var drawRingLines = false;
    var drawCircleLines = false;

    raylib.SetTargetFPS(60); // Set our game to run at 60 frames-per-second
    //--------------------------------------------------------------------------------------

    // Main game loop
    while (!raylib.WindowShouldClose()) // Detect window close button or ESC key
    {
        // Update
        //----------------------------------------------------------------------------------
        // NOTE: All variables update happens inside GUI control functions
        //----------------------------------------------------------------------------------

        // Draw
        //----------------------------------------------------------------------------------
        raylib.BeginDrawing();

        raylib.ClearBackground(raylib.RAYWHITE);

        raylib.DrawLine(500, 0, 500, raylib.GetScreenHeight(), raylib.Fade(raylib.LIGHTGRAY, 0.6));
        raylib.DrawRectangle(500, 0, raylib.GetScreenWidth() - 500, raylib.GetScreenHeight(), raylib.Fade(raylib.LIGHTGRAY, 0.3));

        if (drawRing) raylib.DrawRing(
            center,
            innerRadius,
            outerRadius,
            startAngle,
            endAngle,
            @intFromFloat(segments),
            raylib.Fade(raylib.MAROON, 0.3),
        );
        if (drawRingLines) raylib.DrawRingLines(
            center,
            innerRadius,
            outerRadius,
            startAngle,
            endAngle,
            @intFromFloat(segments),
            raylib.Fade(raylib.BLACK, 0.4),
        );
        if (drawCircleLines) raylib.DrawCircleSectorLines(
            center,
            outerRadius,
            startAngle,
            endAngle,
            @intFromFloat(segments),
            raylib.Fade(raylib.BLACK, 0.4),
        );

        // Draw GUI controls
        //------------------------------------------------------------------------------
        _ = raylib.GuiSliderBar(
            raylib.Rectangle{ .x = 600, .y = 40, .width = 120, .height = 20 },
            "StartAngle",
            null,
            &startAngle,
            -450,
            450,
        );
        _ = raylib.GuiSliderBar(
            raylib.Rectangle{ .x = 600, .y = 70, .width = 120, .height = 20 },
            "EndAngle",
            null,
            &endAngle,
            -450,
            450,
        );

        _ = raylib.GuiSliderBar(
            raylib.Rectangle{ .x = 600, .y = 140, .width = 120, .height = 20 },
            "InnerRadius",
            null,
            &innerRadius,
            0,
            100,
        );
        _ = raylib.GuiSliderBar(
            raylib.Rectangle{ .x = 600, .y = 170, .width = 120, .height = 20 },
            "OuterRadius",
            null,
            &outerRadius,
            0,
            200,
        );

        _ = raylib.GuiSliderBar(
            raylib.Rectangle{ .x = 600, .y = 240, .width = 120, .height = 20 },
            "Segments",
            null,
            &segments,
            0,
            100,
        );

        _ = raylib.GuiCheckBox(
            raylib.Rectangle{ .x = 600, .y = 320, .width = 20, .height = 20 },
            "Draw Ring",
            &drawRing,
        );
        _ = raylib.GuiCheckBox(
            raylib.Rectangle{ .x = 600, .y = 350, .width = 20, .height = 20 },
            "Draw RingLines",
            &drawRingLines,
        );
        _ = raylib.GuiCheckBox(
            raylib.Rectangle{ .x = 600, .y = 380, .width = 20, .height = 20 },
            "Draw CircleLines",
            &drawCircleLines,
        );
        //------------------------------------------------------------------------------

        // const minSegments = std.c.ceilf((endAngle - startAngle) / 90);
        // raylib.DrawText(raylib.TextFormat("MODE: %s", (segments >= minSegments)? "MANUAL" : "AUTO"), 600, 270, 10, (segments >= minSegments)? MAROON : DARKGRAY);

        raylib.DrawFPS(10, 10);

        raylib.EndDrawing();
        //----------------------------------------------------------------------------------
    }
}
