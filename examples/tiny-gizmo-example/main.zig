// This is free and unencumbered software released into the public domain.
// For more information, please refer to <http://unlicense.org>
const std = @import("std");
const c = @import("c.zig");
const orbit_camera = @import("orbit_camera.zig");
const rdrag = @import("rdrag.zig");
const drawable = @import("drawable.zig");
const gizmo_dragger = @import("gizmo_dragger.zig");
const teapot = @cImport({
    @cInclude("teapot.h");
});

pub fn main() !void {
    const allocator = std.heap.page_allocator;

    c.InitWindow(1280, 800, "tiny-gizmo-example-app");
    defer c.CloseWindow();

    var a = drawable.Drawable{
        .name = "first-example-gizmo",
        .position = .{ .x = -2, .y = 0, .z = 0 },
    };
    try a.load_floats(&teapot.teapot_vertices, &teapot.teapot_triangles, false);

    var b = drawable.Drawable{
        .name = "second-example-gizmo",
        .position = .{ .x = 2, .y = 0, .z = 0 },
    };
    try b.load_floats(&teapot.teapot_vertices, &teapot.teapot_triangles, false);

    var scene = [_]*drawable.Drawable{
        &a,
        &b,
    };

    var orbit = orbit_camera.OrbitCamera{};
    var camera = c.Camera3D{
        .position = .{ .x = 0, .y = 1.5, .z = 10 },
        .target = .{ .x = 0, .y = 0, .z = 0 },
        .up = .{ .x = 0, .y = 1, .z = 0 },
        .fovy = 45.0,
        .projection = c.CAMERA_PERSPECTIVE,
    };
    orbit.update_view(&camera);

    var yawpitch = orbit_camera.CameraYawPitchDragger{
        ._camera = &camera,
        ._orbit = &orbit,
    };
    var right_drag = rdrag.make_dragger(&yawpitch);

    var shift = orbit_camera.CameraShiftDragger{
        ._camera = &camera,
        ._orbit = &orbit,
    };
    var middle_drag = rdrag.make_dragger(&shift);

    var gizmo = try gizmo_dragger.TRSGizmo.init(allocator, &camera, &scene);
    defer gizmo.deinit();

    var gizmo_mesh = drawable.Drawable{
        .name = "gizmo-mesh",
    };
    var left_drag = rdrag.make_dragger(&gizmo);

    while (!c.WindowShouldClose()) {
        const w = c.GetScreenWidth();
        const h = c.GetScreenHeight();
        const cursor = c.GetMousePosition();

        const active_hotkey = gizmo_dragger.Hotkey{
            .hotkey_ctrl = c.IsKeyDown(c.KEY_LEFT_CONTROL),
            .hotkey_translate = c.IsKeyDown(c.KEY_T),
            .hotkey_rotate = c.IsKeyDown(c.KEY_R),
            .hotkey_scale = c.IsKeyDown(c.KEY_S),
            .hotkey_local = c.IsKeyDown(c.KEY_L),
        };
        gizmo.process_hotkey(w, h, cursor, active_hotkey);

        // camera
        orbit_camera.dolly(&camera);

        // render
        {
            c.BeginDrawing();
            c.rlEnableDepthTest();
            c.rlEnableColorBlend();
            c.glBlendFunc(c.GL_SRC_ALPHA, c.GL_ONE_MINUS_SRC_ALPHA);
            c.rlDisableBackfaceCulling();
            c.ClearBackground(c.RAYWHITE);

            left_drag.process(w, h, cursor, c.IsMouseButtonDown(c.MOUSE_BUTTON_LEFT));
            right_drag.process(w, h, cursor, c.IsMouseButtonDown(c.MOUSE_BUTTON_RIGHT));
            middle_drag.process(w, h, cursor, c.IsMouseButtonDown(c.MOUSE_BUTTON_MIDDLE));

            {
                c.BeginMode3D(camera);
                // teapot
                a.draw();
                b.draw();
                c.DrawGrid(10, 1.0);

                // draw gizmo
                c.glClear(c.GL_DEPTH_BUFFER_BIT);
                gizmo.load(&gizmo_mesh);
                gizmo_mesh.draw();
                c.EndMode3D();
            }

            c.EndDrawing();
        }
    }
}
