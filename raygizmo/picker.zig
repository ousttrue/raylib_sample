const std = @import("std");
const c = @import("c.zig");
const PICKING_FBO_WIDTH = 512;
const PICKING_FBO_HEIGHT = 512;

pub const Picker = struct {
    PICKING_FBO: c_uint = 0,
    PICKING_TEXTURE: c_uint = 0,

    pub fn load() @This() {
        // Load picking fbo
        const PICKING_FBO = c.rlLoadFramebuffer(
            PICKING_FBO_WIDTH,
            PICKING_FBO_HEIGHT,
        );
        if (PICKING_FBO == 0) {
            c.TraceLog(c.LOG_ERROR, "RAYGIZMO: Failed to create picking fbo");
            std.c.exit(1);
        }
        c.rlEnableFramebuffer(PICKING_FBO);
        const PICKING_TEXTURE = c.rlLoadTexture(null, PICKING_FBO_WIDTH, PICKING_FBO_HEIGHT, c.RL_PIXELFORMAT_UNCOMPRESSED_R8G8B8A8, 1);
        c.rlActiveDrawBuffers(1);
        c.rlFramebufferAttach(PICKING_FBO, PICKING_TEXTURE, c.RL_ATTACHMENT_COLOR_CHANNEL0, c.RL_ATTACHMENT_TEXTURE2D, 0);
        if (!c.rlFramebufferComplete(PICKING_FBO)) {
            c.TraceLog(c.LOG_ERROR, "RAYGIZMO: Picking fbo is not complete");
            std.c.exit(1);
        }

        return .{
            .PICKING_FBO = PICKING_FBO,
            .PICKING_TEXTURE = PICKING_TEXTURE,
        };
    }

    pub fn unload(self: @This()) void {
        c.rlUnloadFramebuffer(self.PICKING_FBO);
        c.rlUnloadTexture(self.PICKING_TEXTURE);
    }

    pub fn begin(self: @This()) void {
        c.rlEnableFramebuffer(self.PICKING_FBO);
        c.rlViewport(0, 0, PICKING_FBO_WIDTH, PICKING_FBO_HEIGHT);
        c.rlClearColor(0, 0, 0, 0);
        c.rlClearScreenBuffers();
        c.rlDisableColorBlend();
    }

    pub fn end(self: @This()) ?u8 {
        c.rlDisableFramebuffer();
        c.rlEnableColorBlend();
        c.rlViewport(0, 0, c.GetScreenWidth(), c.GetScreenHeight());

        // -------------------------------------------------------------------
        // Pick the pixel under the mouse cursor
        const mouse_position = c.GetMousePosition();
        const pixels = c.rlReadTexturePixels(
            self.PICKING_TEXTURE,
            PICKING_FBO_WIDTH,
            PICKING_FBO_HEIGHT,
            c.RL_PIXELFORMAT_UNCOMPRESSED_R8G8B8A8,
        );

        const x_fract = c.Clamp(mouse_position.x / @as(f32, @floatFromInt(c.GetScreenWidth())), 0.0, 1.0);
        const y_fract = c.Clamp(1.0 - (mouse_position.y / @as(f32, @floatFromInt(c.GetScreenHeight()))), 0.0, 1.0);
        const x: usize = @as(usize, @intFromFloat(PICKING_FBO_WIDTH * x_fract));
        const y: usize = @as(usize, @intFromFloat(PICKING_FBO_HEIGHT * y_fract));
        const idx: usize = 4 * (y * PICKING_FBO_WIDTH + x);
        var picked_id: ?u8 = null;
        if (pixels) |p| {
            picked_id = @as([*c]u8, @ptrCast(p))[idx];
        }

        std.c.free(pixels);

        return picked_id;
    }
};
