const minalg = @import("minalg.zig");

pub const RigidTransform = struct {
    orientation: minalg.Quaternion = .{},
    position: minalg.Float3 = .{ .x = 0, .y = 0, .z = 0 },
    scale: minalg.Float3 = .{ .x = 1, .y = 1, .z = 1 },

    pub fn uniform_scale(self: @This()) bool {
        return self.scale.x == self.scale.y and self.scale.x == self.scale.z;
    }

    pub fn matrix(self: @This()) minalg.Float4x4 {
        return .{
            .row0 = minalg.Float4.from_Float3(
                minalg.qxdir(self.orientation) * self.scale.x,
                0,
            ),
            .row1 = minalg.Float4.from_Float3(
                minalg.qydir(self.orientation) * self.scale.y,
                0,
            ),
            .row2 = minalg.Float4.from_Float3(
                minalg.qzdir(self.orientation) * self.scale.z,
                0,
            ),
            .row3 = minalg.Float4.from_Float3(
                self.position,
                1,
            ),
        };
    }

    // minalg.float3 transform_vector(const minalg.float3 &vec) const {
    //   return qrot(orientation, vec * scale);
    // }
    // minalg.float3 transform_point(const minalg.float3 &p) const {
    //   return position + transform_vector(p);
    // }

    pub fn detransform_point(self: @This(), p: minalg.Float3) minalg.Float3 {
        return self.detransform_vector(p.subtract(self.position));
    }

    // pub fn detransform_vector(self: @This(), vec: minalg.Float3) minalg.Float3 {
    //     return self.orientation.inverse().rotate(vec).divide(self.scale);
    // }
};
