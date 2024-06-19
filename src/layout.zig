pub const c = @cImport({
    @cInclude("raylib.h");
    @cInclude("rlgl.h");
    // @cInclude("rcamera.h");
    // @cInclude("raymath.h");
});
const std = @import("std");
const zamath = @import("zamath.zig");

pub const RenderTarget = struct {
    viewport: zamath.Rect = .{ .x = 0, .y = 0, .width = 1, .height = 1 },
    render_texture: ?c.RenderTexture2D = null,
    // view
    orbit: zamath.CameraOrbit = .{},
    // projection
    projection: zamath.CameraProjection = .{},

    pub fn make() @This() {
        var rendertarget = RenderTarget{};
        rendertarget.orbit.update_matrix();
        rendertarget.projection.update_matrix(rendertarget.viewport);
        return rendertarget;
    }

    pub fn set_viewport(self: *@This(), x: f32, y: f32, w: f32, h: f32) void {
        self.viewport.x = x;
        self.viewport.y = y;
        if (self.viewport.width != w or self.viewport.height != h) {
            self.viewport.width = w;
            self.viewport.height = h;
            self.render_texture = null;
        }
    }

    fn contains(self: @This(), x: f32, y: f32) bool {
        if (x < self.viewport.x) {
            return false;
        }
        if (x > (self.viewport.x + self.viewport.width)) {
            return false;
        }
        if (y < self.viewport.y) {
            return false;
        }
        if (y > (self.viewport.y + self.viewport.height)) {
            return false;
        }
        return true;
    }

    fn process(self: *@This()) bool {
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

    pub fn get_or_create_render_texture(self: *@This()) c.RenderTexture2D {
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

    pub fn begin(self: *@This(), active: bool) c.Texture2D {
        _ = active; // autofix
        const render_texture = self.get_or_create_render_texture();

        c.BeginTextureMode(render_texture);
        c.rlViewport(
            @intFromFloat(0),
            @intFromFloat(0),
            @intFromFloat(self.viewport.width),
            @intFromFloat(self.viewport.height),
        );
        c.ClearBackground(c.SKYBLUE);

        // if (active) {
        //     c.DrawText(
        //         c.TextFormat(
        //             "yaw: %d, pitch: %d, shift: %.3f, %.3f",
        //             self.orbit.yawDegree,
        //             self.orbit.pitchDegree,
        //             self.orbit.shiftX,
        //             self.orbit.shiftY,
        //         ),
        //         0,
        //         0,
        //         20,
        //         c.LIGHTGRAY,
        //     );
        // }
        c.DrawText(
            c.TextFormat(
                "x: %.0f, y: %.0f, width: %.0f, height: %.0f",
                self.viewport.x,
                self.viewport.y,
                self.viewport.width,
                self.viewport.height,
            ),
            0,
            0,
            20,
            c.LIGHTGRAY,
        );

        return render_texture.texture;
    }

    pub fn end(self: @This(), texture: c.Texture2D) void {
        c.EndTextureMode();

        c.DrawTextureRec(
            texture,
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
};

pub const Layout = struct {
    rendertargets: std.ArrayList(RenderTarget),
    active: ?*RenderTarget = null,

    pub fn make(allocator: std.mem.Allocator) !@This() {
        var layout = Layout{
            .rendertargets = std.ArrayList(RenderTarget).init(allocator),
        };
        try layout.rendertargets.append(RenderTarget.make());
        try layout.rendertargets.append(RenderTarget.make());
        return layout;
    }

    pub fn deinit(self: *@This()) void {
        self.rendertargets.deinit();
    }

    fn get_active(self: *@This(), x: f32, y: f32) ?*RenderTarget {
        if (self.active) |rendertarget| {
            return rendertarget;
        } else {
            for (self.rendertargets.items) |*rendertarget| {
                if (rendertarget.contains(x, y)) {
                    return rendertarget;
                }
            }
            return null;
        }
    }

    fn set_active(self: *@This(), active: ?*RenderTarget) void {
        self.active = active;
    }

    pub fn update(self: *@This(), w: i32, h: i32, _x: i32, y: i32) void {
        const width: i32 = @divTrunc(w, 2);
        var x: f32 = 0;
        for (self.rendertargets.items) |*rendertarget| {
            rendertarget.set_viewport(
                x,
                0,
                @floatFromInt(width),
                @floatFromInt(h),
            );
            x += @floatFromInt(width);
        }

        if (self.get_active(@floatFromInt(_x), @floatFromInt(y))) |active| {
            if (active.process()) {
                self.set_active(active);
            } else {
                self.set_active(null);
            }
        } else {
            for (self.rendertargets.items) |*rendertarget| {
                if (rendertarget.contains(@floatFromInt(_x), @floatFromInt(y))) {
                    if (rendertarget.process()) {
                        self.set_active(rendertarget);
                    }
                }
            }
        }
    }
};
