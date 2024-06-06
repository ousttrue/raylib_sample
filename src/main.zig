const std = @import("std");
const raylib = @cImport({
    @cInclude("raylib.h");
    @cInclude("rcamera.h");
    @cInclude("raymath.h");
});

const OribtCamera = struct {
    yawDegree: i32 = 0,
    pitchDegree: i32 = -40,
    distance: f32 = 10,
    shiftX: f32 = 0,
    shiftY: f32 = 0,

    // prevMousePos: raylib.Vector2 = .{ .x = 0, .y = 0 },
    const Self = @This();
    fn MouseUpdateCamera(
        self: *Self,
        camera: *raylib.Camera3D,
        rect: raylib.Rectangle,
    ) bool {
        const delta = raylib.GetMouseDelta();

        // mouse wheel
        const wheel = raylib.GetMouseWheelMoveV();
        if (wheel.y > 0) {
            self.distance *= 0.9;
        } else if (wheel.y < 0) {
            self.distance *= 1.1;
        }

        var active = false;

        // camera shift
        if (raylib.IsMouseButtonDown(raylib.MOUSE_BUTTON_MIDDLE)) {
            const d = raylib.Vector3Distance(camera.target, camera.position);
            const speed = d * std.math.tan(camera.fovy * 0.5) * 2.0 / rect.height;
            self.shiftX += delta.x * speed;
            self.shiftY += delta.y * speed;
            active = true;
        }

        // yaw pitch
        if (raylib.IsMouseButtonDown(raylib.MOUSE_BUTTON_RIGHT)) {
            self.yawDegree -= @intFromFloat(delta.x);
            self.pitchDegree -= @intFromFloat(delta.y);
            if (self.pitchDegree > 89) {
                self.pitchDegree = 89;
            } else if (self.pitchDegree < -89) {
                self.pitchDegree = -89;
            }
            active = true;
        }

        const pitch = raylib.MatrixRotateX(
            @as(f32, @floatFromInt(self.pitchDegree)) * raylib.DEG2RAD,
        );
        const yaw = raylib.MatrixRotateY(
            @as(f32, @floatFromInt(-self.yawDegree)) * raylib.DEG2RAD,
        );
        const translation = raylib.MatrixTranslate(
            -self.shiftX,
            -self.shiftY,
            --self.distance,
        );

        const view = raylib.MatrixMultiply(
            raylib.MatrixMultiply(yaw, pitch),
            translation,
        );

        const viewInverse = raylib.MatrixInvert(view);

        camera.position = .{
            .x = viewInverse.m12,
            .y = viewInverse.m13,
            .z = viewInverse.m14,
        };
        const forward = raylib.Vector3{
            .x = viewInverse.m8,
            .y = viewInverse.m9,
            .z = viewInverse.m10,
        };
        camera.target = raylib.Vector3Add(
            camera.position,
            raylib.Vector3Scale(forward, self.distance),
        );

        return active;
    }
};

const View = struct {
    rect: raylib.Rectangle = .{},
    camera: raylib.Camera3D = .{},
    orbit: OribtCamera = .{},
    render_texture: ?raylib.RenderTexture2D = null,
    cubePosition: raylib.Vector3 = .{ .x = 0.0, .y = 0.0, .z = 0.0 },
    is_active: bool = false,

    const Self = @This();

    fn make(rect: raylib.Rectangle) Self {
        return .{
            .rect = rect,
            .camera = raylib.Camera3D{
                .position = .{ .x = 0.0, .y = 10.0, .z = 10.0 }, // Camera position
                .target = .{ .x = 0.0, .y = 0.0, .z = 0.0 }, // Camera looking at point
                .up = .{ .x = 0.0, .y = 1.0, .z = 0.0 }, // Camera up vector (rotation towards target)
                .fovy = 45.0, // Camera field-of-view Y
                .projection = raylib.CAMERA_PERSPECTIVE, // Camera mode type
            },
        };
    }

    fn contains(self: Self, cursor: raylib.Vector2) bool {
        if (cursor.x < self.rect.x) {
            return false;
        }
        if (cursor.x > (self.rect.x + self.rect.width)) {
            return false;
        }
        if (cursor.y < self.rect.y) {
            return false;
        }
        if (cursor.y > (self.rect.y + self.rect.height)) {
            return false;
        }
        return true;
    }

    fn process(self: *Self) bool {
        return self.orbit.MouseUpdateCamera(
            &self.camera,
            self.rect,
        );
    }

    fn render(self: *Self) void {
        const render_texture = self.get_or_create_render_texture();

        raylib.BeginTextureMode(render_texture);
        raylib.ClearBackground(if (self.is_active) raylib.RED else raylib.SKYBLUE);

        // raylib.DrawText(
        //     raylib.TextFormat(
        //         "yaw: %d, pitch: %d, shift: %.3f, %.3f, distance: %.3f",
        //         orbit.yawDegree,
        //         orbit.pitchDegree,
        //         orbit.shiftX,
        //         orbit.shiftY,
        //         orbit.distance,
        //     ),
        //     190,
        //     200,
        //     20,
        //     raylib.LIGHTGRAY,
        // );

        // {
        // }

        {
            raylib.BeginMode3D(self.camera);

            raylib.DrawCube(self.cubePosition, 2.0, 2.0, 2.0, raylib.RED);
            raylib.DrawCubeWires(self.cubePosition, 2.0, 2.0, 2.0, raylib.MAROON);

            raylib.DrawGrid(10, 1.0);

            raylib.EndMode3D();
        }

        raylib.EndTextureMode();

        raylib.DrawTextureRec(
            render_texture.texture,
            .{
                .width = self.rect.width,
                .height = -self.rect.height,
            },
            .{
                .x = self.rect.x,
                .y = self.rect.y,
            },
            raylib.WHITE,
        );
    }

    fn get_or_create_render_texture(self: *Self) raylib.RenderTexture2D {
        if (self.render_texture) |render_texture| {
            return render_texture;
        } else {
            const render_texture = raylib.LoadRenderTexture(
                @intFromFloat(self.rect.width),
                @intFromFloat(self.rect.height),
            );
            self.render_texture = render_texture;
            return render_texture;
        }
    }
};

const Focus = struct {
    views: [4]View,
    active: ?*View = null,

    const Self = @This();
    fn make(screen_width: i32, screen_height: i32) Self {
        const half_width = @divTrunc(screen_width, 2);
        const half_height = @divTrunc(screen_height, 2);

        var focus = Focus{
            .views = [_]View{
                View.make(.{
                    .x = 0,
                    .y = 0,
                    .width = @as(f32, @floatFromInt(half_width)),
                    .height = @as(f32, @floatFromInt(half_height)),
                }),
                View.make(.{
                    .x = @as(f32, @floatFromInt(half_width)),
                    .y = 0,
                    .width = @as(f32, @floatFromInt(half_width)),
                    .height = @as(f32, @floatFromInt(half_height)),
                }),
                View.make(.{
                    .x = 0,
                    .y = @as(f32, @floatFromInt(half_height)),
                    .width = @as(f32, @floatFromInt(half_width)),
                    .height = @as(f32, @floatFromInt(half_height)),
                }),
                View.make(.{
                    .x = @as(f32, @floatFromInt(half_width)),
                    .y = @as(f32, @floatFromInt(half_height)),
                    .width = @as(f32, @floatFromInt(half_width)),
                    .height = @as(f32, @floatFromInt(half_height)),
                }),
            },
        };
        for (&focus.views) |*view| {
            _ = view.process();
        }
        return focus;
    }

    fn get_active(self: *Self, cursor: raylib.Vector2) ?*View {
        if (self.active) |view| {
            return view;
        } else {
            for (&self.views) |*view| {
                if (view.contains(cursor)) {
                    return view;
                }
            }
            return null;
        }
    }

    fn set_active(self: *Self, active: ?*View) void {
        self.active = active;
    }
};

pub fn main() !void {
    raylib.InitWindow(1024, 786, "experiment");
    defer raylib.CloseWindow();

    const w = raylib.GetScreenWidth();
    const h = raylib.GetScreenHeight();

    var focus = Focus.make(w, h);

    while (!raylib.WindowShouldClose()) {

        // render
        raylib.BeginDrawing();
        raylib.ClearBackground(raylib.RAYWHITE);

        const cursor = raylib.GetMousePosition();
        if (focus.get_active(cursor)) |active| {
            if (active.process()) {
                focus.set_active(active);
            } else {
                focus.set_active(null);
            }
        } else {
            for (&focus.views) |*view| {
                if (view.contains(cursor)) {
                    if (view.process()) {
                        focus.set_active(view);
                    }
                }
            }
        }

        for (&focus.views) |*view| {
            view.render();
        }

        raylib.EndDrawing();
    }
}
