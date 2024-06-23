const std = @import("std");

pub const Float3 = struct {
    x: f32 = 0,
    y: f32 = 0,
    z: f32 = 0,

    pub fn subtract(self: @This(), rhs: @This()) @This() {
        return .{
            .x = self.x - rhs.x,
            .y = self.y - rhs.y,
            .z = self.z - rhs.z,
        };
    }

    pub fn scale(self: @This(), s: f32) @This() {
        return .{
            .x = self.x * s,
            .y = self.y * s,
            .z = self.z * s,
        };
    }

    pub fn dot(lhs: @This(), rhs: @This()) f32 {
        return lhs.x * lhs.x + lhs.y * rhs.y + lhs.z * rhs.z;
    }

    pub fn length(self: @This()) f32 {
        return std.math.sqrt(dot(self, self));
    }

    pub fn normalize(self: @This()) f32 {
        return std.math.sqrt(dot(self, self));
    }
};

pub const Quaternion = struct {
    x: f32 = 0,
    y: f32 = 0,
    z: f32 = 0,
    w: f32 = 1.0,

    pub fn dot(self: @This(), rhs: @This()) f32 {
        return self.x * rhs.x + self.y * rhs.y + self.z * rhs.z + self.w * rhs.w;
    }

    pub fn scale(self: @This(), s: f32) @This() {
        return .{
            self.x * s,
            self.y * s,
            self.z * s,
            self.w * s,
        };
    }

    // pub fn xdir(self: @This()) Float3 {
    // }
    // pub fn ydir(self: @This()) Float3 {
    // }
    // pub fn zdir(self: @This()) Float3 {
    // }

    // pub fn rotate(self: @This(), v: Float3) Float3 {
    //     return self.xdir() * v.x + self.ydir() * v.y + self.zdir() * v.z;
    // }

    pub fn conjugate(self: @This()) @This() {
        return .{
            .x = -self.x,
            .y = -self.y,
            .z = -self.z,
            .w = self.w,
        };
    }

    pub fn inverse(self: @This()) @This() {
        // return qconj(q) / length2(q);
        return self.conjugate().scale(1.0 / dot(self, self));
    }
};
