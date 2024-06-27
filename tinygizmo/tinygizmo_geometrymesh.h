#include "tinygizmo_alg.h"

namespace tinygizmo {

struct GeometryMesh {
  std::vector<Vertex> vertices;
  std::vector<UInt3> triangles;
  Float4 base_color;
  Float4 highlight_color;

  void compute_normals() {
    static const double NORMAL_EPSILON = 0.0001;

    std::vector<uint32_t> uniqueVertIndices(this->vertices.size(), 0);
    for (uint32_t i = 0; i < uniqueVertIndices.size(); ++i) {
      if (uniqueVertIndices[i] == 0) {
        uniqueVertIndices[i] = i + 1;
        auto v0 = this->vertices[i].position;
        for (auto j = i + 1; j < this->vertices.size(); ++j) {
          auto v1 = this->vertices[j].position;
          if ((v1 - v0).length2() < NORMAL_EPSILON) {
            uniqueVertIndices[j] = uniqueVertIndices[i];
          }
        }
      }
    }

    uint32_t idx0, idx1, idx2;
    for (auto &t : this->triangles) {
      idx0 = uniqueVertIndices[t.x] - 1;
      idx1 = uniqueVertIndices[t.y] - 1;
      idx2 = uniqueVertIndices[t.z] - 1;

      auto &v0 = this->vertices[idx0];
      auto &v1 = this->vertices[idx1];
      auto &v2 = this->vertices[idx2];
      auto n =
          Float3::cross(v1.position - v0.position, v2.position - v0.position);
      v0.normal = v0.normal + n;
      v1.normal = v1.normal + n;
      v2.normal = v2.normal + n;
    }

    for (uint32_t i = 0; i < this->vertices.size(); ++i) {
      this->vertices[i].normal =
          this->vertices[uniqueVertIndices[i] - 1].normal;
    }
    for (auto &v : this->vertices) {
      v.normal = v.normal.normalize();
    }
  }

  static GeometryMesh make_box_geometry(const Float3 &min_bounds,
                                        const Float3 &max_bounds,
                                        const Float4 &base_color,
                                        const Float4 &highlight_color) {
    const auto a = min_bounds, b = max_bounds;
    GeometryMesh mesh{
        .base_color = base_color,
        .highlight_color = highlight_color,
    };
    mesh.vertices = {
        {{a.x, a.y, a.z}, {-1, 0, 0}}, {{a.x, a.y, b.z}, {-1, 0, 0}},
        {{a.x, b.y, b.z}, {-1, 0, 0}}, {{a.x, b.y, a.z}, {-1, 0, 0}},
        {{b.x, a.y, a.z}, {+1, 0, 0}}, {{b.x, b.y, a.z}, {+1, 0, 0}},
        {{b.x, b.y, b.z}, {+1, 0, 0}}, {{b.x, a.y, b.z}, {+1, 0, 0}},
        {{a.x, a.y, a.z}, {0, -1, 0}}, {{b.x, a.y, a.z}, {0, -1, 0}},
        {{b.x, a.y, b.z}, {0, -1, 0}}, {{a.x, a.y, b.z}, {0, -1, 0}},
        {{a.x, b.y, a.z}, {0, +1, 0}}, {{a.x, b.y, b.z}, {0, +1, 0}},
        {{b.x, b.y, b.z}, {0, +1, 0}}, {{b.x, b.y, a.z}, {0, +1, 0}},
        {{a.x, a.y, a.z}, {0, 0, -1}}, {{a.x, b.y, a.z}, {0, 0, -1}},
        {{b.x, b.y, a.z}, {0, 0, -1}}, {{b.x, a.y, a.z}, {0, 0, -1}},
        {{a.x, a.y, b.z}, {0, 0, +1}}, {{b.x, a.y, b.z}, {0, 0, +1}},
        {{b.x, b.y, b.z}, {0, 0, +1}}, {{a.x, b.y, b.z}, {0, 0, +1}},
    };
    mesh.triangles = {{0, 1, 2},    {0, 2, 3},    {4, 5, 6},    {4, 6, 7},
                      {8, 9, 10},   {8, 10, 11},  {12, 13, 14}, {12, 14, 15},
                      {16, 17, 18}, {16, 18, 19}, {20, 21, 22}, {20, 22, 23}};
    return mesh;
  }

  static GeometryMesh make_cylinder_geometry(const Float3 &axis,
                                             const Float3 &arm1,
                                             const Float3 &arm2,
                                             uint32_t slices) {

    const float tau = 6.28318530718f;

    // Generated curved surface
    GeometryMesh mesh;
    for (uint32_t i = 0; i <= slices; ++i) {
      // auto tex_s = static_cast<float>(i) / slices;
      auto angle = (float)(i % slices) * tau / slices;
      auto arm = arm1.scale(std::cos(angle)) + arm2.scale(std::sin(angle));
      mesh.vertices.push_back({arm, arm.normalize()});
      mesh.vertices.push_back({arm + axis, arm.normalize()});
    }
    for (uint32_t i = 0; i < slices; ++i) {
      mesh.triangles.push_back({i * 2, i * 2 + 2, i * 2 + 3});
      mesh.triangles.push_back({i * 2, i * 2 + 3, i * 2 + 1});
    }

    // Generate caps
    uint32_t base = (uint32_t)mesh.vertices.size();
    for (uint32_t i = 0; i < slices; ++i) {
      const float angle = static_cast<float>(i % slices) * tau / slices,
                  c = std::cos(angle), s = std::sin(angle);
      auto arm = arm1.scale(c) + arm2.scale(s);
      mesh.vertices.push_back({arm + axis, axis.normalize()});
      mesh.vertices.push_back({arm, -axis.normalize()});
    }
    for (uint32_t i = 2; i < slices; ++i) {
      mesh.triangles.push_back({base, base + i * 2 - 2, base + i * 2});
      mesh.triangles.push_back({base + 1, base + i * 2 + 1, base + i * 2 - 1});
    }
    return mesh;
  }

  struct Float3x2 {
    float m00, m01, m02;
    float m10, m11, m12;

    Float3 apply(const Float2 &b) const {
      return Float3{m00, m01, m02}.scale(b.x) +
             Float3{m10, m11, m12}.scale(b.y);
    }
  };

  static GeometryMesh make_lathed_geometry(
      const Float3 &axis, const Float3 &arm1, const Float3 &arm2, int slices,
      const std::vector<Float2> &points, const Float4 &base_color,
      const Float4 &highlight_color, const float eps = 0.0f) {

    const float tau = 6.28318530718f;

    GeometryMesh mesh{
        .base_color = base_color,
        .highlight_color = highlight_color,
    };
    for (int i = 0; i <= slices; ++i) {
      const float angle = (static_cast<float>(i % slices) * tau / slices) +
                          (tau / 8.f),
                  c = std::cos(angle), s = std::sin(angle);
      auto row1 = arm1.scale(c) + arm2.scale(s);
      const Float3x2 mat = {
          axis.x, axis.y, axis.z, //
          row1.x, row1.y, row1.z, //
      };
      for (auto &p : points) {
        auto position = mat.apply(p) + Float3{eps, eps, eps};
        mesh.vertices.push_back({
            .position = position,
            .normal = Float3(0.f),
        });
      }

      if (i > 0) {
        for (uint32_t j = 1; j < (uint32_t)points.size(); ++j) {
          uint32_t i0 = (i - 1) * uint32_t(points.size()) + (j - 1);
          uint32_t i1 = (i - 0) * uint32_t(points.size()) + (j - 1);
          uint32_t i2 = (i - 0) * uint32_t(points.size()) + (j - 0);
          uint32_t i3 = (i - 1) * uint32_t(points.size()) + (j - 0);
          mesh.triangles.push_back({i0, i1, i2});
          mesh.triangles.push_back({i0, i2, i3});
        }
      }
    }
    mesh.compute_normals();
    return mesh;
  }

  static Float3 transform_coord(const Float4x4 &m, const Float3 &coord) {
    auto r = m.transform(Float4::make(coord, 1));
    return r.xyz().scale(1.0 / r.w);
  }

  void add_triangles(const AddTriangleFunc &add_triangle,
                     const Float4x4 &modelMatrix, const Float4 &color) const {
    for (auto &t : this->triangles) {
      auto v0 = this->vertices[t.x];
      auto v1 = this->vertices[t.y];
      auto v2 = this->vertices[t.z];
      auto p0 = transform_coord(
          modelMatrix,
          v0.position); // transform local coordinates into worldspace
      auto p1 = transform_coord(
          modelMatrix,
          v1.position); // transform local coordinates into worldspace
      auto p2 = transform_coord(
          modelMatrix,
          v2.position); // transform local coordinates into worldspace
      add_triangle({color.x, color.y, color.z, color.w}, {p0.x, p0.y, p0.z},
                   {p1.x, p1.y, p1.z}, {p2.x, p2.y, p2.z});
    }
  }

  float intersect(const Ray &ray) const {
    float best_t = std::numeric_limits<float>::infinity();
    std::optional<int32_t> best_tri = {};
    for (auto &tri : this->triangles) {
      float t;
      if (ray.intersect_triangle(this->vertices[tri.x].position,
                                 this->vertices[tri.y].position,
                                 this->vertices[tri.z].position, &t) &&
          t < best_t) {
        best_t = t;
        best_tri = uint32_t(&tri - this->triangles.data());
      }
    }
    return best_t;
  }
};

} // namespace tinygizmo
