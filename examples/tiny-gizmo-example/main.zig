// This is free and unencumbered software released into the public domain.
// For more information, please refer to <http://unlicense.org>
const c = @import("c.zig");
const orbit_camera = @import("orbit_camera.zig");
const rdrag = @import("rdrag.zig");

pub fn main() void {
    c.InitWindow(1280, 800, "tiny-gizmo-example-app");
    defer c.CloseWindow();

    // auto a = std::make_shared<Drawable>();
    // a->name = "first-example-gizmo";
    // a->position = {-2, 0, 0};
    // a->load({(Vertex *)teapot_vertices, _countof(teapot_vertices) / 6},
    //         teapot_triangles, false);
    //
    // auto b = std::make_shared<Drawable>();
    // b->name = "second-example-gizmo";
    // b->position = {2, 0, 0};
    // b->load({(Vertex *)teapot_vertices, _countof(teapot_vertices) / 6},
    //         teapot_triangles, false);
    //
    // std::list<std::shared_ptr<Drawable>> scene{
    //     a,
    //     b,
    // };

    var orbit = orbit_camera.OrbitCamera{};
    var camera = c.Camera3D{
        .position = .{ .x = 0, .y = 1.5, .z = 10 },
        .target = .{ .x = 0, .y = 0, .z = 0 },
        .up = .{ .x = 0, .y = 1, .z = 0 },
        .fovy = 45.0,
        .projection = c.CAMERA_PERSPECTIVE,
    };
    orbit.update_view(&camera);

    var right_drag = rdrag.make_dragger(orbit_camera.CameraYawPitchDragger{
        ._camera = &camera,
        ._orbit = &orbit,
    });

    var middle_drag = rdrag.make_dragger(orbit_camera.CameraShiftDragger{
        ._camera = &camera,
        ._orbit = &orbit,
    });

    // auto gizmo = std::make_shared<TRSGizmo>(&camera, scene);
    // Drawable gizmo_mesh;
    // Drag left_drag{
    //     .draggable = gizmo,
    // };

    while (!c.WindowShouldClose()) {
        const w = c.GetScreenWidth();
        const h = c.GetScreenHeight();
        const cursor = c.GetMousePosition();

        //   hotkey active_hotkey = {
        //       .hotkey_ctrl = IsKeyDown(KEY_LEFT_CONTROL),
        //       .hotkey_translate = IsKeyDown(KEY_T),
        //       .hotkey_rotate = IsKeyDown(KEY_R),
        //       .hotkey_scale = IsKeyDown(KEY_S),
        //       .hotkey_local = IsKeyDown(KEY_L),
        //   };
        //   gizmo->hotkey(w, h, cursor, active_hotkey);

        // camera
        orbit_camera.dolly(&camera);

        // render
        {
            c.BeginDrawing();
            c.rlEnableDepthTest();
            c.rlEnableColorBlend();
            // c.glBlendFunc(c.GL_SRC_ALPHA, c.GL_ONE_MINUS_SRC_ALPHA);
            c.rlDisableBackfaceCulling();
            c.ClearBackground(c.RAYWHITE);

            //     left_drag.process(w, h, cursor, IsMouseButtonDown(MOUSE_BUTTON_LEFT));
            right_drag.process(w, h, cursor, c.IsMouseButtonDown(c.MOUSE_BUTTON_RIGHT));
            middle_drag.process(w, h, cursor, c.IsMouseButtonDown(c.MOUSE_BUTTON_MIDDLE));

            {
                c.BeginMode3D(camera);
                //       // teapot
                //       a->draw();
                //       b->draw();
                c.DrawGrid(10, 1.0);

                // draw gizmo
                c.glClear(c.GL_DEPTH_BUFFER_BIT);
                //       gizmo->load(&gizmo_mesh);
                //       gizmo_mesh.draw();
                c.EndMode3D();
            }

            c.EndDrawing();
        }
    }
}
