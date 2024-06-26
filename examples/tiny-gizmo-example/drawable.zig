const std = @import("std");
const c = @import("c.zig");
const tinygizmo = @import("tinygizmo");

pub const Vertex = struct {
    position: c.Vector3,
    color: c.Vector3,
};

pub const Drawable = struct {
    name: []const u8,
    model: ?c.Model = null,
    transform: tinygizmo.Transform = .{
        .position = .{ .x = 0, .y = 0, .z = 0 },
        .orientation = .{ .x = 0, .y = 0, .z = 0, .w = 1 },
        .scale = .{ .x = 1, .y = 1, .z = 1 },
    },

    pub fn load_pointer(
        self: *@This(),
        vertexCount: usize,
        vertices: *const c.Vector3,
        colors: *const c.Color,
        indexCount: usize,
        indices: *const c_ushort,
        dynamic: bool,
    ) void {
        var mesh = c.Mesh{
            .vertexCount = @intCast(vertexCount),
            .triangleCount = @intCast(indexCount / 3),
            .vertices = @ptrCast(@constCast(vertices)),
            .colors = @ptrCast(@constCast(colors)),
            .indices = @ptrCast(@constCast(indices)),
        };
        c.UploadMesh(&mesh, dynamic);
        self.model = c.LoadModelFromMesh(mesh);
    }

    pub fn load_slice(
        self: *@This(),
        vertices: []const c.Vector3,
        colors: []const c.Color,
        indices: []const c_ushort,
        dynamic: bool,
    ) void {
        self.load_pointer(vertices.len, &vertices[0], &colors[0], indices.len, &indices[0], dynamic);
    }

    // Generate a simple triangle mesh from code
    pub fn load_floats(
        self: *@This(),
        vertices: []const f32,
        indices: []const c_uint,
        dynamic: bool,
    ) !void {
        const allocator = std.heap.page_allocator;
        const vertex_count = @divTrunc(vertices.len, 6);
        // vertices
        const mesh_vertices = try allocator.alloc(c.Vector3, vertex_count);
        // mesh.normals = (float *)MemAlloc(mesh.vertexCount * 3 * sizeof(float));
        const mesh_colors = try allocator.alloc(c.Color, vertex_count);

        for (mesh_vertices, 0..) |*v, i| {
            v.x = vertices[i * 6];
            v.y = vertices[i * 6 + 1];
            v.z = vertices[i * 6 + 2];
        }

        for (mesh_colors, 0..) |*v, i| {
            v.r = @intFromFloat(@max(0.0, vertices[i * 6 + 3]) * 255);
            v.g = @intFromFloat(@max(0.0, vertices[i * 6 + 4]) * 255);
            v.b = @intFromFloat(@max(0.0, vertices[i * 6 + 5]) * 255);
            v.a = 255;
        }

        const mesh_indices = try allocator.alloc(c_ushort, indices.len);
        for (indices, 0..) |value, i| {
            mesh_indices[i] = @intCast(value);
        }

        self.load_pointer(
            vertex_count,
            &mesh_vertices[0],
            &mesh_colors[0],
            indices.len,
            &mesh_indices[0],
            dynamic,
        );
    }

    pub fn draw(self: @This()) void {
        c.rlPushMatrix();
        const m = self.transform.matrix();
        c.rlMultMatrixf(&m.m00);
        if (self.model) |model| {
            c.DrawModel(model, .{ .x = 0, .y = 0, .z = 0 }, 1.0, c.WHITE);
        }
        c.rlPopMatrix();
    }
};
