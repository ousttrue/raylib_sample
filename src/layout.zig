pub const c = @cImport({
    @cInclude("raylib.h");
    @cInclude("rlgl.h");
    // @cInclude("rcamera.h");
    // @cInclude("raymath.h");
});
const std = @import("std");
const zamath = @import("zamath.zig");

pub const RenderTarget = struct {
    camera: zamath.Camera = .{},
    camera_projection: zamath.CameraProjection = .{},
    camera_orbit: zamath.CameraOrbit = .{},
    render_texture: ?c.RenderTexture2D = null,

    pub fn frustum(self: @This()) zamath.Frustum {
        return zamath.Frustum.make(
            self.camera.transform_matrix,
            self.camera_projection.fovy,
            self.camera.viewport.width / self.camera.viewport.height,
            self.camera_projection.z_near,
            self.camera_projection.z_far,
        );
    }

    pub fn set_viewport_cursor(
        self: *@This(),
        x: f32,
        y: f32,
        w: f32,
        h: f32,
        cursor_x: f32,
        cursor_y: f32,
    ) void {
        self.camera.viewport.x = x;
        self.camera.viewport.y = y;
        self.camera.cursor_x = cursor_x - self.camera.viewport.x;
        self.camera.cursor_y = cursor_y - self.camera.viewport.y;
        if (self.camera.viewport.width == w and self.camera.viewport.height == h) {
            return;
        }

        self.camera.viewport.width = w;
        self.camera.viewport.height = h;
        self.render_texture = null;

        self.camera.projection_matrix = self.camera_projection.calc_matrix(
            self.camera.viewport,
        );
    }

    fn contains(self: @This(), x: f32, y: f32) bool {
        if (x < self.camera.viewport.x) {
            return false;
        }
        if (x > (self.camera.viewport.x + self.camera.viewport.width)) {
            return false;
        }
        if (y < self.camera.viewport.y) {
            return false;
        }
        if (y > (self.camera.viewport.y + self.camera.viewport.height)) {
            return false;
        }
        return true;
    }

    fn process_mouseevent(self: *@This(), wheel: f32, delta: c.Vector2) bool {
        self.camera_orbit.dolly(wheel);

        var active = wheel != 0;
        if (c.IsMouseButtonDown(c.MOUSE_BUTTON_RIGHT)) {
            // yaw pitch
            active = true;
            self.camera_orbit.yawDegree += @intFromFloat(delta.x);
            self.camera_orbit.pitchDegree += @intFromFloat(delta.y);
            if (self.camera_orbit.pitchDegree > 89) {
                self.camera_orbit.pitchDegree = 89;
            } else if (self.camera_orbit.pitchDegree < -89) {
                self.camera_orbit.pitchDegree = -89;
            }
        }

        if (c.IsMouseButtonDown(c.MOUSE_BUTTON_MIDDLE)) {
            // camera shift
            active = true;
            const speed = self.camera_orbit.distance * std.math.tan(self.camera_projection.fovy * 0.5) * 2.0 / self.camera.viewport.height;
            self.camera_orbit.shiftX += delta.x * speed;
            self.camera_orbit.shiftY -= delta.y * speed;
        }

        return active;
    }

    pub fn update_view_matrix(self: *@This()) void {
        self.camera.view_matrix, self.camera.transform_matrix = self.camera_orbit.calc_matrix();
    }

    pub fn get_or_create_render_texture(self: *@This()) c.RenderTexture2D {
        if (self.render_texture) |render_texture| {
            return render_texture;
        } else {
            const render_texture = c.LoadRenderTexture(
                @intFromFloat(self.camera.viewport.width),
                @intFromFloat(self.camera.viewport.height),
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
            @intFromFloat(self.camera.viewport.width),
            @intFromFloat(self.camera.viewport.height),
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
            c.rlMultMatrixf(&self.camera.projection_matrix.m00);
        }
        {
            c.rlMatrixMode(c.RL_MODELVIEW);
            c.rlLoadIdentity();
            c.rlMultMatrixf(&self.camera.view_matrix.m00);
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
                .width = self.camera.viewport.width,
                .height = -self.camera.viewport.height,
            },
            .{
                .x = self.camera.viewport.x,
                .y = self.camera.viewport.y,
            },
            c.WHITE,
        );
    }

    pub fn mouse_near_far(self: @This()) struct { zamath.Vec3, zamath.Vec3 } {
        const x = self.camera.transform_matrix.right();
        const y = self.camera.transform_matrix.up();
        const z = self.camera.transform_matrix.forward();
        const t = self.camera.transform_matrix.translation();
        const tan = std.math.tan(self.camera_projection.fovy / 2);
        // const near = self.projection.z_near;
        const far = self.camera_projection.z_far;
        const aspect = self.camera.viewport.width / self.camera.viewport.height;

        const half_width = self.camera.viewport.width / 2;
        const half_height = self.camera.viewport.height / 2;

        const mouse_h = (self.camera.cursor_x - half_width) / half_width;
        const mouse_v = -(self.camera.cursor_y - half_height) / half_height;
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
        try layout.rendertargets.append(.{});
        try layout.rendertargets.append(.{});

        for (layout.rendertargets.items) |*rendertarget| {
            rendertarget.update_view_matrix();
        }

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
            rendertarget.set_viewport_cursor(
                left,
                0,
                @floatFromInt(half_width),
                @floatFromInt(viewport_height),
                cursor.x,
                cursor.y,
            );
            left += @floatFromInt(half_width);
        }

        if (self.get_active_or_hover(
            cursor.x,
            cursor.y,
        )) |active| {
            if (active.process_mouseevent(wheel, cursor_delta)) {
                self.set_active(active);
                active.update_view_matrix();
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
