const std = @import("std");
const alg = @import("tinygizmo_alg.zig");

pub const Vertex = struct {
    position: alg.Float3,
    normal: alg.Float3,
    // color: Float4,
};

pub const AddTriangleFunc = fn (
    user: *anyopaque,
    rgba: alg.Float4,
    p0: alg.Float3,
    p1: alg.Float3,
    p2: alg.Float3,
) void;

fn transform_coord(m: alg.Float4x4, coord: alg.Float3) alg.Float3 {
    const r = m.transform(alg.Float4.make(coord, 1));
    return r.xyz().scale(1.0 / r.w);
}

fn make_mesh(comptime verts: anytype, comptime tris: anytype) type {
    return struct {
        const vertices = verts;
        const triangles = tris;
    };
}

pub fn make_box_geometry(comptime a: alg.Float3, comptime b: alg.Float3) type {
    return struct {
        const vertices = [_]Vertex{
            .{ .position = .{ .x = a.x, .y = a.y, .z = a.z }, .normal = .{ .x = -1, .y = 0, .z = 0 } },
            .{ .position = .{ .x = a.x, .y = a.y, .z = b.z }, .normal = .{ .x = -1, .y = 0, .z = 0 } },
            .{ .position = .{ .x = a.x, .y = b.y, .z = b.z }, .normal = .{ .x = -1, .y = 0, .z = 0 } },
            .{ .position = .{ .x = a.x, .y = b.y, .z = a.z }, .normal = .{ .x = -1, .y = 0, .z = 0 } },
            .{ .position = .{ .x = b.x, .y = a.y, .z = a.z }, .normal = .{ .x = 1, .y = 0, .z = 0 } },
            .{ .position = .{ .x = b.x, .y = b.y, .z = a.z }, .normal = .{ .x = 1, .y = 0, .z = 0 } },
            .{ .position = .{ .x = b.x, .y = b.y, .z = b.z }, .normal = .{ .x = 1, .y = 0, .z = 0 } },
            .{ .position = .{ .x = b.x, .y = a.y, .z = b.z }, .normal = .{ .x = 1, .y = 0, .z = 0 } },
            .{ .position = .{ .x = a.x, .y = a.y, .z = a.z }, .normal = .{ .x = 0, .y = -1, .z = 0 } },
            .{ .position = .{ .x = b.x, .y = a.y, .z = a.z }, .normal = .{ .x = 0, .y = -1, .z = 0 } },
            .{ .position = .{ .x = b.x, .y = a.y, .z = b.z }, .normal = .{ .x = 0, .y = -1, .z = 0 } },
            .{ .position = .{ .x = a.x, .y = a.y, .z = b.z }, .normal = .{ .x = 0, .y = -1, .z = 0 } },
            .{ .position = .{ .x = a.x, .y = b.y, .z = a.z }, .normal = .{ .x = 0, .y = 1, .z = 0 } },
            .{ .position = .{ .x = a.x, .y = b.y, .z = b.z }, .normal = .{ .x = 0, .y = 1, .z = 0 } },
            .{ .position = .{ .x = b.x, .y = b.y, .z = b.z }, .normal = .{ .x = 0, .y = 1, .z = 0 } },
            .{ .position = .{ .x = b.x, .y = b.y, .z = a.z }, .normal = .{ .x = 0, .y = 1, .z = 0 } },
            .{ .position = .{ .x = a.x, .y = a.y, .z = a.z }, .normal = .{ .x = 0, .y = 0, .z = -1 } },
            .{ .position = .{ .x = a.x, .y = b.y, .z = a.z }, .normal = .{ .x = 0, .y = 0, .z = -1 } },
            .{ .position = .{ .x = b.x, .y = b.y, .z = a.z }, .normal = .{ .x = 0, .y = 0, .z = -1 } },
            .{ .position = .{ .x = b.x, .y = a.y, .z = a.z }, .normal = .{ .x = 0, .y = 0, .z = -1 } },
            .{ .position = .{ .x = a.x, .y = a.y, .z = b.z }, .normal = .{ .x = 0, .y = 0, .z = 1 } },
            .{ .position = .{ .x = b.x, .y = a.y, .z = b.z }, .normal = .{ .x = 0, .y = 0, .z = 1 } },
            .{ .position = .{ .x = b.x, .y = b.y, .z = b.z }, .normal = .{ .x = 0, .y = 0, .z = 1 } },
            .{ .position = .{ .x = a.x, .y = b.y, .z = b.z }, .normal = .{ .x = 0, .y = 0, .z = 1 } },
        };
        const triangles = [_]alg.UInt3{
            .{ .x = 0, .y = 1, .z = 2 },
            .{ .x = 0, .y = 2, .z = 3 },
            .{ .x = 4, .y = 5, .z = 6 },
            .{ .x = 4, .y = 6, .z = 7 },
            .{ .x = 8, .y = 9, .z = 10 },
            .{ .x = 8, .y = 10, .z = 11 },
            .{ .x = 12, .y = 13, .z = 14 },
            .{ .x = 12, .y = 14, .z = 15 },
            .{ .x = 16, .y = 17, .z = 18 },
            .{ .x = 16, .y = 18, .z = 19 },
            .{ .x = 20, .y = 21, .z = 22 },
            .{ .x = 20, .y = 22, .z = 23 },
        };
    };
}

fn make_lathed_geometry(
    comptime axis: alg.Float3,
    comptime arm1: alg.Float3,
    comptime arm2: alg.Float3,
    comptime slices: usize,
    comptime points: []const alg.Float2,
    comptime eps: f32,
) type {
    @setEvalBranchQuota(5000);

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
        fn apply(self: @This(), b: alg.Float2) alg.Float3 {
            const _a = alg.Float3{ .x = self.m00, .y = self.m01, .z = self.m02 };
            const _b = alg.Float3{ .x = self.m10, .y = self.m11, .z = self.m12 };
            return _a.scale(b.x).add(_b.scale(b.y));
        }
    };

    var vertices: [points.len * (slices + 1)]Vertex = undefined;
    var triangles: [(points.len - 1) * slices * 2]alg.UInt3 = undefined;
    var k: usize = 0;
    var l: usize = 0;
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
            vertices[k] = .{
                .position = position,
                .normal = .{ .x = 0, .y = 0, .z = 0 },
                // .color = .{ .x = 0, .y = 0, .z = 0, .w = 0 },
            };
            k += 1;
        }

        if (i > 0) {
            for (1..points.len) |j| {
                const _i0 = (i - 1) * (points.len) + (j - 1);
                const _i1 = (i - 0) * (points.len) + (j - 1);
                const _i2 = (i - 0) * (points.len) + (j - 0);
                const _i3 = (i - 1) * (points.len) + (j - 0);
                triangles[l] = .{
                    .x = @intCast(_i0),
                    .y = @intCast(_i1),
                    .z = @intCast(_i2),
                };
                l += 1;
                triangles[l] = .{
                    .x = @intCast(_i0),
                    .y = @intCast(_i2),
                    .z = @intCast(_i3),
                };
                l += 1;
            }
        }
    }

    return make_mesh(vertices, triangles);
}

pub const GeometryMesh = struct {
    vertices: []const Vertex,
    triangles: []const alg.UInt3,
    base_color: alg.Float4,
    highlight_color: alg.Float4,

    pub fn make_box(
        comptime min: alg.Float3,
        comptime max: alg.Float3,
        base_color: alg.Float4,
        highlight_color: alg.Float4,
    ) @This() {
        const mesh = make_box_geometry(min, max);
        return .{
            .vertices = &mesh.vertices,
            .triangles = &mesh.triangles,
            .base_color = base_color,
            .highlight_color = highlight_color,
        };
    }

    pub fn make_lathed(
        axis: alg.Float3,
        arm1: alg.Float3,
        arm2: alg.Float3,
        comptime slices: usize,
        comptime points: []const alg.Float2,
        base_color: alg.Float4,
        highlight_color: alg.Float4,
        eps: f32,
    ) @This() {
        const mesh = make_lathed_geometry(axis, arm1, arm2, slices, points, eps);
        return .{
            .vertices = &mesh.vertices,
            .triangles = &mesh.triangles,
            .base_color = base_color,
            .highlight_color = highlight_color,
        };
    }

    pub fn add_triangles(
        self: @This(),
        user: *anyopaque,
        add_triangle: AddTriangleFunc,
        modelMatrix: alg.Float4x4,
        color: alg.Float4,
    ) void {
        for (self.triangles) |t| {
            const v0 = self.vertices[t.x];
            const v1 = self.vertices[t.y];
            const v2 = self.vertices[t.z];
            const p0 = transform_coord(modelMatrix, v0.position); // transform local coordinates into worldspace
            const p1 = transform_coord(modelMatrix, v1.position); // transform local coordinates into worldspace
            const p2 = transform_coord(modelMatrix, v2.position); // transform local coordinates into worldspace
            add_triangle(user, color, p0, p1, p2);
        }
    }

    pub fn intersect(self: @This(), ray: alg.Ray) f32 {
        var best_t = std.math.inf(f32);
        var best_tri: ?usize = null;
        for (self.triangles, 0..) |tri, i| {
            if (ray.intersect_triangle(self.vertices[tri.x].position, self.vertices[tri.y].position, self.vertices[tri.z].position)) |t| {
                if (t < best_t) {
                    best_t = t;
                    best_tri = i;
                }
            }
        }
        return best_t;
    }
};
