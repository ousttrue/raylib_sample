const c = @import("c.zig");
const std = @import("std");
const rdrag = @import("rdrag.zig");
// #pragma once
// #include "rdrag.h"
// #include <raylib.h>
// #include <raymath.h>
// #include <rcamera.h>

pub fn dolly(camera: *c.Camera3D) void {
    const wheel = c.GetMouseWheelMoveV();
    if (wheel.y > 0) {
        const distance = c.Vector3Distance(camera.target, camera.position);
        c.CameraMoveToTarget(camera, distance * 0.9 - distance);
    } else if (wheel.y < 0) {
        const distance = c.Vector3Distance(camera.target, camera.position);
        c.CameraMoveToTarget(camera, distance * 1.1 - distance);
    }
}

pub const OrbitCamera = struct {
    yawDegree: i32 = 0,
    pitchDegree: i32 = 40,
    shiftX: f32 = 0,
    shiftY: f32 = 0,

    pub fn yaw_pitch(self: *@This(), delta: c.Vector2) void {
        self.yawDegree -= @intFromFloat(delta.x);
        self.pitchDegree += @intFromFloat(delta.y);
        if (self.pitchDegree > 89) {
            self.pitchDegree = 89;
        } else if (self.pitchDegree < -89) {
            self.pitchDegree = -89;
        }
    }

    pub fn shift(self: *@This(), delta: c.Vector2, distance: f32, fovy: f32, rect: c.Rectangle) void {
        const speed = distance * std.math.tan(fovy * 0.5) * 2.0 / rect.height;
        self.shiftX += delta.x * speed;
        self.shiftY += delta.y * speed;
    }

    pub fn update_view(self: @This(), camera: *c.Camera3D) void {
        const distance = c.Vector3Distance(camera.target, camera.position);
        const pitch = c.MatrixRotateX(@as(f32, @floatFromInt(self.pitchDegree)) * c.DEG2RAD);
        const yaw = c.MatrixRotateY(@as(f32, @floatFromInt(self.yawDegree)) * c.DEG2RAD);
        const translation = c.MatrixTranslate(self.shiftX, self.shiftY, -distance);
        const camera_transform = c.MatrixMultiply(translation, c.MatrixMultiply(pitch, yaw));

        camera.position = .{
            .x = camera_transform.m12,
            .y = camera_transform.m13,
            .z = camera_transform.m14,
        };
        const forward = c.Vector3{
            .x = camera_transform.m8,
            .y = camera_transform.m9,
            .z = camera_transform.m10,
        };
        camera.target =
            c.Vector3Add(camera.position, c.Vector3Scale(forward, distance));
    }
};

pub const CameraYawPitchDragger = struct {
    _camera: *c.Camera,
    _orbit: *OrbitCamera,
    _last: c.Vector2 = .{ .x = 0, .y = 0 },

    pub fn begin(self: *@This(), cursor: c.Vector2) void {
        self._last = cursor;
    }

    pub fn end(_: *@This(), _: c.Vector2) void {}

    pub fn drag(self: *@This(), _: rdrag.DragState, w: i32, h: i32, cursor: c.Vector2) void {
        _ = h; // autofix
        _ = w; // autofix
        const delta = c.Vector2Subtract(cursor, self._last);
        self._last = cursor;
        // const distance = c.Vector3Distance(self._camera.target, self._camera.position);
        self._orbit.yaw_pitch(delta);
        self._orbit.update_view(self._camera);
    }
};

pub const CameraShiftDragger = struct {
    _camera: *c.Camera,
    _orbit: *OrbitCamera,
    _last: c.Vector2 = .{ .x = 0, .y = 0 },

    pub fn begin(self: *@This(), cursor: c.Vector2) void {
        self._last = cursor;
    }

    pub fn end(_: *@This(), _: c.Vector2) void {}

    pub fn drag(self: *@This(), _: rdrag.DragState, w: i32, h: i32, cursor: c.Vector2) void {
        const delta = c.Vector2Subtract(cursor, self._last);
        self._last = cursor;
        const distance = c.Vector3Distance(self._camera.target, self._camera.position);
        self._orbit.Shift(delta, distance, self._camera.fovy, .{
            0,
            0,
            w,
            h,
        });
        self._orbit.update_view(self._camera);
    }
};
