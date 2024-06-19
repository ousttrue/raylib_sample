const std = @import("std");
const c = @cImport({
    @cInclude("raylib.h");
    @cInclude("rlgl.h");
    // @cInclude("rcamera.h");
    // @cInclude("raymath.h");
});
const zamath = @import("zamath.zig");

const Scene = struct {
    cubePosition: c.Vector3 = .{ .x = 0.0, .y = 0.0, .z = 0.0 },
    rendertargets: []RenderTarget,

    pub fn draw(self: @This(), current: *const RenderTarget) void {
        // c.BeginMode3D(current.*);
        c.rlDrawRenderBatchActive(); // Update and draw internal render batch

        {
            c.rlMatrixMode(c.RL_PROJECTION);
            c.rlPushMatrix();
            c.rlLoadIdentity();
            c.rlMultMatrixf(&current.projection.matrix.m00);
        }
        {
            c.rlMatrixMode(c.RL_MODELVIEW);
            c.rlLoadIdentity();
            c.rlMultMatrixf(&current.orbit.view_matrix.m00);
        }

        c.rlEnableDepthTest(); // Enable DEPTH_TEST for 3D

        for (self.rendertargets) |rendertarget| {
            if (&rendertarget != current) {
                // c.DrawLine3D(camera.position, camera.target, c.DARKBLUE);
            }
        }

        c.DrawCube(self.cubePosition, 2.0, 2.0, 2.0, c.RED);
        c.DrawCubeWires(self.cubePosition, 2.0, 2.0, 2.0, c.MAROON);

        c.DrawGrid(10, 1.0);

        c.EndMode3D();
    }
};

const RenderTarget = struct {
    viewport: zamath.Rect = .{ .x = 0, .y = 0, .width = 1, .height = 1 },
    render_texture: ?c.RenderTexture2D = null,
    // view
    orbit: zamath.CameraOrbit = .{},
    // projection
    projection: zamath.CameraProjection = .{},

    is_active: bool = false,

    fn make(rect: c.Rectangle) @This() {
        var rt = RenderTarget{
            .viewport = .{
                .x = rect.x,
                .y = rect.y,
                .width = rect.width,
                .height = rect.height,
            },
        };
        rt.orbit.update_matrix();
        rt.projection.update_matrix(rt.viewport);
        return rt;
    }

    fn contains(self: Self, cursor: c.Vector2) bool {
        if (cursor.x < self.viewport.width) {
            return false;
        }
        if (cursor.x > (self.viewport.x + self.viewport.width)) {
            return false;
        }
        if (cursor.y < self.viewport.y) {
            return false;
        }
        if (cursor.y > (self.viewport.y + self.viewport.height)) {
            return false;
        }
        return true;
    }

    fn process(self: *Self) bool {
        const wheel = c.GetMouseWheelMoveV();
        const delta = c.GetMouseDelta();

        self.orbit.dolly(wheel.y);

        var active = wheel.y != 0;
        if (c.IsMouseButtonDown(c.MOUSE_BUTTON_RIGHT)) {
            // yaw pitch
            active = true;
            self.orbit.yawDegree += @intFromFloat(delta.x);
            self.orbit.pitchDegree += @intFromFloat(delta.y);
            if (self.orbit.pitchDegree > 89) {
                self.orbit.pitchDegree = 89;
            } else if (self.orbit.pitchDegree < -89) {
                self.orbit.pitchDegree = -89;
            }
        }

        if (c.IsMouseButtonDown(c.MOUSE_BUTTON_MIDDLE)) {
            // camera shift
            active = true;
            const speed = self.orbit.distance * std.math.tan(self.projection.fovy * 0.5) * 2.0 / self.viewport.height;
            self.orbit.shiftX += delta.x * speed;
            self.orbit.shiftY -= delta.y * speed;
        }

        if (active) {
            self.orbit.update_matrix();
            self.projection.update_matrix(self.viewport);
        }
        return active;
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

        scene.draw(self);

        c.EndTextureMode();

        c.DrawTextureRec(
            render_texture.texture,
            .{
                .width = self.viewport.width,
                .height = -self.viewport.height,
            },
            .{
                .x = self.viewport.x,
                .y = self.viewport.y,
            },
            c.WHITE,
        );
    }

    fn get_or_create_render_texture(self: *Self) c.RenderTexture2D {
        if (self.render_texture) |render_texture| {
            return render_texture;
        } else {
            const render_texture = c.LoadRenderTexture(
                @intFromFloat(self.viewport.width),
                @intFromFloat(self.viewport.height),
            );
            self.render_texture = render_texture;
            return render_texture;
        }
    }
};

const Focus = struct {
    rendertargets: [2]RenderTarget,
    active: ?*RenderTarget = null,

    const Self = @This();
    fn make(screen_width: i32, screen_height: i32) Self {
        const half_width = @divTrunc(screen_width, 2);
        // const half_height = @divTrunc(screen_height, 2);

        var focus = Focus{
            .rendertargets = [_]RenderTarget{
                RenderTarget.make(.{
                    .x = 0,
                    .y = 0,
                    .width = @as(f32, @floatFromInt(half_width)),
                    .height = @as(f32, @floatFromInt(screen_height)),
                }),
                RenderTarget.make(.{
                    .x = @as(f32, @floatFromInt(half_width)),
                    .y = 0,
                    .width = @as(f32, @floatFromInt(half_width)),
                    .height = @as(f32, @floatFromInt(screen_height)),
                }),
            },
        };
        for (&focus.rendertargets) |*rendertarget| {
            _ = rendertarget.process();
        }
        return focus;
    }

    fn get_active(self: *Self, cursor: c.Vector2) ?*RenderTarget {
        if (self.active) |rendertarget| {
            return rendertarget;
        } else {
            for (&self.rendertargets) |*rendertarget| {
                if (rendertarget.contains(cursor)) {
                    return rendertarget;
                }
            }
            return null;
        }
    }

    fn set_active(self: *Self, active: ?*RenderTarget) void {
        self.active = active;
    }
};

pub fn main() !void {
    c.InitWindow(1600, 1200, "experiment");
    defer c.CloseWindow();

    const w = c.GetScreenWidth();
    const h = c.GetScreenHeight();

    var focus = Focus.make(w, h);

    const scene = Scene{
        .rendertargets = &focus.rendertargets,
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
            for (&focus.rendertargets) |*rendertarget| {
                if (rendertarget.contains(cursor)) {
                    if (rendertarget.process()) {
                        focus.set_active(rendertarget);
                    }
                }
            }
        }

        for (&focus.rendertargets) |*rendertarget| {
            rendertarget.render(scene);
        }

        c.EndDrawing();
    }
}
