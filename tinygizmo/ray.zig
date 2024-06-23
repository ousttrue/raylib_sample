const minalg = @import("minalg.zig");

pub const Plane = struct {
    normal: minalg.float3,
    d: f32,

    pub fn from_normal_and_point(n: minalg.Float3, point_on_plane: minalg.Float3) @This() {
        const normal = n.normalize(n);
        return .{
            .normal = normal,
            .d = -minalg.dot(normal, point_on_plane),
        };
    }
};

pub const Ray = struct {
    origin: minalg.Float3,
    direction: minalg.Float3,

    //   minalg::float3 point(float t) const { return origin + direction * t; }
    //
    //   std::optional<float> intersect_plane(const plane &plane) const {
    //     float denom = dot(plane.normal, this->direction);
    //     if (std::abs(denom) == 0) {
    //       // not intersect
    //       return {};
    //     }
    //     return -(dot(plane.normal, this->origin) + plane.d) / denom;
    //   }
    //
    //   bool intersect_triangle(const minalg::float3 &v0, const minalg::float3 &v1,
    //                           const minalg::float3 &v2, float *hit_t) const {
    //     auto e1 = v1 - v0, e2 = v2 - v0, h = cross(this->direction, e2);
    //     auto a = dot(e1, h);
    //     if (std::abs(a) == 0)
    //       return false;
    //
    //     float f = 1 / a;
    //     auto s = this->origin - v0;
    //     auto u = f * dot(s, h);
    //     if (u < 0 || u > 1)
    //       return false;
    //
    //     auto q = cross(s, e1);
    //     auto v = f * dot(this->direction, q);
    //     if (v < 0 || u + v > 1)
    //       return false;
    //
    //     auto t = f * dot(e2, q);
    //     if (t < 0)
    //       return false;
    //
    //     if (hit_t)
    //       *hit_t = t;
    //     return true;
    //   }
    //
    //   bool intersect_mesh(const geometry_mesh &mesh, float *hit_t) const {
    //     float best_t = std::numeric_limits<float>::infinity(), t;
    //     int32_t best_tri = -1;
    //     for (auto &tri : mesh.triangles) {
    //       if (this->intersect_triangle(mesh.vertices[tri[0]].position,
    //                                    mesh.vertices[tri[1]].position,
    //                                    mesh.vertices[tri[2]].position, &t) &&
    //           t < best_t) {
    //         best_t = t;
    //         best_tri = uint32_t(&tri - mesh.triangles.data());
    //       }
    //     }
    //     if (best_tri == -1)
    //       return false;
    //     if (hit_t)
    //       *hit_t = best_t;
    //     return true;
    //   }
};
