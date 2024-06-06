const raygui = @cImport({
    @cInclude("raylib.h");
    @cInclude("raygui.h");
    // #include "../../styles/dark/style_dark.h"
});

//     #if !defined(RAYGUI_WINDOWBOX_STATUSBAR_HEIGHT)
const RAYGUI_WINDOWBOX_STATUSBAR_HEIGHT = 24;
//     #endif
//
//     #if !defined(RAYGUI_WINDOW_CLOSEBUTTON_SIZE)
const RAYGUI_WINDOW_CLOSEBUTTON_SIZE = 18;
//     #endif

var window_position = raygui.Vector2{ .x = 10, .y = 10 };
var window_size = raygui.Vector2{ .x = 200, .y = 400 };
var g_minimized = false;
var g_moving = false;
var g_resizing = false;
var g_scroll = raygui.Vector2{};

var window2_position = raygui.Vector2{ .x = 250, .y = 10 };
var window2_size = raygui.Vector2{ .x = 200, .y = 400 };
var minimized2 = false;
var moving2 = false;
var resizing2 = false;
var scroll2 = raygui.Vector2{};

fn GuiWindowFloating(
    position: *raygui.Vector2,
    size: *raygui.Vector2,
    minimized: *bool,
    moving: *bool,
    resizing: *bool,
    draw_content: fn (raygui.Vector2, raygui.Vector2) void,
    content_size: raygui.Vector2,
    scroll: *raygui.Vector2,
    title: []const u8,
) void {
    //
    const close_title_size_delta_half = (RAYGUI_WINDOWBOX_STATUSBAR_HEIGHT - RAYGUI_WINDOW_CLOSEBUTTON_SIZE) / 2;

    // window movement and resize input and collision check
    if (raygui.IsMouseButtonPressed(raygui.MOUSE_LEFT_BUTTON) and !moving.* and !resizing.*) {
        const mouse_position = raygui.GetMousePosition();

        const title_collision_rect = raygui.Rectangle{ .x = position.x, .y = position.y, .width = size.x - (RAYGUI_WINDOW_CLOSEBUTTON_SIZE + close_title_size_delta_half), .height = RAYGUI_WINDOWBOX_STATUSBAR_HEIGHT };
        const resize_collision_rect = raygui.Rectangle{ .x = position.x + size.x - 20, .y = position.y + size.y - 20, .width = 20, .height = 20 };

        if (raygui.CheckCollisionPointRec(mouse_position, title_collision_rect)) {
            moving.* = true;
        } else if (!minimized.* and raygui.CheckCollisionPointRec(mouse_position, resize_collision_rect)) {
            resizing.* = true;
        }
    }

    // window movement and resize update
    if (moving.*) {
        const mouse_delta = raygui.GetMouseDelta();
        position.x += mouse_delta.x;
        position.y += mouse_delta.y;

        if (raygui.IsMouseButtonReleased(raygui.MOUSE_LEFT_BUTTON)) {
            moving.* = false;

            // clamp window position keep it inside the application area
            if (position.x < 0) {
                position.x = 0;
            } else if (position.x > @as(f32, @floatFromInt(raygui.GetScreenWidth())) - size.x) {
                position.x = @as(f32, @floatFromInt(raygui.GetScreenWidth())) - size.x;
            }
            if (position.y < 0) {
                position.y = 0;
            } else if (position.y > @as(f32, @floatFromInt(raygui.GetScreenHeight()))) {
                position.y = @as(f32, @floatFromInt(raygui.GetScreenHeight() - RAYGUI_WINDOWBOX_STATUSBAR_HEIGHT));
            }
        }
    } else if (resizing.*) {
        const mouse = raygui.GetMousePosition();
        if (mouse.x > position.x)
            size.x = mouse.x - position.x;
        if (mouse.y > position.y)
            size.y = mouse.y - position.y;

        // clamp window size to an arbitrary minimum value and the window size as the maximum
        if (size.x < 100) {
            size.x = 100;
        } else if (size.x > @as(f32, @floatFromInt(raygui.GetScreenWidth()))) {
            size.x = @as(f32, @floatFromInt(raygui.GetScreenWidth()));
        }
        if (size.y < 100) {
            size.y = 100;
        } else if (size.y > @as(f32, @floatFromInt(raygui.GetScreenHeight()))) {
            size.y = @as(f32, @floatFromInt(raygui.GetScreenHeight()));
        }

        if (raygui.IsMouseButtonReleased(raygui.MOUSE_LEFT_BUTTON)) {
            resizing.* = false;
        }
    }

    // window and content drawing with scissor and scroll area
    if (minimized.*) {
        _ = raygui.GuiStatusBar(.{ .x = position.x, .y = position.y, .width = size.x, .height = RAYGUI_WINDOWBOX_STATUSBAR_HEIGHT }, &title[0]);

        if (raygui.GuiButton(.{ .x = position.x + size.x - RAYGUI_WINDOW_CLOSEBUTTON_SIZE - close_title_size_delta_half, .y = position.y + close_title_size_delta_half, .width = RAYGUI_WINDOW_CLOSEBUTTON_SIZE, .height = RAYGUI_WINDOW_CLOSEBUTTON_SIZE }, "#120#") != 0) {
            minimized.* = false;
        }
    } else {
        minimized.* = raygui.GuiWindowBox(.{ .x = position.x, .y = position.y, .width = size.x, .height = size.y }, &title[0]) != 0;

        // scissor and draw content within a scroll panel
        // if (draw_content != null)
        {
            var scissor = raygui.Rectangle{};
            _ = raygui.GuiScrollPanel(.{ .x = position.x, .y = position.y + RAYGUI_WINDOWBOX_STATUSBAR_HEIGHT, .width = size.x, .height = size.y - RAYGUI_WINDOWBOX_STATUSBAR_HEIGHT }, null, .{ .x = position.x, .y = position.y, .width = content_size.x, .height = content_size.y }, scroll, &scissor);

            const require_scissor = size.x < content_size.x or size.y < content_size.y;

            if (require_scissor) {
                raygui.BeginScissorMode(
                    @intFromFloat(scissor.x),
                    @intFromFloat(scissor.y),
                    @intFromFloat(scissor.width),
                    @intFromFloat(scissor.height),
                );
            }

            draw_content(position.*, scroll.*);

            if (require_scissor) {
                raygui.EndScissorMode();
            }
        }

        // draw the resize button/icon
        raygui.GuiDrawIcon(
            71,
            @as(i32, @intFromFloat(position.x + size.x)) - 20,
            @as(i32, @intFromFloat(position.y + size.y)) - 20,
            1,
            raygui.WHITE,
        );
    }
}

fn DrawContent(position: raygui.Vector2, scroll: raygui.Vector2) void {
    _ = raygui.GuiButton(.{ .x = position.x + 20 + scroll.x, .y = position.y + 50 + scroll.y, .width = 100, .height = 25 }, "Button 1");
    _ = raygui.GuiButton(.{ .x = position.x + 20 + scroll.x, .y = position.y + 100 + scroll.y, .width = 100, .height = 25 }, "Button 2");
    _ = raygui.GuiButton(.{ .x = position.x + 20 + scroll.x, .y = position.y + 150 + scroll.y, .width = 100, .height = 25 }, "Button 3");
    _ = raygui.GuiLabel(.{ .x = position.x + 20 + scroll.x, .y = position.y + 200 + scroll.y, .width = 250, .height = 25 }, "A Label");
    _ = raygui.GuiLabel(.{ .x = position.x + 20 + scroll.x, .y = position.y + 250 + scroll.y, .width = 250, .height = 25 }, "Another Label");
    _ = raygui.GuiLabel(.{ .x = position.x + 20 + scroll.x, .y = position.y + 300 + scroll.y, .width = 250, .height = 25 }, "Yet Another Label");
}

pub fn main() void {
    raygui.InitWindow(960, 560, "raygui - floating window example");
    defer raygui.CloseWindow();
    raygui.SetTargetFPS(60);
    // raygui.GuiLoadStyleDark();

    while (!raygui.WindowShouldClose()) {
        raygui.BeginDrawing();
        raygui.ClearBackground(raygui.DARKGREEN);
        GuiWindowFloating(
            &window_position,
            &window_size,
            &g_minimized,
            &g_moving,
            &g_resizing,
            DrawContent,
            .{ .x = 140, .y = 320 },
            &g_scroll,
            "Movable & Scalable Window",
        );
        GuiWindowFloating(
            &window2_position,
            &window2_size,
            &minimized2,
            &moving2,
            &resizing2,
            DrawContent,
            .{ .x = 140, .y = 320 },
            &scroll2,
            "Another window",
        );
        raygui.EndDrawing();
    }
}
