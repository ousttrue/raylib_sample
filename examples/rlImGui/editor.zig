// *******************************************************************************************
//
//    raylib-extras [ImGui] example - editor
//
// 	This is a more complex ImGui Integration
// 	It shows how to build windows on top of 2d and 3d views using a render texture
//
//    Copyright (c) 2021 Jeffery Myers
//
// *******************************************************************************************/

const std = @import("std");
const raylib = @cImport({
    @cInclude("raylib.h");
    @cInclude("raymath.h");
    @cDefine("CIMGUI_DEFINE_ENUMS_AND_STRUCTS", "");
    @cDefine("IMGUI_DISABLE_OBSOLETE_FUNCTIONS", "1");
    @cInclude("cimgui.h");
    @cInclude("rlImGui.h");
    // @cInclude("rlImGuiColors.h");
});

var Quit = false;
var ImGuiDemoOpen = false;

fn igGetWindowPos() raylib.ImVec2 {
    var v = raylib.ImVec2{};
    raylib.igGetWindowPos(&v);
    return v;
}
fn igGetWindowContentRegionMin() raylib.ImVec2 {
    var v = raylib.ImVec2{};
    raylib.igGetWindowContentRegionMin(&v);
    return v;
}
fn igGetContentRegionAvail() raylib.ImVec2 {
    var v = raylib.ImVec2{};
    raylib.igGetContentRegionAvail(&v);
    return v;
}

const ImageViewerWindow = struct {
    const ToolMode = enum {
        None,
        Move,
    };

    Open: bool = false,
    Focused: bool = false,
    Camera: raylib.Camera2D,
    ViewTexture: raylib.RenderTexture,
    ImageTexture: raylib.Texture,
    CurrentToolMode: ToolMode = .None,
    ContentRect: raylib.Rectangle = .{},
    Dragging: bool = false,
    LastMousePos: raylib.Vector2 = .{},
    LastTarget: raylib.Vector2 = .{},
    DirtyScene: bool = false,

    fn Make() ImageViewerWindow {
        var window = ImageViewerWindow{
            .Camera = .{
                .zoom = 1,
                .target = .{
                    .x = 0,
                    .y = 0,
                },
                .rotation = 0,
                .offset = .{
                    .x = @as(f32, @floatFromInt(raylib.GetScreenWidth())) / 2.0,
                    .y = @as(f32, @floatFromInt(raylib.GetScreenHeight())) / 2.0,
                },
            },

            .ViewTexture = raylib.LoadRenderTexture(raylib.GetScreenWidth(), raylib.GetScreenHeight()),
            .ImageTexture = raylib.LoadTexture("resources/parrots.png"),
        };

        window.UpdateRenderTexture();

        return window;
    }

    fn Show(self: *@This()) void {
        raylib.igPushStyleVar_Vec2(raylib.ImGuiStyleVar_WindowPadding, .{});
        raylib.igSetNextWindowSizeConstraints(
            .{ .x = 400, .y = 400 },
            .{ .x = @floatFromInt(raylib.GetScreenWidth()), .y = @floatFromInt(raylib.GetScreenHeight()) },
            null,
            null,
        );

        self.Focused = false;

        if (raylib.igBegin(
            "Image Viewer",
            &self.Open,
            raylib.ImGuiWindowFlags_NoScrollbar,
        )) {
            // save off the screen space content rectangle
            self.ContentRect = .{
                .x = igGetWindowPos().x + igGetWindowContentRegionMin().x,
                .y = igGetWindowPos().y + igGetWindowContentRegionMin().y,
                .width = igGetContentRegionAvail().x,
                .height = igGetContentRegionAvail().y,
            };

            self.Focused = raylib.igIsWindowFocused(
                raylib.ImGuiFocusedFlags_RootAndChildWindows,
            );

            const size = igGetContentRegionAvail();

            // center the scratch pad in the view
            const viewRect = raylib.Rectangle{
                .x = @as(f32, @floatFromInt(self.ViewTexture.texture.width)) / 2.0 - size.x / 2.0,
                .y = @as(f32, @floatFromInt(self.ViewTexture.texture.height)) / 2.0 - size.y / 2.0,
                .width = size.x,
                .height = -size.y,
            };

            if (raylib.igBeginChild_Str(
                "Toolbar",
                .{ .x = igGetContentRegionAvail().x, .y = 25 },
                0,
                0,
            )) {
                raylib.igSetCursorPosX(2);
                raylib.igSetCursorPosY(3);

                if (raylib.igButton("None", .{})) {
                    self.CurrentToolMode = .None;
                }
                raylib.igSameLine(0, 1);

                if (raylib.igButton("Move", .{})) {
                    self.CurrentToolMode = .Move;
                }

                raylib.igSameLine(0, 1);
                switch (self.CurrentToolMode) {
                    .None => {
                        raylib.igTextUnformatted("No Tool", null);
                    },
                    .Move => {
                        raylib.igTextUnformatted("Move Tool", null);
                    },
                }

                raylib.igSameLine(0, 1);
                raylib.igTextUnformatted(raylib.TextFormat(
                    "camera target X%f Y%f",
                    self.Camera.target.x,
                    self.Camera.target.y,
                ), null);
                raylib.igEndChild();
            }

            raylib.rlImGuiImageRect(
                &self.ViewTexture.texture,
                @as(i32, @intFromFloat(size.x)),
                @as(i32, @intFromFloat(size.y)),
                viewRect,
            );
        }
        raylib.igEnd();
        raylib.igPopStyleVar(1);
    }

    fn Update(self: *@This()) void {
        if (!self.Open)
            return;

        if (raylib.IsWindowResized()) {
            raylib.UnloadRenderTexture(self.ViewTexture);
            self.ViewTexture = raylib.LoadRenderTexture(
                raylib.GetScreenWidth(),
                raylib.GetScreenHeight(),
            );

            self.Camera.offset.x = @as(f32, @floatFromInt(raylib.GetScreenWidth())) / 2.0;
            self.Camera.offset.y = @as(f32, @floatFromInt(raylib.GetScreenHeight())) / 2.0;
        }

        const mousePos = raylib.GetMousePosition();

        if (self.Focused) {
            if (self.CurrentToolMode == .Move) {
                // only do this tool when the mouse is in the content area of the window
                if (raylib.IsMouseButtonDown(0) and raylib.CheckCollisionPointRec(mousePos, self.ContentRect)) {
                    if (!self.Dragging) {
                        self.LastMousePos = mousePos;
                        self.LastTarget = self.Camera.target;
                    }
                    self.Dragging = true;
                    var mouseDelta = raylib.Vector2Subtract(self.LastMousePos, mousePos);

                    mouseDelta.x /= self.Camera.zoom;
                    mouseDelta.y /= self.Camera.zoom;
                    self.Camera.target = raylib.Vector2Add(self.LastTarget, mouseDelta);

                    self.DirtyScene = true;
                } else {
                    self.Dragging = false;
                }
            }
        } else {
            self.Dragging = false;
        }

        if (self.DirtyScene) {
            self.DirtyScene = false;
            self.UpdateRenderTexture();
        }
    }

    fn UpdateRenderTexture(self: *@This()) void {
        raylib.BeginTextureMode(self.ViewTexture);
        raylib.ClearBackground(raylib.BLUE);

        // camera with our view offset with a world origin of 0,0
        raylib.BeginMode2D(self.Camera);

        // center the image at 0,0
        raylib.DrawTexture(
            self.ImageTexture,
            @divTrunc(self.ImageTexture.width, -2),
            @divTrunc(self.ImageTexture.height, -2),
            raylib.WHITE,
        );

        raylib.EndMode2D();
        raylib.EndTextureMode();
    }

    fn Shutdown(self: *@This()) void {
        raylib.UnloadRenderTexture(self.ViewTexture);
        raylib.UnloadTexture(self.ImageTexture);
    }
};

const SceneViewWindow = struct {
    Open: bool = false,
    Focused: bool = false,
    Camera: raylib.Camera3D,
    ViewTexture: raylib.RenderTexture,
    GridTexture: raylib.Texture2D,

    fn Make() @This() {
        const img = raylib.GenImageChecked(256, 256, 32, 32, raylib.DARKGRAY, raylib.WHITE);
        defer raylib.UnloadImage(img);

        var window = SceneViewWindow{
            .ViewTexture = raylib.LoadRenderTexture(raylib.GetScreenWidth(), raylib.GetScreenHeight()),

            .Camera = .{
                .fovy = 45,
                .up = .{ .y = 1 },
                .position = .{
                    .y = 3,
                    .z = -25,
                },
            },
            .GridTexture = raylib.LoadTextureFromImage(img),
        };

        raylib.GenTextureMipmaps(&window.GridTexture);
        raylib.SetTextureFilter(window.GridTexture, raylib.TEXTURE_FILTER_ANISOTROPIC_16X);
        raylib.SetTextureWrap(window.GridTexture, raylib.TEXTURE_WRAP_CLAMP);

        return window;
    }

    fn Shutdown(self: *@This()) void {
        raylib.UnloadRenderTexture(self.ViewTexture);
        raylib.UnloadTexture(self.GridTexture);
    }

    fn Show(self: *@This()) void {
        raylib.igPushStyleVar_Vec2(raylib.ImGuiStyleVar_WindowPadding, .{});
        raylib.igSetNextWindowSizeConstraints(
            .{ .x = 400, .y = 400 },
            .{ .x = @floatFromInt(raylib.GetScreenWidth()), .y = @floatFromInt(raylib.GetScreenHeight()) },
            null,
            null,
        );

        if (raylib.igBegin("3D View", &self.Open, raylib.ImGuiWindowFlags_NoScrollbar)) {
            self.Focused = raylib.igIsWindowFocused(raylib.ImGuiFocusedFlags_ChildWindows);
            // draw the view
            raylib.rlImGuiImageRenderTextureFit(&self.ViewTexture, true);
        }
        raylib.igEnd();
        raylib.igPopStyleVar(1);
    }

    fn Update(self: *@This()) void {
        if (!self.Open)
            return;

        if (raylib.IsWindowResized()) {
            raylib.UnloadRenderTexture(self.ViewTexture);
            self.ViewTexture = raylib.LoadRenderTexture(raylib.GetScreenWidth(), raylib.GetScreenHeight());
        }

        const period = 10.0;
        const magnitude = 25.0;

        self.Camera.position.x = (@as(f32, @floatCast(std.math.sin(raylib.GetTime() / period))) * magnitude);

        raylib.BeginTextureMode(self.ViewTexture);
        raylib.ClearBackground(raylib.SKYBLUE);

        raylib.BeginMode3D(self.Camera);

        // grid of cube trees on a plane to make a "world"
        raylib.DrawPlane(.{}, .{ .x = 50, .y = 50 }, raylib.BEIGE); // simple world plane
        const spacing = 4.0;
        const count = 5;
        var x: f32 = -count * spacing;
        while (x <= count * spacing) : (x += spacing) {
            var z: f32 = -count * spacing;
            while (z <= count * spacing) : (z += spacing) {
                // const pos = raylib.Vector3{ .x = x, .y = 0.5, .z = z };

                // const min = raylib.Vector3{ .x = x - 0.5, .y = 0, .z = z - 0.5 };
                // const max = raylib.Vector3{ .x = x + 0.5, .y = 1, .z = z + 0.5 };

                raylib.DrawCube(.{ .x = x, .y = 1.5, .z = z }, 1, 1, 1, raylib.GREEN);
                raylib.DrawCube(.{ .x = x, .y = 0.5, .z = z }, 0.25, 1, 0.25, raylib.BROWN);
            }
        }

        raylib.EndMode3D();
        raylib.EndTextureMode();
    }
};

fn DoMainMenu(ImageViewer: *ImageViewerWindow, SceneView: *SceneViewWindow) void {
    if (raylib.igBeginMainMenuBar()) {
        if (raylib.igBeginMenu("File", true)) {
            if (raylib.igMenuItem_Bool("Exit", null, false, true))
                Quit = true;

            raylib.igEndMenu();
        }

        if (raylib.igBeginMenu("Window", true)) {
            _ = raylib.igMenuItem_BoolPtr("ImGui Demo", null, &ImGuiDemoOpen, true);
            _ = raylib.igMenuItem_BoolPtr("Image Viewer", null, &ImageViewer.Open, true);
            _ = raylib.igMenuItem_BoolPtr("3D View", null, &SceneView.Open, true);

            raylib.igEndMenu();
        }
        raylib.igEndMainMenuBar();
    }
}

pub fn main() void {
    // Initialization
    //--------------------------------------------------------------------------------------
    const screenWidth = 1900;
    const screenHeight = 900;

    raylib.SetConfigFlags(raylib.FLAG_MSAA_4X_HINT | raylib.FLAG_VSYNC_HINT);
    raylib.InitWindow(screenWidth, screenHeight, "raylib-Extras [ImGui] example - ImGui Demo");
    defer raylib.CloseWindow(); // Close window and OpenGL context

    raylib.SetTargetFPS(144);
    raylib.rlImGuiSetup(true);
    defer raylib.rlImGuiShutdown();
    raylib.igGetIO().*.ConfigWindowsMoveFromTitleBarOnly = true;

    var ImageViewer = ImageViewerWindow.Make();
    ImageViewer.Open = true;
    defer ImageViewer.Shutdown();

    var SceneView = SceneViewWindow.Make();
    SceneView.Open = true;
    defer SceneView.Shutdown();

    // Main game loop
    while (!raylib.WindowShouldClose() and !Quit) // Detect window close button or ESC key
    {
        ImageViewer.Update();
        SceneView.Update();

        raylib.BeginDrawing();
        raylib.ClearBackground(raylib.DARKGRAY);

        raylib.rlImGuiBegin();
        DoMainMenu(&ImageViewer, &SceneView);

        if (ImGuiDemoOpen)
            raylib.igShowDemoWindow(&ImGuiDemoOpen);

        if (ImageViewer.Open)
            ImageViewer.Show();

        if (SceneView.Open)
            SceneView.Show();

        raylib.rlImGuiEnd();

        raylib.EndDrawing();
        //----------------------------------------------------------------------------------
    }
}
