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
        // fn TRS(t: c.Vector3, r: c.Quaternion, s: c.Vector3) c.Matrix {
        //     return c.MatrixMultiply(
        //         c.MatrixMultiply(
        //             c.MatrixScale(s.x, s.y, s.z),
        //             c.QuaternionToMatrix(r),
        //         ),
        //         c.MatrixTranslate(t.x, t.y, t.z),
        //     );
        // }
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

pub const Vertex = struct {
    position: Float3,
    normal: Float3,
    color: Float4,
};

pub const AddTriangleFunc = fn (
    user: *anyopaque,
    rgba: Float4,
    p0: Float3,
    p1: Float3,
    p2: Float3,
) void;

fn transform_coord(m: Float4x4, coord: Float3) Float3 {
    const r = m.transform(Float4.make(coord, 1));
    return r.xyz().scale(1.0 / r.w);
}

pub const GeometryMesh = struct {
    vertices: std.ArrayList(Vertex),
    triangles: std.ArrayList(UInt3),

    pub fn make_lathed_geometry(
        allocator: std.mem.Allocator,
        axis: Float3,
        arm1: Float3,
        arm2: Float3,
        slices: usize,
        points: []const Float2,
        eps: f32,
    ) !@This() {
        const tau = 6.28318530718;
        const tau_8 = tau / 8.0;
        const tau_slices = tau / @as(f32, @floatFromInt(slices));

        const Float3x2 = struct {
            m00: f32,
            m01: f32,
            m02: f32,
            m10: f32,
            m11: f32,
            m12: f32,
            fn apply(self: @This(), b: Float2) Float3 {
                const _a = Float3{ .x = self.m00, .y = self.m01, .z = self.m02 };
                const _b = Float3{ .x = self.m10, .y = self.m11, .z = self.m12 };
                return _a.scale(b.x).add(_b.scale(b.y));
            }
        };

        var mesh = GeometryMesh{
            .vertices = std.ArrayList(Vertex).init(allocator),
            .triangles = std.ArrayList(UInt3).init(allocator),
        };
        for (0..(slices + 1)) |i| {
            const angle = (@as(f32, @floatFromInt(@mod(i, slices))) * tau_slices) + tau_8;
            const c = std.math.cos(angle);
            const s = std.math.sin(angle);
            const row1 = arm1.scale(c).add(arm2.scale(s));
            const mat = Float3x2{
                .m00 = axis.x,
                .m01 = axis.y,
                .m02 = axis.z, //
                .m10 = row1.x,
                .m11 = row1.y,
                .m12 = row1.z, //
            };
            for (points) |p| {
                const position = mat.apply(p).add(.{ .x = eps, .y = eps, .z = eps });
                try mesh.vertices.append(.{
                    .position = position,
                    .normal = .{ .x = 0, .y = 0, .z = 0 },
                    .color = .{ .x = 0, .y = 0, .z = 0, .w = 0 },
                });
            }

            if (i > 0) {
                for (1..points.len) |j| {
                    const _i0 = (i - 1) * (points.len) + (j - 1);
                    const _i1 = (i - 0) * (points.len) + (j - 1);
                    const _i2 = (i - 0) * (points.len) + (j - 0);
                    const _i3 = (i - 1) * (points.len) + (j - 0);
                    try mesh.triangles.append(.{
                        .x = @intCast(_i0),
                        .y = @intCast(_i1),
                        .z = @intCast(_i2),
                    });
                    try mesh.triangles.append(.{
                        .x = @intCast(_i0),
                        .y = @intCast(_i2),
                        .z = @intCast(_i3),
                    });
                }
            }
        }
        // mesh.compute_normals();
        return mesh;
    }

    pub fn add_triangles(self: @This(), user: *anyopaque, add_triangle: AddTriangleFunc, modelMatrix: Float4x4, color: Float4) void {
        for (self.triangles.items) |t| {
            const v0 = self.vertices.items[t.x];
            const v1 = self.vertices.items[t.y];
            const v2 = self.vertices.items[t.z];
            const p0 = transform_coord(modelMatrix, v0.position); // transform local coordinates into worldspace
            const p1 = transform_coord(modelMatrix, v1.position); // transform local coordinates into worldspace
            const p2 = transform_coord(modelMatrix, v2.position); // transform local coordinates into worldspace
            add_triangle(user, color, p0, p1, p2);
        }
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
            return {};
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

    pub fn intersect_mesh(self: @This(), mesh: GeometryMesh) ?f32 {
        var best_t = std.math.inf(f32);
        var best_tri: ?usize = null;
        for (mesh.triangles.items, 0..) |tri, i| {
            if (self.intersect_triangle(
                mesh.vertices.items[tri.x].position,
                mesh.vertices.items[tri.y].position,
                mesh.vertices.items[tri.z].position,
            )) |t| {
                if (t < best_t) {
                    best_t = t;
                    best_tri = i;
                }
            }
        }
        if (best_tri) |_| {
            return best_t;
        } else {
            return null;
        }
    }
};

pub const FrameState = struct {
    mouse_down: bool = false,
    // If > 0.f, the gizmos are drawn scale-invariant with a screenspace value
    // defined here
    _screenspace_scale: f32 = 0,
    viewport_size: Float2 = .{ .x = 0, .y = 0 },
    ray: Ray = .{ .origin = .{ .x = 0, .y = 0, .z = 0 }, .direction = .{ .x = 0, .y = 0, .z = 0 } },
    cam_yfov: f32 = 0,
    cam_orientation: Quaternion = .{},

    // This will calculate a scale constant based on the number of screenspace
    // pixels passed as pixel_scale.
    pub fn calc_scale_screenspace(self: @This(), position: Float3) f32 {
        const dist = position.sub(self.ray.origin).length();
        return std.math.tan(self.cam_yfov) * dist *
            (self._screenspace_scale / self.viewport_size.y);
    }

    pub fn scale_screenspace(self: @This(), position: Float3) f32 {
        return if (self._screenspace_scale > 0.0) self.calc_scale_screenspace(position) else 1.0;
    }

    pub fn gizmo_transform_and_local_ray(
        self: @This(),
        local_toggle: bool,
        src: Transform,
    ) struct { f32, Transform, Ray } {
        const draw_scale = self.scale_screenspace(src.position);
        const p = Transform{
            .orientation = if (local_toggle) src.orientation else Quaternion{},
            .position = src.position,
        };
        var ray = self.ray.detransform(p);
        ray = ray.descale(draw_scale);
        return .{ draw_scale, p, ray };
    }
};

pub const DragState = struct {
    // Original position of an object being manipulated with a gizmo
    original_position: Float3 = .{ .x = 0, .y = 0, .z = 0 },
    // Offset from position of grabbed object to coordinates of clicked point
    click_offset: Float3 = .{ .x = 0, .y = 0, .z = 0 },
    // Original scale of an object being manipulated with a gizmo
    original_scale: Float3 = .{ .x = 0, .y = 0, .z = 0 },
    // Original orientation of an object being manipulated with a gizmo
    original_orientation: Quaternion = .{ .x = 0, .y = 0, .z = 0, .w = 1 },
};

// std::vector<Float2> arrow_points = {
//     {0.25f, 0}, {0.25f, 0.05f}, {1, 0.05f}, {1, 0.10f}, {1.2f, 0}};
//
// struct translation_plane_component : gizmo_component {
//   using gizmo_component::gizmo_component;
//
//   virtual Float3 get_axis(const FrameState &state, bool local_toggle,
//                           const Quaternion &rotation) const = 0;
//
//   std::optional<RigidTransform> drag(DragState *drag,
//                                      const FrameState &active_state,
//                                      bool local_toggle,
//                                      const RigidTransform &p) const override {
//     auto plane_normal = get_axis(active_state, local_toggle, p.orientation);
//     auto plane =
//         Plane::from_normal_and_position(plane_normal, drag->original_position);
//     auto t = active_state.ray.intersect_plane(plane);
//     if (!t) {
//       return {};
//     }
//     auto dst = active_state.ray.point(*t) - drag->click_offset;
//
//     return RigidTransform{
//         .orientation = p.orientation,
//         .position = dst,
//         .scale = p.scale,
//     };
//   }
// };
//
// struct translation_axis_component : gizmo_component {
//   using gizmo_component::gizmo_component;
//
//   virtual Float3 get_axis(const FrameState &state, bool local_toggle,
//                           const Quaternion &rotation) const = 0;
//
//   std::optional<RigidTransform> drag(DragState *drag,
//                                      const FrameState &active_state,
//                                      bool local_toggle,
//                                      const RigidTransform &p) const override {
//     // First apply a plane translation dragger with a plane that contains the
//     // desired axis and is oriented to face the camera
//     auto axis = get_axis(active_state, local_toggle, p.orientation);
//     auto plane_tangent =
//         Float3::cross(axis, p.position - active_state.ray.origin);
//     auto plane_normal = Float3::cross(axis, plane_tangent);
//     auto plane =
//         Plane::from_normal_and_position(plane_normal, drag->original_position);
//     auto t = active_state.ray.intersect_plane(plane);
//     if (!t) {
//       return {};
//     }
//     auto dst = active_state.ray.point(*t);
//
//     // Constrain object motion to be along the desired axis
//     auto point = drag->original_position +
//                  axis.scale(Float3::dot(dst - drag->original_position, axis)) -
//                  drag->click_offset;
//     return RigidTransform(p.orientation, point, p.scale);
//   }
// };
//
// struct translation_x_component : translation_axis_component {
//   translation_x_component()
//       : translation_axis_component(
//             GeometryMesh::make_lathed_geometry({1, 0, 0}, {0, 1, 0}, {0, 0, 1},
//                                                16, arrow_points),
//             Float4{1, 0.5f, 0.5f, 1.f}, Float4{1, 0, 0, 1.f}) {}
//
//   Float3 get_axis(const FrameState &state, bool local_toggle,
//                   const Quaternion &rotation) const override {
//     return (local_toggle) ? rotation.xdir() : Float3{1, 0, 0};
//   }
// };
//
// struct translation_y_component : translation_axis_component {
//   translation_y_component()
//       : translation_axis_component(
//             GeometryMesh::make_lathed_geometry({0, 1, 0}, {0, 0, 1}, {1, 0, 0},
//                                                16, arrow_points),
//             Float4{0.5f, 1, 0.5f, 1.f}, Float4{0, 1, 0, 1.f}) {}
//
//   Float3 get_axis(const FrameState &state, bool local_toggle,
//                   const Quaternion &rotation) const override {
//     return (local_toggle) ? rotation.ydir() : Float3{0, 1, 0};
//   }
// };
//
// struct translation_z_component : translation_axis_component {
//   translation_z_component()
//       : translation_axis_component(
//             GeometryMesh::make_lathed_geometry({0, 0, 1}, {1, 0, 0}, {0, 1, 0},
//                                                16, arrow_points),
//             Float4{0.5f, 0.5f, 1, 1.f}, Float4{0, 0, 1, 1.f}) {}
//
//   Float3 get_axis(const FrameState &state, bool local_toggle,
//                   const Quaternion &rotation) const override {
//     return (local_toggle) ? rotation.zdir() : Float3{0, 0, 1};
//   }
// };
//
// struct translation_yz_component : translation_plane_component {
//   translation_yz_component()
//       : translation_plane_component(
//             GeometryMesh::make_box_geometry({-0.01f, 0.25, 0.25},
//                                             {0.01f, 0.75f, 0.75f}),
//             Float4{0.5f, 1, 1, 0.5f}, Float4{0, 1, 1, 0.6f}) {}
//
//   Float3 get_axis(const FrameState &state, bool local_toggle,
//                   const Quaternion &rotation) const override {
//     return (local_toggle) ? rotation.xdir() : Float3{1, 0, 0};
//   }
// };
//
// struct translation_zx_component : translation_plane_component {
//   translation_zx_component()
//       : translation_plane_component(
//             GeometryMesh::make_box_geometry({0.25, -0.01f, 0.25},
//                                             {0.75f, 0.01f, 0.75f}),
//             Float4{1, 0.5f, 1, 0.5f}, Float4{1, 0, 1, 0.6f}) {}
//
//   Float3 get_axis(const FrameState &state, bool local_toggle,
//                   const Quaternion &rotation) const override {
//     return (local_toggle) ? rotation.ydir() : Float3{0, 1, 0};
//   }
// };
//
// struct translation_xy_component : translation_plane_component {
//   translation_xy_component()
//       : translation_plane_component(
//             GeometryMesh::make_box_geometry({0.25, 0.25, -0.01f},
//                                             {0.75f, 0.75f, 0.01f}),
//             Float4{1, 1, 0.5f, 0.5f}, Float4{1, 1, 0, 0.6f}) {}
//
//   Float3 get_axis(const FrameState &state, bool local_toggle,
//                   const Quaternion &rotation) const override {
//     return (local_toggle) ? rotation.zdir() : Float3{0, 0, 1};
//   }
// };
//
// struct translation_xyz_component : translation_plane_component {
//   translation_xyz_component()
//       : translation_plane_component(
//             GeometryMesh::make_box_geometry({-0.05f, -0.05f, -0.05f},
//                                             {0.05f, 0.05f, 0.05f}),
//             Float4{0.9f, 0.9f, 0.9f, 0.25f}, Float4{1, 1, 1, 0.35f}) {}
//
//   Float3 get_axis(const FrameState &state, bool local_toggle,
//                   const Quaternion &) const override {
//     return -state.cam_orientation.zdir();
//   }
// };

// auto _translate_x = std::make_shared<translation_x_component>();
// auto _translate_y = std::make_shared<translation_y_component>();
// auto _translate_z = std::make_shared<translation_z_component>();
// auto _translate_yz = std::make_shared<translation_yz_component>();
// auto _translate_zx = std::make_shared<translation_zx_component>();
// auto _translate_xy = std::make_shared<translation_xy_component>();
// auto _translate_xyz = std::make_shared<translation_xyz_component>();

pub const GizmoComponent = struct {
    mesh: GeometryMesh,
    base_color: Float4,
    highlight_color: Float4,

    pub fn Translation_X(allocator: std.mem.Allocator) !@This() {
        //   Float3 get_axis(const FrameState &state, bool local_toggle,
        //                   const Quaternion &rotation) const override {
        //     return (local_toggle) ? rotation.xdir() : Float3{1, 0, 0};
        //   }

        const arrow_points = [_]Float2{
            .{ .x = 0.25, .y = 0 },
            .{ .x = 0.25, .y = 0.05 },
            .{ .x = 1, .y = 0.05 },
            .{ .x = 1, .y = 0.10 },
            .{ .x = 1.2, .y = 0 },
        };

        return .{
            .mesh = try GeometryMesh.make_lathed_geometry(
                allocator,
                .{ .x = 1, .y = 0, .z = 0 },
                .{ .x = 0, .y = 1, .z = 0 },
                .{ .x = 0, .y = 0, .z = 1 },
                16,
                &arrow_points,
                0,
            ),
            .base_color = .{ .x = 1, .y = 0.5, .z = 0.5, .w = 1.0 },
            .highlight_color = .{ .x = 1, .y = 0, .z = 0, .w = 1.0 },
        };
    }
};

pub const TranslationGizmo = struct {

    // std::list<std::shared_ptr<Drawable>> _scene;
    // std::shared_ptr<Drawable> gizmo_target;
    active_component: ?*GizmoComponent = null,
    // tinygizmo::drag_state drag_state;

    pub fn begin(self: *@This(), cursor: Float2) void {
        _ = cursor; // autofix
        _ = self; // autofix
        // float best_t = std::numeric_limits<float>::infinity();
        // std::unordered_map<std::shared_ptr<Drawable>, RayState> ray_map;
        // for (auto &target : this._scene) {
        //   minalg::rigid_transform src{
        //       .orientation = *(minalg::float4 *)&target.rotation,
        //       .position = *(minalg::float3 *)&target.position,
        //       .scale = *(minalg::float3 *)&target.scale,
        //   };
        //   auto [draw_scale, gizmo_transform, local_ray] =
        //       tinygizmo::gizmo_transform(gizmo_state, local_toggle, src);
        //   // ray intersection
        //   auto [updated_state, t] = tinygizmo::position_intersect(local_ray);
        //   if (updated_state) {
        //     ray_map.insert({target,
        //                     {
        //                         .transform = src,
        //                         .draw_scale = draw_scale,
        //                         .gizmo_transform = gizmo_transform,
        //                         .local_ray = local_ray,
        //                         .t = t,
        //                     }});
        //     if (t < best_t) {
        //       this.active = updated_state;
        //       this.gizmo_target = target;
        //     }
        //   }
        // }
        //
        // if (this.active) {
        //   // begin drag
        //   auto ray_state = ray_map[this.gizmo_target];
        //   auto ray = tinygizmo::scaling(ray_state.draw_scale, ray_state.local_ray);
        //   this.drag_state = {.original_position = ray_state.transform.position,
        //                       .click_offset = ray.point(ray_state.t)};
        //   if (local_toggle) {
        //     // click point in gizmo local
        //     this.drag_state.click_offset =
        //         ray_state.gizmo_transform.transform_vector(
        //             this.drag_state.click_offset);
        //   }
        // }
    }

    pub fn drag(
        self: *@This(),
        state: DragState,
        w: i32,
        h: i32,
        cursor: Float2,
    ) void {
        _ = cursor; // autofix
        _ = h; // autofix
        _ = w; // autofix
        _ = w; // autofix
        _ = state; // autofix
        _ = self; // autofix
    }

    pub fn end(self: *@This(), _: Float2) void {
        _ = self; // autofix
        // self.active = .{};
        // self.gizmo_target = .{};
        // self.drag_state = .{};
    }

    //   virtual void draw(const tinygizmo::AddTriangleFunc &add_triangle,
    //                     const minalg::float4x4 &modelMatrix) = 0;
};

pub const System = struct {
    translations: std.ArrayList(GizmoComponent),

    local_toggle: bool = true,
    gizmo_state: FrameState = .{},

    pub fn init(allocator: std.mem.Allocator) !@This() {
        var system = System{
            .translations = std.ArrayList(GizmoComponent).init(allocator),
        };
        try system.translations.append(try GizmoComponent.Translation_X(allocator));
        return system;
    }
    pub fn deinit(self: *@This()) void {
        self.translations.deinit();
    }

    pub fn translation_draw(
        self: @This(),
        user: *anyopaque,
        add_triangle: AddTriangleFunc,
        active: ?*GizmoComponent,
        modelMatrix: Float4x4,
    ) void {
        for (self.translations.items) |c| {
            const color = if (&c == active) c.base_color else c.highlight_color;
            c.mesh.add_triangles(user, add_triangle, modelMatrix, color);
        }
    }

    pub fn translation_intersect(self: @This(), ray: Ray) struct { ?GizmoComponent, f32 } {
        var best_t = std.math.inf(f32);
        var updated_state: ?GizmoComponent = null;
        for (self.translations.items) |c| {
            if (ray.intersect_mesh(c.mesh)) |t| {
                if (t < best_t) {
                    updated_state = c;
                    best_t = t;
                }
            }
        }
        return .{ updated_state, best_t };
    }

    // Float3 position_drag(DragState *drag, const FrameState &state,
    //                      bool local_toggle,
    //                      const std::shared_ptr<gizmo_component> &active,
    //                      const RigidTransform &p) {
    //
    //   if (auto dst = active->drag(drag, state, local_toggle, p)) {
    //     return dst->position;
    //   } else {
    //     return p.position;
    //   }
    // }

    pub fn rotation_intersect(self: @This(), ray: Ray) struct { ?GizmoComponent, f32 } {
        _ = ray; // autofix
        _ = self; // autofix
        return .{ null, 0 };
    }

    pub fn scaling_intersect(self: @This(), ray: Ray) struct { ?GizmoComponent, f32 } {
        _ = ray; // autofix
        _ = self; // autofix
        return .{ null, 0 };
    }
};

pub const RayState = struct {
    transform: Transform,
    draw_scale: f32,
    gizmo_transform: Transform,
    local_ray: Ray,
    t: f32,
};

pub const GizmoModeType = enum {
    Translation,
    Rotation,
    Scaling,
};

pub const GizmoMode = union(GizmoModeType) {
    Translation: struct {
        pub fn begin_gizmo(
            _: @This(),
            ray_state: RayState,
            local_toggle: bool,
        ) DragState {
            const ray = ray_state.local_ray.scaling(ray_state.draw_scale);
            var drag_state = DragState{
                .click_offset = ray.point(ray_state.t),
                .original_position = ray_state.transform.position,
            };
            if (local_toggle) {
                drag_state.click_offset =
                    ray_state.gizmo_transform.transform_vector(drag_state.click_offset);
            }
            return drag_state;
        }
    },

    Rotation: struct {
        pub fn begin_gizmo(
            _: @This(),
            ray_state: RayState,
            _: bool,
        ) DragState {
            const ray = ray_state.local_ray.scaling(ray_state.draw_scale);
            const drag_state = DragState{
                .click_offset = ray_state.transform.transform_point(ray.origin.add(ray.direction.scale(ray_state.t))),
                .original_orientation = ray_state.transform.orientation,
            };
            return drag_state;
        }
    },

    Scaling: struct {
        uniform: bool = false,
        pub fn begin_gizmo(
            _: @This(),
            ray_state: RayState,
            _: bool,
        ) DragState {
            const ray = ray_state.local_ray.scaling(ray_state.draw_scale);
            const drag_state = DragState{
                .click_offset = ray_state.transform.transform_point(ray.origin.add(ray.direction.scale(ray_state.t))),
                .original_scale = ray_state.transform.scale,
            };
            return drag_state;
        }
    },
};
