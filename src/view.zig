const std = @import("std");
const zamath = @import("zamath.zig");
const c = @import("c.zig");

pub const View = struct {
    camera: zamath.Camera = .{},
    camera_projection: zamath.CameraProjection = .{},
    camera_orbit: zamath.CameraOrbit = .{},

    pub fn ray(self: @This()) zamath.Ray {
        return .{
            .origin = self.camera.transform_matrix.translation(),
            .dir = self.camera.transform_matrix.forward(),
        };
    }

    pub fn frustum(self: @This()) zamath.Frustum {
        return zamath.Frustum.make(
            self.camera.transform_matrix,
            self.camera_projection.fovy,
            self.camera.aspect(),
            self.camera_projection.z_near,
            self.camera_projection.z_far,
        );
    }

    pub fn update_projection_matrix(self: *@This()) void {
        self.camera.projection_matrix = self.camera_projection.calc_matrix(self.camera.viewport);
    }

    pub fn update_view_matrix(self: *@This()) void {
        self.camera.view_matrix, self.camera.transform_matrix = self.camera_orbit.calc_matrix();
    }

    pub fn update(
        self: *@This(),
        cursor_delta_x: f32,
        cursor_delta_y: f32,
        h: f32,
    ) void {
        var active = false;
        if (c.IsMouseButtonDown(c.MOUSE_BUTTON_RIGHT)) {
            self.camera_orbit.yawpitch(cursor_delta_x, cursor_delta_y);
            active = true;
        }
        if (c.IsMouseButtonDown(c.MOUSE_BUTTON_MIDDLE)) {
            self.camera_orbit.shift(
                cursor_delta_x,
                cursor_delta_y,
                h,
                self.camera_projection.fovy,
            );
            active = true;
        }
        if (active) {
            self.update_view_matrix();
        }
    }

    pub fn begin_camera3D(self: *const @This()) void {
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
