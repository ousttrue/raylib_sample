const raygizmo = @cImport({
    @cInclude("raylib.h");
    @cInclude("rlgl.h");
    @cDefine("RAYGIZMO_IMPLEMENTATION", "");
    @cInclude("raygizmo.h");
});

pub fn main() void {
    raygizmo.InitWindow(800, 450, "raygizmo");
    defer raygizmo.CloseWindow();

    const camera = raygizmo.Camera3D{
        .fovy = 45.0,
        .target = .{ .x = 0.0, .y = 0.0, .z = 0.0 },
        .position = .{ .x = 5.0, .y = 5.0, .z = 5.0 },
        .up = .{ .x = 0.0, .y = 1.0, .z = 0.0 },
        .projection = raygizmo.CAMERA_PERSPECTIVE,
    };

    var model = raygizmo.LoadModelFromMesh(raygizmo.GenMeshTorus(0.3, 1.5, 16.0, 16.0));
    defer raygizmo.UnloadModel(model);

    var gizmo = raygizmo.rgizmo_create();
    defer raygizmo.rgizmo_unload();

    while (!raygizmo.WindowShouldClose()) {
        raygizmo.BeginDrawing();
        {
            const position = raygizmo.Vector3{
                .x = model.transform.m12,
                .y = model.transform.m13,
                .z = model.transform.m14,
            };
            raygizmo.rgizmo_update(&gizmo, camera, position);
            model.transform = raygizmo.MatrixMultiply(
                model.transform,
                raygizmo.rgizmo_get_tranform(gizmo, position),
            );

            raygizmo.ClearBackground(raygizmo.BLACK);
            raygizmo.rlEnableDepthTest();

            raygizmo.BeginMode3D(camera);
            {
                raygizmo.DrawModel(model, .{ .x = 0.0, .y = 0.0, .z = 0.0 }, 1.0, raygizmo.PURPLE);
            }
            raygizmo.EndMode3D();

            raygizmo.rgizmo_draw(gizmo, camera, position);
        }
        raygizmo.EndDrawing();
    }
}
