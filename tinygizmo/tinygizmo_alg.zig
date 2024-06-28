const std = @import("std");

pub const Float2 = struct {
    x: f32,
    y: f32,

    pub fn add(self: @This(), rhs: @This()) @This() {
        return .{ .x = self.x + rhs.x, .y = self.y + rhs.y };
    }
};

pub const Float3 = struct {
    x: f32,
    y: f32,
    z: f32,

    pub const x_axis = Float3{ .x = 1, .y = 0, .z = 0 };
    pub const y_axis = Float3{ .x = 0, .y = 1, .z = 0 };
    pub const z_axis = Float3{ .x = 0, .y = 0, .z = 1 };

    pub fn dot(lhs: @This(), rhs: @This()) f32 {
        return lhs.x * rhs.x + lhs.y * rhs.y + lhs.z * rhs.z;
    }

    pub fn length2(self: @This()) f32 {
        return dot(self, self);
    }

    pub fn length(self: @This()) f32 {
        return std.math.sqrt(self.length2());
    }

    pub fn scale(self: @This(), f: f32) @This() {
        return .{
            .x = self.x * f,
            .y = self.y * f,
            .z = self.z * f,
        };
    }

    pub fn normalize(self: @This()) @This() {
        return self.scale(1.0 / self.length());
    }

    pub fn cross(a: @This(), b: @This()) @This() {
        return .{
            .x = a.y * b.z - a.z * b.y,
            .y = a.z * b.x - a.x * b.z,
            .z = a.x * b.y - a.y * b.x,
        };
    }

    pub fn add(self: @This(), rhs: @This()) @This() {
        return .{
            .x = self.x + rhs.x,
            .y = self.y + rhs.y,
            .z = self.z + rhs.z,
        };
    }

    pub fn negate(self: @This()) @This() {
        return .{
            .x = -self.x,
            .y = -self.y,
            .z = -self.z,
        };
    }

    pub fn sub(self: @This(), rhs: @This()) @This() {
        return .{
            .x = self.x - rhs.x,
            .y = self.y - rhs.y,
            .z = self.z - rhs.z,
        };
    }

    pub fn mult_each(self: @This(), rhs: @This()) @This() {
        return .{
            .x = self.x * rhs.x,
            .y = self.y * rhs.y,
            .z = self.z * rhs.z,
        };
    }

    pub fn div_each(self: @This(), rhs: @This()) @This() {
        return .{
            .x = self.x / rhs.x,
            .y = self.y / rhs.y,
            .z = self.z / rhs.z,
        };
    }
};

pub const UInt3 = struct {
    x: u32,
    y: u32,
    z: u32,
};

pub const Float4 = struct {
    x: f32,
    y: f32,
    z: f32,
    w: f32,

    pub fn dot(lhs: @This(), rhs: @This()) f32 {
        return lhs.x * rhs.x + lhs.y * rhs.y + lhs.z * rhs.z + lhs.w * rhs.w;
    }

    pub fn length2(self: @This()) f32 {
        return dot(self, self);
    }

    pub fn length(self: @This()) f32 {
        return std.math.sqrt(self.length2());
    }

    pub fn xyz(self: @This()) Float3 {
        return .{ .x = self.x, .y = self.y, .z = self.z };
    }

    pub fn make(v: Float3, w: f32) @This() {
        return .{
            .x = v.x,
            .y = v.y,
            .z = v.z,
            .w = w,
        };
    }
};

pub const Float4x4 = struct {
    m00: f32,
    m01: f32,
    m02: f32,
    m03: f32,
    m10: f32,
    m11: f32,
    m12: f32,
    m13: f32,
    m20: f32,
    m21: f32,
    m22: f32,
    m23: f32,
    m30: f32,
    m31: f32,
    m32: f32,
    m33: f32,

    pub fn row0(self: @This()) Float4 {
        return .{ .x = self.m00, .y = self.m01, .z = self.m02, .w = self.m03 };
    }
    pub fn row1(self: @This()) Float4 {
        return .{ .x = self.m10, .y = self.m11, .z = self.m12, .w = self.m13 };
    }
    pub fn row2(self: @This()) Float4 {
        return .{ .x = self.m20, .y = self.m21, .z = self.m22, .w = self.m23 };
    }
    pub fn row3(self: @This()) Float4 {
        return .{ .x = self.m30, .y = self.m31, .z = self.m32, .w = self.m33 };
    }

    pub fn col0(self: @This()) Float4 {
        return .{ .x = self.m00, .y = self.m10, .z = self.m20, .w = self.m30 };
    }
    pub fn col1(self: @This()) Float4 {
        return .{ .x = self.m01, .y = self.m11, .z = self.m21, .w = self.m31 };
    }
    pub fn col2(self: @This()) Float4 {
        return .{ .x = self.m02, .y = self.m12, .z = self.m22, .w = self.m32 };
    }
    pub fn col3(self: @This()) Float4 {
        return .{ .x = self.m03, .y = self.m13, .z = self.m23, .w = self.m33 };
    }

    pub fn make(r0: Float4, r1: Float4, r2: Float4, r3: Float4) @This() {
        return .{
            .m00 = r0.x,
            .m01 = r0.y,
            .m02 = r0.z,
            .m03 = r0.w, //
            .m10 = r1.x,
            .m11 = r1.y,
            .m12 = r1.z,
            .m13 = r1.w, //
            .m20 = r2.x,
            .m21 = r2.y,
            .m22 = r2.z,
            .m23 = r2.w, //
            .m30 = r3.x,
            .m31 = r3.y,
            .m32 = r3.z,
            .m33 = r3.w, //
        };
    }

    pub fn transpose(self: @This()) @This() {
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

    pub fn scaling(x: f32, y: f32, z: f32) @This() {
        return .{
            .m00 = x,
            .m01 = 0,
            .m02 = 0,
            .m03 = 0, //
            .m10 = 0,
            .m11 = y,
            .m12 = 0,
            .m13 = 0, //
            .m20 = 0,
            .m21 = 0,
            .m22 = z,
            .m23 = 0, //
            .m30 = 0,
            .m31 = 0,
            .m32 = 0,
            .m33 = 1, //
        };
    }

    pub fn mul(self: @This(), rhs: @This()) @This() {
        return .{
            .m00 = Float4.dot(self.row0(), rhs.col0()),
            .m01 = Float4.dot(self.row0(), rhs.col1()),
            .m02 = Float4.dot(self.row0(), rhs.col2()),
            .m03 = Float4.dot(self.row0(), rhs.col3()), //
            .m10 = Float4.dot(self.row1(), rhs.col0()),
            .m11 = Float4.dot(self.row1(), rhs.col1()),
            .m12 = Float4.dot(self.row1(), rhs.col2()),
            .m13 = Float4.dot(self.row1(), rhs.col3()), //
            .m20 = Float4.dot(self.row2(), rhs.col0()),
            .m21 = Float4.dot(self.row2(), rhs.col1()),
            .m22 = Float4.dot(self.row2(), rhs.col2()),
            .m23 = Float4.dot(self.row2(), rhs.col3()), //
            .m30 = Float4.dot(self.row3(), rhs.col0()),
            .m31 = Float4.dot(self.row3(), rhs.col1()),
            .m32 = Float4.dot(self.row3(), rhs.col2()),
            .m33 = Float4.dot(self.row3(), rhs.col3()), //
        };
    }

    pub fn transform(self: @This(), rhs: Float4) Float4 {
        return .{
            .x = Float4.dot(rhs, self.col0()),
            .y = Float4.dot(rhs, self.col1()),
            .z = Float4.dot(rhs, self.col2()),
            .w = Float4.dot(rhs, self.col3()),
        };
    }
};

pub const Quaternion = struct {
    x: f32 = 0,
    y: f32 = 0,
    z: f32 = 0,
    w: f32 = 1,

    pub const identity = Quaternion{ .x = 0, .y = 0, .z = 0, .w = 1 };

    pub fn dot(a: @This(), b: @This()) f32 {
        return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
    }

    pub fn length2(self: @This()) f32 {
        return dot(self, self);
    }

    pub fn xdir(self: @This()) Float3 {
        return .{
            .x = self.w * self.w + self.x * self.x - self.y * self.y - self.z * self.z,
            .y = (self.x * self.y + self.z * self.w) * 2,
            .z = (self.z * self.x - self.y * self.w) * 2,
        };
    }
    pub fn ydir(self: @This()) Float3 {
        return .{
            .x = (self.x * self.y - self.z * self.w) * 2,
            .y = self.w * self.w - self.x * self.x + self.y * self.y - self.z * self.z,
            .z = (self.y * self.z + self.x * self.w) * 2,
        };
    }
    pub fn zdir(self: @This()) Float3 {
        return .{
            .x = (self.z * self.x + self.y * self.w) * 2,
            .y = (self.y * self.z - self.x * self.w) * 2,
            .z = self.w * self.w - self.x * self.x - self.y * self.y + self.z * self.z,
        };
    }

    pub fn from_axis_angle(axis: Float3, angle: f32) Quaternion {
        const v = axis.scale(std.math.sin(angle / 2));
        return .{
            .x = v.x,
            .y = v.y,
            .z = v.z,
            .w = std.math.cos(angle / 2),
        };
    }

    pub fn mul(self: @This(), b: @This()) @This() {
        return .{
            .x = self.x * b.w + self.w * b.x + self.y * b.z - self.z * b.y,
            .y = self.y * b.w + self.w * b.y + self.z * b.x - self.x * b.z,
            .z = self.z * b.w + self.w * b.z + self.x * b.y - self.y * b.x,
            .w = self.w * b.w - self.x * b.x - self.y * b.y - self.z * b.z,
        };
    }

    pub fn scale(self: @This(), f: f32) @This() {
        return .{
            .x = self.x * f,
            .y = self.y * f,
            .z = self.z * f,
            .w = self.w * f,
        };
    }

    pub fn conjugage(self: @This()) @This() {
        return .{
            .x = -self.x,
            .y = -self.y,
            .z = -self.z,
            .w = self.w,
        };
    }

    pub fn inverse(self: @This()) @This() {
        return self.conjugage().scale(1.0 / self.length2());
    }

    pub fn rotate(self: @This(), v: Float3) Float3 {
        return self.xdir().scale(v.x).add(self.ydir().scale(v.y)).add(self.zdir().scale(v.z));
    }
};

pub const Transform = struct {
    orientation: Quaternion = .{ .x = 0, .y = 0, .z = 0, .w = 1 },
    position: Float3 = .{ .x = 0, .y = 0, .z = 0 },
    scale: Float3 = .{ .x = 1, .y = 1, .z = 1 },

    pub fn uniform_scale(self: @This()) bool {
        return self.scale.x == self.scale.y and self.scale.x == self.scale.z;
    }
    pub fn matrix(self: @This()) Float4x4 {
        return Float4x4.make(
            Float4.make(self.orientation.xdir().scale(self.scale.x), 0),
            Float4.make(self.orientation.ydir().scale(self.scale.y), 0),
            Float4.make(self.orientation.zdir().scale(self.scale.z), 0),
            Float4.make(self.position, 1),
        );
    }
    pub fn transform_vector(self: @This(), vec: Float3) Float3 {
        return self.orientation.rotate(vec.mult_each(self.scale));
    }
    pub fn detransform_vector(self: @This(), vec: Float3) Float3 {
        return self.orientation.inverse().rotate(vec).div_each(self.scale);
    }
    pub fn transform_point(self: @This(), p: Float3) Float3 {
        return self.position.add(self.transform_vector(p));
    }
    pub fn detransform_point(self: @This(), p: Float3) Float3 {
        return self.detransform_vector(p.sub(self.position));
    }
};

pub const Plane = struct {
    normal: Float3,
    d: f32,

    pub fn from_normal_and_position(n: Float3, point_on_plane: Float3) Plane {
        const normal = n.normalize();
        return .{
            .normal = normal,
            .d = -Float3.dot(normal, point_on_plane),
        };
    }
};

pub const Ray = struct {
    origin: Float3,
    direction: Float3,

    pub fn scaling(self: @This(), scale: f32) @This() {
        return .{
            .origin = self.origin.scale(scale),
            .direction = self.direction.scale(scale),
        };
    }

    pub fn descale(self: @This(), scale: f32) @This() {
        return .{
            .origin = self.origin.scale(1.0 / scale),
            .direction = self.direction.scale(1.0 / scale),
        };
    }

    pub fn transform(self: @This(), p: Transform) @This() {
        return .{
            .origin = p.transform_point(self.origin),
            .direction = p.transform_vector(self.direction),
        };
    }

    pub fn detransform(self: @This(), p: Transform) @This() {
        return .{
            .origin = p.detransform_point(self.origin),
            .direction = p.detransform_vector(self.direction),
        };
    }

    pub fn point(self: @This(), t: f32) Float3 {
        return self.origin.add(self.direction.scale(t));
    }

    pub fn intersect_plane(self: @This(), plane: Plane) ?f32 {
        const denom = Float3.dot(plane.normal, self.direction);
        if (@abs(denom) == 0) {
            // not intersect
            return null;
        }
        return -(Float3.dot(plane.normal, self.origin) + plane.d) / denom;
    }

    pub fn intersect_triangle(self: @This(), v0: Float3, v1: Float3, v2: Float3) ?f32 {
        const e1 = v1.sub(v0);
        const e2 = v2.sub(v0);
        const h = Float3.cross(self.direction, e2);
        const a = Float3.dot(e1, h);
        if (@abs(a) == 0) {
            return null;
        }

        const f = 1 / a;
        const s = self.origin.sub(v0);
        const u = f * Float3.dot(s, h);
        if (u < 0 or u > 1) {
            return null;
        }

        const q = Float3.cross(s, e1);
        const v = f * Float3.dot(self.direction, q);
        if (v < 0 or u + v > 1) {
            return null;
        }

        const t = f * Float3.dot(e2, q);
        if (t < 0) {
            return null;
        }

        return t;
    }
};
