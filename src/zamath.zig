// zig algebra math
const std = @import("std");
const DEG2RAD = std.math.pi / 180.0;

pub const Vec3 = struct {
    x: f32 = 0,
    y: f32 = 0,
    z: f32 = 0,

    pub fn negate(self: @This()) @This() {
        return .{
            .x = -self.x,
            .y = -self.y,
            .z = -self.z,
        };
    }

    pub fn dot(self: @This(), rhs: @This()) f32 {
        return self.x * rhs.x + self.y * rhs.y + self.z * rhs.z;
    }

    pub fn length(self: @This()) f32 {
        return std.math.sqrt(self.dot(self));
    }

    pub fn scale(self: @This(), scaling: f32) Vec3 {
        return .{
            .x = self.x * scaling,
            .y = self.y * scaling,
            .z = self.z * scaling,
        };
    }

    pub fn add(self: @This(), rhs: @This()) Vec3 {
        return .{
            .x = self.x + rhs.x,
            .y = self.y + rhs.y,
            .z = self.z + rhs.z,
        };
    }

    pub fn subtract(self: @This(), rhs: @This()) Vec3 {
        return .{
            .x = self.x - rhs.x,
            .y = self.y - rhs.y,
            .z = self.z - rhs.z,
        };
    }
};

pub const Vec4 = struct {
    x: f32 = 0,
    y: f32 = 0,
    z: f32 = 0,
    w: f32 = 0,
};

pub const Mat3 = struct {
    m00: f32 = 1,
    m01: f32 = 0,
    m02: f32 = 0,
    m10: f32 = 0,
    m11: f32 = 1,
    m12: f32 = 0,
    m20: f32 = 0,
    m21: f32 = 0,
    m22: f32 = 1,

    pub fn multiply(self: @This(), rhs: @This()) Mat3 {
        return .{
            // 0
            .m00 = self.m00 * rhs.m00 + self.m01 * rhs.m10 + self.m02 * rhs.m20,
            .m01 = self.m00 * rhs.m01 + self.m01 * rhs.m11 + self.m02 * rhs.m21,
            .m02 = self.m00 * rhs.m02 + self.m01 * rhs.m12 + self.m02 * rhs.m22,
            // 1
            .m10 = self.m10 * rhs.m00 + self.m11 * rhs.m10 + self.m12 * rhs.m20,
            .m11 = self.m10 * rhs.m01 + self.m11 * rhs.m11 + self.m12 * rhs.m21,
            .m12 = self.m10 * rhs.m02 + self.m11 * rhs.m12 + self.m12 * rhs.m22,
            // 2
            .m20 = self.m20 * rhs.m00 + self.m21 * rhs.m10 + self.m22 * rhs.m20,
            .m21 = self.m20 * rhs.m01 + self.m21 * rhs.m11 + self.m22 * rhs.m21,
            .m22 = self.m20 * rhs.m02 + self.m21 * rhs.m12 + self.m22 * rhs.m22,
        };
    }

    pub fn make_rotate_x(rad: f32) Mat3 {
        const s = std.math.sin(rad);
        const c = std.math.cos(rad);
        // 1
        //   c s
        //  -s c
        return .{
            .m00 = 1,
            .m01 = 0,
            .m02 = 0,
            .m10 = 0,
            .m11 = c,
            .m12 = s,
            .m20 = 0,
            .m21 = -s,
            .m22 = c,
        };
    }

    pub fn make_rotate_y(rad: f32) Mat3 {
        const s = std.math.sin(rad);
        const c = std.math.cos(rad);
        // c  -s
        //   1
        // s   c
        return .{
            .m00 = c,
            .m01 = 0,
            .m02 = -s,
            .m10 = 0,
            .m11 = 1,
            .m12 = 0,
            .m20 = s,
            .m21 = 0,
            .m22 = c,
        };
    }
};

pub const Mat4 = struct {
    m00: f32 = 1,
    m01: f32 = 0,
    m02: f32 = 0,
    m03: f32 = 0,
    m10: f32 = 0,
    m11: f32 = 1,
    m12: f32 = 0,
    m13: f32 = 0,
    m20: f32 = 0,
    m21: f32 = 0,
    m22: f32 = 1,
    m23: f32 = 0,
    m30: f32 = 0,
    m31: f32 = 0,
    m32: f32 = 0,
    m33: f32 = 1,

    pub fn row0(self: @This()) Vec4 {
        return .{ .x = self.m00, .y = self.m01, .z = self.m02, .w = self.m03 };
    }
    pub fn row1(self: @This()) Vec4 {
        return .{ .x = self.m10, .y = self.m11, .z = self.m12, .w = self.m13 };
    }
    pub fn row2(self: @This()) Vec4 {
        return .{ .x = self.m20, .y = self.m21, .z = self.m22, .w = self.m23 };
    }
    pub fn row3(self: @This()) Vec4 {
        return .{ .x = self.m30, .y = self.m31, .z = self.m32, .w = self.m03 };
    }

    pub fn right(self: @This()) Vec3 {
        return .{ .x = self.m00, .y = self.m01, .z = self.m02 };
    }
    pub fn up(self: @This()) Vec3 {
        return .{ .x = self.m10, .y = self.m11, .z = self.m12 };
    }
    pub fn forward(self: @This()) Vec3 {
        return .{ .x = -self.m20, .y = -self.m21, .z = -self.m22 };
    }
    pub fn translation(self: @This()) Vec3 {
        return .{ .x = self.m30, .y = self.m31, .z = self.m32 };
    }

    pub fn transpose(self: @This()) Mat4 {
        return .{
            .m00 = self.m00,
            .m01 = self.m10,
            .m02 = self.m20,
            .m03 = self.m30,
            .m10 = self.m01,
            .m11 = self.m11,
            .m12 = self.m21,
            .m13 = self.m31,
            .m20 = self.m02,
            .m21 = self.m12,
            .m22 = self.m22,
            .m23 = self.m32,
            .m30 = self.m03,
            .m31 = self.m13,
            .m32 = self.m23,
            .m33 = self.m33,
        };
    }

    pub fn multiply(self: @This(), rhs: @This()) Mat4 {
        return .{
            // 0
            .m00 = self.m00 * rhs.m00 + self.m01 * rhs.m10 + self.m02 * rhs.m20 + self.m03 * rhs.m30,
            .m01 = self.m00 * rhs.m01 + self.m01 * rhs.m11 + self.m02 * rhs.m21 + self.m03 * rhs.m31,
            .m02 = self.m00 * rhs.m02 + self.m01 * rhs.m12 + self.m02 * rhs.m22 + self.m03 * rhs.m32,
            .m03 = self.m00 * rhs.m03 + self.m01 * rhs.m13 + self.m02 * rhs.m23 + self.m03 * rhs.m33,
            // 1
            .m10 = self.m10 * rhs.m00 + self.m11 * rhs.m10 + self.m12 * rhs.m20 + self.m13 * rhs.m30,
            .m11 = self.m10 * rhs.m01 + self.m11 * rhs.m11 + self.m12 * rhs.m21 + self.m13 * rhs.m31,
            .m12 = self.m10 * rhs.m02 + self.m11 * rhs.m12 + self.m12 * rhs.m22 + self.m13 * rhs.m32,
            .m13 = self.m10 * rhs.m03 + self.m11 * rhs.m13 + self.m12 * rhs.m23 + self.m13 * rhs.m33,
            // 2
            .m20 = self.m20 * rhs.m00 + self.m21 * rhs.m10 + self.m22 * rhs.m20 + self.m23 * rhs.m30,
            .m21 = self.m20 * rhs.m01 + self.m21 * rhs.m11 + self.m22 * rhs.m21 + self.m23 * rhs.m31,
            .m22 = self.m20 * rhs.m02 + self.m21 * rhs.m12 + self.m22 * rhs.m22 + self.m23 * rhs.m32,
            .m23 = self.m20 * rhs.m03 + self.m21 * rhs.m13 + self.m22 * rhs.m23 + self.m23 * rhs.m33,
            // 3
            .m30 = self.m30 * rhs.m00 + self.m31 * rhs.m10 + self.m32 * rhs.m20 + self.m33 * rhs.m30,
            .m31 = self.m30 * rhs.m01 + self.m31 * rhs.m11 + self.m32 * rhs.m21 + self.m33 * rhs.m31,
            .m32 = self.m30 * rhs.m02 + self.m31 * rhs.m12 + self.m32 * rhs.m22 + self.m33 * rhs.m32,
            .m33 = self.m30 * rhs.m03 + self.m31 * rhs.m13 + self.m32 * rhs.m23 + self.m33 * rhs.m33,
        };
    }

    pub fn make_translate(x: f32, y: f32, z: f32) Mat4 {
        return .{
            .m00 = 1,
            .m01 = 0,
            .m02 = 0,
            .m03 = 0,
            .m10 = 0,
            .m11 = 1,
            .m12 = 0,
            .m13 = 0,
            .m20 = 0,
            .m21 = 0,
            .m22 = 1,
            .m23 = 0,
            .m30 = x,
            .m31 = y,
            .m32 = z,
            .m33 = 1,
        };
    }

    pub fn from_mat3(m: Mat3) Mat4 {
        return .{
            .m00 = m.m00,
            .m01 = m.m01,
            .m02 = m.m02,
            .m03 = 0,
            .m10 = m.m10,
            .m11 = m.m11,
            .m12 = m.m12,
            .m13 = 0,
            .m20 = m.m20,
            .m21 = m.m21,
            .m22 = m.m22,
            .m23 = 0,
            .m30 = 0,
            .m31 = 0,
            .m32 = 0,
            .m33 = 1,
        };
    }
};

pub const RotationTag = enum {
    Mat3,
};

pub const Rotation = union(RotationTag) {
    Mat3: Mat3,
};

pub const EuclideanTransform = struct {
    position: Vec3 = .{ .x = 0, .y = 0, .z = -5 },
    rotation: Mat3 = .{},

    pub fn forward(self: *@This()) Vec3 {
        return .{
            .x = self.rotation.m20,
            .y = self.rotation.m21,
            .z = self.rotation.m22,
        };
    }
};

pub const Rect = struct {
    x: f32,
    y: f32,
    width: f32,
    height: f32,
};

pub const CameraProjection = struct {
    fovy: f32 = 60.0 * (std.math.pi / 180.0),
    z_near: f32 = 1.5,
    z_far: f32 = 30.0,

    // gluPerspective
    pub fn calc_matrix(self: *@This(), viewport: Rect) Mat4 {
        const f = 1 / std.math.tan(self.fovy / 2);
        return .{
            .m00 = f / viewport.width * viewport.height,
            .m01 = 0,
            .m02 = 0,
            .m03 = 0,

            .m10 = 0,
            .m11 = f,
            .m12 = 0,
            .m13 = 0,

            .m20 = 0,
            .m21 = 0,
            .m22 = (self.z_near + self.z_far) / (self.z_near - self.z_far),
            .m23 = -1,

            .m30 = 0,
            .m31 = 0,
            .m32 = 2 * self.z_near * self.z_far / (self.z_near - self.z_far),
            .m33 = 0,
        };
    }
};

pub const CameraOrbit = struct {
    yawDegree: i32 = 0,
    pitchDegree: i32 = 40,
    shiftX: f32 = 0,
    shiftY: f32 = 0,
    distance: f32 = 10,

    pub fn dolly(self: *@This(), wheel: f32) void {
        if (wheel > 0) {
            self.distance *= 0.9;
        } else if (wheel < 0) {
            self.distance *= 1.1;
        }
    }

    pub fn yawpitch(self: *@This(), cursor_dx: f32, cursor_dy: f32) void {
        self.yawDegree += @intFromFloat(cursor_dx);
        self.pitchDegree += @intFromFloat(cursor_dy);
        if (self.pitchDegree > 89) {
            self.pitchDegree = 89;
        } else if (self.pitchDegree < -89) {
            self.pitchDegree = -89;
        }
    }

    pub fn shift(
        self: *@This(),
        cursor_dx: f32,
        cursor_dy: f32,
        viewport_height: f32,
        fovy: f32,
    ) void {
        const speed = self.distance * std.math.tan(fovy * 0.5) * 2.0 / viewport_height;
        self.shiftX += cursor_dx * speed;
        self.shiftY -= cursor_dy * speed;
    }

    pub fn calc_matrix(self: *@This()) struct { Mat4, Mat4 } {
        // const distance = c.Vector3Distance(camera.target, camera.position);
        const pitch = Mat3.make_rotate_x(
            @as(f32, @floatFromInt(self.pitchDegree)) * DEG2RAD,
        );
        const yaw = Mat3.make_rotate_y(
            @as(f32, @floatFromInt(self.yawDegree)) * DEG2RAD,
        );
        const rot = Mat4.from_mat3(yaw.multiply(pitch));
        const translation = Mat4.make_translate(
            self.shiftX,
            self.shiftY,
            -self.distance,
        );
        const view_matrix = rot.multiply(translation);

        const transform_matrix =
            Mat4.make_translate(
            -self.shiftX,
            -self.shiftY,
            self.distance,
        ).multiply(rot.transpose());

        return .{ view_matrix, transform_matrix };
    }

    pub fn right(self: @This()) Vec3 {
        return .{
            .x = self.transform_matrix.m00,
            .y = self.transform_matrix.m01,
            .z = self.transform_matrix.m02,
        };
    }
    pub fn up(self: @This()) Vec3 {
        return .{
            .x = self.transform_matrix.m10,
            .y = self.transform_matrix.m11,
            .z = self.transform_matrix.m12,
        };
    }
    pub fn forward(self: @This()) Vec3 {
        return .{
            .x = -self.transform_matrix.m20,
            .y = -self.transform_matrix.m21,
            .z = -self.transform_matrix.m22,
        };
    }
    pub fn position(self: @This()) Vec3 {
        return .{
            .x = self.transform_matrix.m30,
            .y = self.transform_matrix.m31,
            .z = self.transform_matrix.m32,
        };
    }
};

fn get_point(x: Vec3, y: Vec3, z: Vec3, pos: Vec3, vertical: f32, horizontal: f32, distance: f32) Vec3 {
    return pos.add(z.scale(distance).add(x.scale(distance * horizontal)).add(y.scale(distance * vertical)));
}

pub const Frustum = struct {
    near_left_top: Vec3,
    near_left_bottom: Vec3,
    near_right_bottom: Vec3,
    near_right_top: Vec3,
    far_left_top: Vec3,
    far_left_bottom: Vec3,
    far_right_bottom: Vec3,
    far_right_top: Vec3,

    pub fn make(
        transform: Mat4,
        fovy: f32,
        aspect: f32,
        z_near: f32,
        z_far: f32,
    ) @This() {
        const x = transform.right();
        const y = transform.up();
        const z = transform.forward();
        const t = transform.translation();
        const tan = std.math.tan(fovy / 2);
        return .{
            .near_left_top = get_point(x.negate(), y, z, t, tan, tan * aspect, z_near),
            .near_left_bottom = get_point(x.negate(), y.negate(), z, t, tan, tan * aspect, z_near),
            .near_right_bottom = get_point(x, y.negate(), z, t, tan, tan * aspect, z_near),
            .near_right_top = get_point(x, y, z, t, tan, tan * aspect, z_near),
            .far_left_top = get_point(x.negate(), y, z, t, tan, tan * aspect, z_far),
            .far_left_bottom = get_point(x.negate(), y.negate(), z, t, tan, tan * aspect, z_far),
            .far_right_bottom = get_point(x, y.negate(), z, t, tan, tan * aspect, z_far),
            .far_right_top = get_point(x, y, z, t, tan, tan * aspect, z_far),
        };
    }
};

pub const Camera = struct {
    // viewport / mouse
    viewport: Rect = .{ .x = 0, .y = 0, .width = 1, .height = 1 },
    cursor_x: f32 = 0,
    cursor_y: f32 = 0,
    // view
    view_matrix: Mat4 = .{},
    transform_matrix: Mat4 = .{},
    // projection
    projection_matrix: Mat4 = .{},

    pub fn aspect(self: @This()) f32 {
        return self.viewport.width / self.viewport.height;
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
        return true;
    }
};
