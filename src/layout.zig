pub const c = @cImport({
    @cInclude("raylib.h");
    @cInclude("rlgl.h");
    // @cInclude("rcamera.h");
    // @cInclude("raymath.h");
    @cDefine("CIMGUI_DEFINE_ENUMS_AND_STRUCTS", "");
    @cDefine("IMGUI_DISABLE_OBSOLETE_FUNCTIONS", "1");
    @cInclude("cimgui.h");
    @cInclude("rlImGui.h");
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

    pub fn update_projection_matrix(self: *@This()) void {
        self.camera.projection_matrix = self.camera_projection.calc_matrix(
            self.camera.viewport,
        );
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
            if (rendertarget.camera.set_viewport_cursor(
                left,
                0,
                @floatFromInt(half_width),
                @floatFromInt(viewport_height),
                cursor.x,
                cursor.y,
            )) {
                rendertarget.render_texture = null;
                rendertarget.update_projection_matrix();
            }

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
