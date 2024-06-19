// zig algebra math
const std = @import("std");
const DEG2RAD = std.math.pi / 180.0;

pub const Vec3 = struct {
    x: f32,
    y: f32,
    z: f32,

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
    matrix: Mat4 = .{},

    // gluPerspective
    fovy: f32 = 60.0 * (std.math.pi / 180.0),
    z_near: f32 = 0.001,
    z_far: f32 = 100.0,

    pub fn update_matrix(self: *@This(), viewport: Rect) void {
        const f = 1 / std.math.tan(self.fovy / 2);
        // const aspect = @as(f32, @intFromFloat(self.view_width)) / @as(f32, @intFromFloat(self.view_height));
        self.matrix = .{
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
    // right handed
    // right: (1, 0, 0)
    // up: (0, 1, 0)
    // forward:  (0, 0, -1)
    view: EuclideanTransform = .{},
    view_matrix: Mat4 = .{},

    yawDegree: i32 = 0,
    pitchDegree: i32 = 40,
    shiftX: f32 = 0,
    shiftY: f32 = -5,
    distance: f32 = 5,

    pub fn dolly(self: *@This(), wheel: f32) void {
        if (wheel > 0) {
            self.distance *= 0.9;
            self.view.position = self.view.position.add(self.view.forward().scale(self.distance));
        } else if (wheel < 0) {
            self.distance *= 1.1;
            self.view.position = self.view.position.add(self.view.forward().scale(self.distance));
        }
    }

    pub fn update_matrix(self: *@This()) void {
        // const distance = c.Vector3Distance(camera.target, camera.position);
        const pitch = Mat3.make_rotate_x(
            @as(f32, @floatFromInt(self.pitchDegree)) * DEG2RAD,
        );
        const yaw = Mat3.make_rotate_y(
            @as(f32, @floatFromInt(self.yawDegree)) * DEG2RAD,
        );
        const translation = Mat4.make_translate(
            self.shiftX,
            self.shiftY,
            -self.distance,
        );
        // self.view_matrix =
        //     translation.multiply(
        //     Mat4.from_mat3(pitch.multiply(yaw)),
        // );
        self.view_matrix =
            Mat4.from_mat3(yaw.multiply(pitch)).multiply(translation);
    }
};
