// *******************************************************************************************
//
//    raylib [text] example - raylib fonts loading
//
//    NOTE: raylib is distributed with some free to use fonts (even for commercial pourposes!)
//          To view details and credits for those fonts, check raylib license file
//
//    Example originally created with raylib 1.7, last time updated with raylib 3.7
//
//    Example licensed under an unmodified zlib/libpng license, which is an OSI-certified,
//    BSD-like license that allows static linking with closed source software
//
//    Copyright (c) 2017-2023 Ramon Santamaria (@raysan5)
//
// *******************************************************************************************

const raylib = @cImport({
    @cInclude("raylib.h");
});

const MAX_FONTS = 8;

//------------------------------------------------------------------------------------
// Program main entry point
//------------------------------------------------------------------------------------
pub fn main() void {
    // Initialization
    //--------------------------------------------------------------------------------------
    const screenWidth = 800;
    const screenHeight = 450;

    raylib.InitWindow(screenWidth, screenHeight, "raylib [text] example - raylib fonts");
    defer raylib.CloseWindow(); // Close window and OpenGL context

    // NOTE: Textures MUST be loaded after Window initialization (OpenGL context is required)
    const fonts = [MAX_FONTS]raylib.Font{
        raylib.LoadFont("resources/fonts/alagard.png"),
        raylib.LoadFont("resources/fonts/pixelplay.png"),
        raylib.LoadFont("resources/fonts/mecha.png"),
        raylib.LoadFont("resources/fonts/setback.png"),
        raylib.LoadFont("resources/fonts/romulus.png"),
        raylib.LoadFont("resources/fonts/pixantiqua.png"),
        raylib.LoadFont("resources/fonts/alpha_beta.png"),
        raylib.LoadFont("resources/fonts/jupiter_crash.png"),
    };
    // Fonts unloading
    defer for (0..MAX_FONTS) |i| raylib.UnloadFont(fonts[i]);

    const messages = [MAX_FONTS][*c]const u8{
        "ALAGARD FONT designed by Hewett Tsoi",
        "PIXELPLAY FONT designed by Aleksander Shevchuk",
        "MECHA FONT designed by Captain Falcon",
        "SETBACK FONT designed by Brian Kent (AEnigma)",
        "ROMULUS FONT designed by Hewett Tsoi",
        "PIXANTIQUA FONT designed by Gerhard Grossmann",
        "ALPHA_BETA FONT designed by Brian Kent (AEnigma)",
        "JUPITER_CRASH FONT designed by Brian Kent (AEnigma)",
    };

    const spacings = [MAX_FONTS]c_int{ 2, 4, 8, 4, 3, 4, 4, 1 };

    var positions = [_]raylib.Vector2{.{ .x = 0, .y = 0 }} ** MAX_FONTS;

    for (0..MAX_FONTS) |i| {
        positions[i].x = screenWidth / 2.0 - raylib.MeasureTextEx(
            fonts[i],
            messages[i],
            @as(f32, @floatFromInt(fonts[i].baseSize)) * 2.0,
            @as(f32, @floatFromInt(spacings[i])),
        ).x / 2.0;
        positions[i].y = 60.0 + @as(f32, @floatFromInt(fonts[i].baseSize)) + 45.0 * @as(f32, @floatFromInt(i));
    }

    // Small Y position corrections
    positions[3].y += 8;
    positions[4].y += 2;
    positions[7].y -= 8;

    const colors = [MAX_FONTS]raylib.Color{
        raylib.MAROON,
        raylib.ORANGE,
        raylib.DARKGREEN,
        raylib.DARKBLUE,
        raylib.DARKPURPLE,
        raylib.LIME,
        raylib.GOLD,
        raylib.RED,
    };

    raylib.SetTargetFPS(60); // Set our game to run at 60 frames-per-second
    //--------------------------------------------------------------------------------------

    // Main game loop
    while (!raylib.WindowShouldClose()) // Detect window close button or ESC key
    {
        // Update
        //----------------------------------------------------------------------------------
        // TODO: Update your variables here
        //----------------------------------------------------------------------------------

        // Draw
        //----------------------------------------------------------------------------------
        raylib.BeginDrawing();

        raylib.ClearBackground(raylib.RAYWHITE);

        raylib.DrawText("free fonts included with raylib", 250, 20, 20, raylib.DARKGRAY);
        raylib.DrawLine(220, 50, 590, 50, raylib.DARKGRAY);

        for (0..MAX_FONTS) |i| {
            raylib.DrawTextEx(
                fonts[i],
                messages[i],
                positions[i],
                @as(f32, @floatFromInt(fonts[i].baseSize)) * 2.0,
                @as(f32, @floatFromInt(spacings[i])),
                colors[i],
            );
        }

        raylib.EndDrawing();
    }
}
