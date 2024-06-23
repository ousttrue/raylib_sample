#pragma once
#include <cmath>
#include <optional>
#include <stdint.h>
#include <vector>

namespace tinygizmo {

struct Float2 {
  float x;
  float y;

  Float2 operator+(const Float2 &rhs) const;
};

struct Float3 {
  float x;
  float y;
  float z;

  static float dot(const Float3 &lhs, const Float3 &rhs);
  static Float3 cross(const Float3 &lhs, const Float3 &rhs);
  float length2() const;
  float length() const;
  Float3 normalize() const;
  Float3 operator+(const Float3 &rhs) const;
  Float3 &operator+=(const Float3 &rhs);
  Float3 operator-() const;
  Float3 operator-(const Float3 &rhs) const;
  Float3 operator*(float s) const;
  Float3 operator*(const Float3 &rhs) const {
    return {
        x * rhs.x,
        y * rhs.y,
        z * rhs.z,
    };
  }
  Float3 operator/(const Float3 &rhs) const;
};

struct Float3x2 {
  Float3 row0;
  Float3 row1;

  Float2 transform(const Float2 &rhs) const;
};

struct UInt3 {
  uint32_t x;
  uint32_t y;
  uint32_t z;
};

struct Float4 {
  float x;
  float y;
  float z;
  float w;

  Float3 xyz() const;

  Float4 static make(const Float3 &v, float w) {
    return {
        v.x,
        v.y,
        v.z,
        w,
    };
  };
};

struct Float4x4 {
  Float4 row0;
  Float4 row1;
  Float4 row2;
  Float4 row3;

  static Float4x4 scaling(float x, float y, float z);

  Float4x4 operator*(const Float4x4 &rhs) const;
  Float4 transform(const Float4 &rhs) const;
};

struct Quaternion : Float4 {
  Float3 xdir() const;
  Float3 ydir() const;
  Float3 zdir() const;

  static Quaternion from_axis_angle(const Float3 &axis, float angle) {
    auto v = axis * std::sin(angle / 2);
    return {v.x, v.y, v.z, std::cos(angle / 2)};
  }

  Float3 rotate(const Float3 &v) const;
  Quaternion inverse() const;
};

struct RigidTransform {
  Quaternion orientation = {0, 0, 0, 1};
  Float3 position = {0, 0, 0};
  Float3 scale = {1, 1, 1};

  bool uniform_scale() const {
    return scale.x == scale.y && scale.x == scale.z;
  }
  Float4x4 matrix() const {
    return {
        Float4::make(orientation.xdir() * scale.x, 0),
        Float4::make(orientation.ydir() * scale.y, 0),
        Float4::make(orientation.zdir() * scale.z, 0),
        Float4::make(position, 1),
    };
  }
  Float3 transform_vector(const Float3 &vec) const {
    return orientation.rotate(vec * scale);
  }
  Float3 transform_point(const Float3 &p) const {
    return position + transform_vector(p);
  }
  Float3 detransform_point(const Float3 &p) const {
    return detransform_vector(p - position);
  }
  Float3 detransform_vector(const Float3 &vec) const {
    return orientation.inverse().rotate(vec) / scale;
  }
};

struct Plane {
  Float3 normal;
  float d;

  static Plane from_normal_and_position(const Float3 &n,
                                        const Float3 &point_on_plane) {
    auto normal = n.normalize();
    return {
        .normal = normal,
        .d = -Float3::dot(normal, point_on_plane),
    };
  }
};

struct Vertex {
  Float3 position;
  Float3 normal;
  Float4 color;
};

struct GeometryMesh {
  std::vector<Vertex> vertices;
  std::vector<UInt3> triangles;

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
      v0.normal += n;
      v1.normal += n;
      v2.normal += n;
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
                                        const Float3 &max_bounds) {
    const auto a = min_bounds, b = max_bounds;
    GeometryMesh mesh;
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
      auto arm = arm1 * std::cos(angle) + arm2 * std::sin(angle);
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
      auto arm = arm1 * c + arm2 * s;
      mesh.vertices.push_back({arm + axis, axis.normalize()});
      mesh.vertices.push_back({arm, -axis.normalize()});
    }
    for (uint32_t i = 2; i < slices; ++i) {
      mesh.triangles.push_back({base, base + i * 2 - 2, base + i * 2});
      mesh.triangles.push_back({base + 1, base + i * 2 + 1, base + i * 2 - 1});
    }
    return mesh;
  }

  static GeometryMesh make_lathed_geometry(const Float3 &axis,
                                           const Float3 &arm1,
                                           const Float3 &arm2, int slices,
                                           const std::vector<Float2> &points,
                                           const float eps = 0.0f);
};
struct Ray {
  Float3 origin;
  Float3 direction;

  Float3 point(float t) const { return origin + direction * t; }

  std::optional<float> intersect_plane(const Plane &plane) const {
    float denom = Float3::dot(plane.normal, this->direction);
    if (std::abs(denom) == 0) {
      // not intersect
      return {};
    }
    return -(Float3::dot(plane.normal, this->origin) + plane.d) / denom;
  }

  bool intersect_triangle(const Float3 &v0, const Float3 &v1, const Float3 &v2,
                          float *hit_t) const {
    auto e1 = v1 - v0;
    auto e2 = v2 - v0;
    auto h = Float3::cross(this->direction, e2);
    auto a = Float3::dot(e1, h);
    if (std::abs(a) == 0) {
      return false;
    }

    float f = 1 / a;
    auto s = this->origin - v0;
    auto u = f * Float3::dot(s, h);
    if (u < 0 || u > 1) {
      return false;
    }

    auto q = Float3::cross(s, e1);
    auto v = f * Float3::dot(this->direction, q);
    if (v < 0 || u + v > 1) {
      return false;
    }

    auto t = f * Float3::dot(e2, q);
    if (t < 0) {
      return false;
    }

    if (hit_t) {
      *hit_t = t;
    }
    return true;
  }

  bool intersect_mesh(const GeometryMesh &mesh, float *hit_t) const {
    float best_t = std::numeric_limits<float>::infinity(), t;
    int32_t best_tri = -1;
    for (auto &tri : mesh.triangles) {
      if (this->intersect_triangle(mesh.vertices[tri.x].position,
                                   mesh.vertices[tri.y].position,
                                   mesh.vertices[tri.z].position, &t) &&
          t < best_t) {
        best_t = t;
        best_tri = uint32_t(&tri - mesh.triangles.data());
      }
    }
    if (best_tri == -1)
      return false;
    if (hit_t)
      *hit_t = best_t;
    return true;
  }
};

} // namespace tinygizmo
