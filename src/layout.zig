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

    cursor_x: f32 = 0,
    cursor_y: f32 = 0,

    pub fn make() @This() {
        var rendertarget = RenderTarget{};
        rendertarget.orbit.update_matrix();
        rendertarget.projection.update_matrix(rendertarget.viewport);
        return rendertarget;
    }

    pub fn set_viewport_cursor(
        self: *@This(),
        x: f32,
        y: f32,
        w: f32,
        h: f32,
        cursor_x: f32,
        cursor_y: f32,
    ) bool {
        self.viewport.x = x;
        self.viewport.y = y;
        self.cursor_x = cursor_x - self.viewport.x;
        self.cursor_y = cursor_y - self.viewport.y;
        if (self.viewport.width == w and self.viewport.height == h) {
            return false;
        }

        self.viewport.width = w;
        self.viewport.height = h;
        self.render_texture = null;
        return true;
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

    fn process_mouseevent(self: *@This(), wheel: f32, delta: c.Vector2) bool {
        self.orbit.dolly(wheel);

        var active = wheel != 0;
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

    pub fn begin(self: *@This()) c.Texture2D {
        const render_texture = self.get_or_create_render_texture();

        c.BeginTextureMode(render_texture);
        c.rlViewport(
            @intFromFloat(0),
            @intFromFloat(0),
            @intFromFloat(self.viewport.width),
            @intFromFloat(self.viewport.height),
        );
        c.ClearBackground(c.SKYBLUE);

        return render_texture.texture;
    }

    pub fn begin_camera3D(self: @This()) void {
        c.rlDrawRenderBatchActive(); // Update and draw internal render batch

        {
            c.rlMatrixMode(c.RL_PROJECTION);
            c.rlPushMatrix();
            c.rlLoadIdentity();
            c.rlMultMatrixf(&self.projection.matrix.m00);
        }
        {
            c.rlMatrixMode(c.RL_MODELVIEW);
            c.rlLoadIdentity();
            c.rlMultMatrixf(&self.orbit.view_matrix.m00);
        }

        c.rlEnableDepthTest(); // Enable DEPTH_TEST for 3D
    }

    pub fn end_camera3D(_: @This()) void {
        c.EndMode3D();
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

    pub fn mouse_near_far(self: @This()) struct { zamath.Vec3, zamath.Vec3 } {
        const x = self.orbit.transform_matrix.right();
        const y = self.orbit.transform_matrix.up();
        const z = self.orbit.transform_matrix.forward();
        const t = self.orbit.transform_matrix.translation();
        const tan = std.math.tan(self.projection.fovy / 2);
        // const near = self.projection.z_near;
        const far = self.projection.z_far;
        const aspect = self.viewport.width / self.viewport.height;

        const half_width = self.viewport.width / 2;
        const half_height = self.viewport.height / 2;

        const mouse_h = (self.cursor_x - half_width) / half_width;
        const mouse_v = -(self.cursor_y - half_height) / half_height;
        const horizontal = tan * aspect * mouse_h;
        const vertical = tan * mouse_v;
        // const p0 = t.add(z.scale(near).add(x.scale(near * horizontal)).add(y.scale(near * vertical)));
        const p1 = t.add(z.scale(far).add(x.scale(far * horizontal)).add(y.scale(far * vertical)));

        return .{ t, p1 };
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

    fn get_active_or_hover(self: *@This(), x: f32, y: f32) ?*RenderTarget {
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

    pub fn update(
        self: *@This(),
        viewport_width: i32,
        viewport_height: i32,
        wheel: f32,
        cursor: c.Vector2,
        cursor_delta: c.Vector2,
    ) void {
        const half_width: i32 = @divTrunc(viewport_width, 2);
        var left: f32 = 0;
        for (self.rendertargets.items) |*rendertarget| {
            if (rendertarget.set_viewport_cursor(
                left,
                0,
                @floatFromInt(half_width),
                @floatFromInt(viewport_height),
                cursor.x,
                cursor.y,
            )) {
                rendertarget.projection.update_matrix(rendertarget.viewport);
            }
            left += @floatFromInt(half_width);
        }

        if (self.get_active_or_hover(
            cursor.x,
            cursor.y,
        )) |active| {
            if (active.process_mouseevent(wheel, cursor_delta)) {
                self.set_active(active);
            } else {
                self.set_active(null);
            }
        } else {
            // for (self.rendertargets.items) |*rendertarget| {
            //     if (rendertarget.contains(
            //         cursor.x,
            //         cursor.y,
            //     )) {
            //         if (rendertarget.process_mouseevent(wheel, cursor_delta)) {
            //             self.set_active(rendertarget);
            //         }
            //     }
            // }
        }
    }
};
