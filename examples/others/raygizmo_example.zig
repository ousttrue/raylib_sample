const raygizmo = @import("raygizmo");
const c = raygizmo.c;

pub fn main() void {
    c.InitWindow(800, 450, "raygizmo");
    defer c.CloseWindow();

    const camera = c.Camera3D{
        .fovy = 45.0,
        .target = .{ .x = 0.0, .y = 0.0, .z = 0.0 },
        .position = .{ .x = 5.0, .y = 5.0, .z = 5.0 },
        .up = .{ .x = 0.0, .y = 1.0, .z = 0.0 },
        .projection = c.CAMERA_PERSPECTIVE,
    };

    var model = c.LoadModelFromMesh(c.GenMeshTorus(0.3, 1.5, 16.0, 16.0));
    defer c.UnloadModel(model);

    var gizmo = raygizmo.rgizmo_create();
    defer raygizmo.rgizmo_unload();

    while (!c.WindowShouldClose()) {
        c.BeginDrawing();
        {
            const position = c.Vector3{
                .x = model.transform.m12,
                .y = model.transform.m13,
                .z = model.transform.m14,
            };
            raygizmo.rgizmo_update(&gizmo, camera, position);
            model.transform = c.MatrixMultiply(
                model.transform,
                raygizmo.rgizmo_get_tranform(gizmo, position),
            );

            c.ClearBackground(c.BLACK);
            c.rlEnableDepthTest();

            c.BeginMode3D(camera);
            {
                c.DrawModel(model, .{ .x = 0.0, .y = 0.0, .z = 0.0 }, 1.0, c.PURPLE);
            }
            c.EndMode3D();

            raygizmo.rgizmo_draw(&gizmo, camera, position);
        }
        c.EndDrawing();
    }
}
