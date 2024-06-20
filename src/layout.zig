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
    render_texture: ?c.RenderTexture2D = null,

    fn get_or_create_render_texture(self: *@This(), width: i32, height: i32) c.RenderTexture2D {
        if (self.render_texture) |render_texture| {
            if (render_texture.texture.width == width and render_texture.texture.height == height) {
                return render_texture;
            }
            c.UnloadRenderTexture(render_texture);
            self.render_texture = null;
        }

        const render_texture = c.LoadRenderTexture(
            width,
            height,
        );
        self.render_texture = render_texture;
        return render_texture;
    }

    pub fn begin(self: *@This(), width: i32, height: i32) ?c.Texture2D {
        if (width == 0 or height == 0) {
            return null;
        }

        const render_texture = self.get_or_create_render_texture(width, height);

        c.BeginTextureMode(render_texture);
        c.rlViewport(
            0,
            0,
            width,
            height,
        );
        c.ClearBackground(c.SKYBLUE);

        return render_texture.texture;
    }

    pub fn end(_: @This()) void {
        c.EndTextureMode();
        // c.DrawTextureRec(
        //     texture,
        //     .{
        //         .width = self.camera.viewport.width,
        //         .height = -self.camera.viewport.height,
        //     },
        //     .{
        //         .x = self.camera.viewport.x,
        //         .y = self.camera.viewport.y,
        //     },
        //     c.WHITE,
        // );
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
