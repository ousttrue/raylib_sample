const std = @import("std");
const c = @cImport({
    @cInclude("raylib.h");
    @cInclude("rcamera.h");
    @cInclude("raymath.h");
    @cInclude("rlgl.h");
});

const Scene = struct {
    cubePosition: c.Vector3 = .{ .x = 0.0, .y = 0.0, .z = 0.0 },
    cameras: []*c.Camera,

    pub fn draw(self: @This(), current: *c.Camera) void {
        c.BeginMode3D(current.*);

        for (self.cameras) |camera| {
            if (camera != current) {
                c.DrawLine3D(camera.position, camera.target, c.DARKBLUE);
            }
        }

        c.DrawCube(self.cubePosition, 2.0, 2.0, 2.0, c.RED);
        c.DrawCubeWires(self.cubePosition, 2.0, 2.0, 2.0, c.MAROON);

        c.DrawGrid(10, 1.0);

        c.EndMode3D();
    }
};

fn dolly(camera: *c.Camera3D) void {
    const wheel = c.GetMouseWheelMoveV();
    if (wheel.y > 0) {
        const distance = c.Vector3Distance(camera.target, camera.position);
        c.CameraMoveToTarget(camera, distance * 0.9 - distance);
    } else if (wheel.y < 0) {
        const distance = c.Vector3Distance(camera.target, camera.position);
        c.CameraMoveToTarget(camera, distance * 1.1 - distance);
    }
}

const OribtCamera = struct {
    yawDegree: i32 = 0,
    pitchDegree: i32 = 40,
    shiftX: f32 = 0,
    shiftY: f32 = 0,

    const Self = @This();
    fn MouseUpdateCamera(
        self: *Self,
        distance: f32,
        fovy: f32,
        rect: c.Rectangle,
    ) bool {
        const delta = c.GetMouseDelta();
        var active = false;

        // camera shift
        if (c.IsMouseButtonDown(c.MOUSE_BUTTON_MIDDLE)) {
            const speed = distance * std.math.tan(fovy * 0.5) * 2.0 / rect.height;
            self.shiftX += delta.x * speed;
            self.shiftY += delta.y * speed;
            active = true;
        }

        // yaw pitch
        if (c.IsMouseButtonDown(c.MOUSE_BUTTON_RIGHT)) {
            self.yawDegree -= @intFromFloat(delta.x);
            self.pitchDegree += @intFromFloat(delta.y);
            if (self.pitchDegree > 89) {
                self.pitchDegree = 89;
            } else if (self.pitchDegree < -89) {
                self.pitchDegree = -89;
            }
            active = true;
        }

        return active;
    }

    fn update_view(self: @This(), camera: *c.Camera3D) void {
        const distance = c.Vector3Distance(camera.target, camera.position);
        const pitch = c.MatrixRotateX(
            @as(f32, @floatFromInt(self.pitchDegree)) * c.DEG2RAD,
        );
        const yaw = c.MatrixRotateY(
            @as(f32, @floatFromInt(self.yawDegree)) * c.DEG2RAD,
        );
        const translation = c.MatrixTranslate(
            self.shiftX,
            self.shiftY,
            -distance,
        );

        const camera_transform = c.MatrixMultiply(
            translation,
            c.MatrixMultiply(pitch, yaw),
        );

        camera.position = .{
            .x = camera_transform.m12,
            .y = camera_transform.m13,
            .z = camera_transform.m14,
        };
        const forward = c.Vector3{
            .x = camera_transform.m8,
            .y = camera_transform.m9,
            .z = camera_transform.m10,
        };
        camera.target = c.Vector3Add(
            camera.position,
            c.Vector3Scale(forward, distance),
        );
    }
};

const View = struct {
    rect: c.Rectangle = .{},
    camera: c.Camera3D = .{},
    orbit: OribtCamera = .{},
    render_texture: ?c.RenderTexture2D = null,
    is_active: bool = false,

    const Self = @This();

    fn make(rect: c.Rectangle) Self {
        return .{
            .rect = rect,
            .camera = c.Camera3D{
                .position = .{ .x = 0.0, .y = 10.0, .z = 10.0 }, // Camera position
                .target = .{ .x = 0.0, .y = 0.0, .z = 0.0 }, // Camera looking at point
                .up = .{ .x = 0.0, .y = 1.0, .z = 0.0 }, // Camera up vector (rotation towards target)
                .fovy = 45.0, // Camera field-of-view Y
                .projection = c.CAMERA_PERSPECTIVE, // Camera mode type
            },
        };
    }

    fn contains(self: Self, cursor: c.Vector2) bool {
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
        dolly(&self.camera);

        const distance = c.Vector3Distance(self.camera.target, self.camera.position);
        self.is_active = self.orbit.MouseUpdateCamera(
            distance,
            self.camera.fovy,
            self.rect,
        );
        if (self.is_active) {
            self.orbit.update_view(&self.camera);
        }
        return self.is_active;
    }

    fn render(self: *Self, scene: Scene) void {
        const render_texture = self.get_or_create_render_texture();

        c.BeginTextureMode(render_texture);
        c.ClearBackground(c.SKYBLUE);

        if (self.is_active) {
            c.DrawText(
                c.TextFormat(
                    "yaw: %d, pitch: %d, shift: %.3f, %.3f",
                    self.orbit.yawDegree,
                    self.orbit.pitchDegree,
                    self.orbit.shiftX,
                    self.orbit.shiftY,
                ),
                0,
                0,
                20,
                c.LIGHTGRAY,
            );
        }

        scene.draw(&self.camera);

        c.EndTextureMode();

        c.DrawTextureRec(
            render_texture.texture,
            .{
                .width = self.rect.width,
                .height = -self.rect.height,
            },
            .{
                .x = self.rect.x,
                .y = self.rect.y,
            },
            c.WHITE,
        );
    }

    fn get_or_create_render_texture(self: *Self) c.RenderTexture2D {
        if (self.render_texture) |render_texture| {
            return render_texture;
        } else {
            const render_texture = c.LoadRenderTexture(
                @intFromFloat(self.rect.width),
                @intFromFloat(self.rect.height),
            );
            self.render_texture = render_texture;
            return render_texture;
        }
    }
};

const Focus = struct {
    views: [2]View,
    active: ?*View = null,

    const Self = @This();
    fn make(screen_width: i32, screen_height: i32) Self {
        const half_width = @divTrunc(screen_width, 2);
        // const half_height = @divTrunc(screen_height, 2);

        var focus = Focus{
            .views = [_]View{
                View.make(.{
                    .x = 0,
                    .y = 0,
                    .width = @as(f32, @floatFromInt(half_width)),
                    .height = @as(f32, @floatFromInt(screen_height)),
                }),
                View.make(.{
                    .x = @as(f32, @floatFromInt(half_width)),
                    .y = 0,
                    .width = @as(f32, @floatFromInt(half_width)),
                    .height = @as(f32, @floatFromInt(screen_height)),
                }),
            },
        };
        for (&focus.views) |*view| {
            _ = view.process();
        }
        return focus;
    }

    fn get_active(self: *Self, cursor: c.Vector2) ?*View {
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
    c.InitWindow(1600, 1200, "experiment");
    defer c.CloseWindow();

    const w = c.GetScreenWidth();
    const h = c.GetScreenHeight();

    var focus = Focus.make(w, h);

    var cameras = [_]*c.Camera{
        &focus.views[0].camera,
        &focus.views[1].camera,
    };

    const scene = Scene{
        .cameras = &cameras,
    };

    while (!c.WindowShouldClose()) {

        // render
        c.BeginDrawing();
        c.ClearBackground(c.RAYWHITE);

        const cursor = c.GetMousePosition();
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
            view.render(scene);
        }

        c.EndDrawing();
    }
}
