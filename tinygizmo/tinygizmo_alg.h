#pragma once
#include <cmath>
#include <functional>
#include <optional>
#include <stdint.h>
#include <vector>

namespace tinygizmo {

struct Float2 {
  float x;
  float y;

  Float2 operator+(const Float2 &rhs) const { return {x + rhs.x, y + rhs.y}; }
};

struct Float3 {
  float x;
  float y;
  float z;

  static float dot(const Float3 &lhs, const Float3 &rhs) {
    return lhs.x * rhs.x + lhs.y * rhs.y + lhs.z * rhs.z;
  }

  float length2() const { return dot(*this, *this); }

  float length() const { return std::sqrt(length2()); }

  Float3 scale(float f) const {
    return {
        x * f,
        y * f,
        z * f,
    };
  }

  Float3 normalize() const { return scale(1.0f / length()); };

  static Float3 cross(const Float3 &a, const Float3 &b) {
    return {a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z,
            a.x * b.y - a.y * b.x};
  }

  Float3 operator+(const Float3 &rhs) const {
    return {
        x + rhs.x,
        y + rhs.y,
        z + rhs.z,
    };
  }

  Float3 operator-() const {
    return {
        -x,
        -y,
        -z,
    };
  }

  Float3 operator-(const Float3 &rhs) const {
    return {
        x - rhs.x,
        y - rhs.y,
        z - rhs.z,
    };
  }

  Float3 mult_each(const Float3 &rhs) const {
    return {
        x * rhs.x,
        y * rhs.y,
        z * rhs.z,
    };
  }

  Float3 div_each(const Float3 &rhs) const {
    return {
        x / rhs.x,
        y / rhs.y,
        z / rhs.z,
    };
  };
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

  static float dot(const Float4 &lhs, const Float4 &rhs) {
    return lhs.x * rhs.x + lhs.y * rhs.y + lhs.z * rhs.z + lhs.w * rhs.w;
  }

  float length2() const { return dot(*this, *this); }

  float length() const { return std::sqrt(length2()); }

  Float3 xyz() const { return {x, y, z}; }

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
  float m00, m01, m02, m03;
  float m10, m11, m12, m13;
  float m20, m21, m22, m23;
  float m30, m31, m32, m33;

  Float4 row0() const { return {m00, m01, m02, m03}; }
  Float4 row1() const { return {m10, m11, m12, m13}; }
  Float4 row2() const { return {m20, m21, m22, m23}; }
  Float4 row3() const { return {m30, m31, m32, m33}; }

  Float4 col0() const { return {m00, m10, m20, m30}; }
  Float4 col1() const { return {m01, m11, m21, m31}; }
  Float4 col2() const { return {m02, m12, m22, m32}; }
  Float4 col3() const { return {m03, m13, m23, m33}; }

  static Float4x4 make(const Float4 &row0, const Float4 &row1,
                       const Float4 &row2, const Float4 &row3) {
    return {
        row0.x, row0.y, row0.z, row0.w, //
        row1.x, row1.y, row1.z, row1.w, //
        row2.x, row2.y, row2.z, row2.w, //
        row3.x, row3.y, row3.z, row3.w, //
    };
  }

  static Float4x4 scaling(float x, float y, float z) {
    return {
        x, 0, 0, 0, //
        0, y, 0, 0, //
        0, 0, z, 0, //
        0, 0, 0, 1, //
    };
  };

  Float4x4 operator*(const Float4x4 &rhs) const {
    return {
        Float4::dot(row0(), rhs.col0()), Float4::dot(row0(), rhs.col1()),
        Float4::dot(row0(), rhs.col2()), Float4::dot(row0(), rhs.col3()), //
        Float4::dot(row1(), rhs.col0()), Float4::dot(row1(), rhs.col1()),
        Float4::dot(row1(), rhs.col2()), Float4::dot(row1(), rhs.col3()), //
        Float4::dot(row2(), rhs.col0()), Float4::dot(row2(), rhs.col1()),
        Float4::dot(row2(), rhs.col2()), Float4::dot(row2(), rhs.col3()), //
        Float4::dot(row3(), rhs.col0()), Float4::dot(row3(), rhs.col1()),
        Float4::dot(row3(), rhs.col2()), Float4::dot(row3(), rhs.col3()), //
    };
  }

  Float4 transform(const Float4 &rhs) const {
    return {
        Float4::dot(rhs, col0()),
        Float4::dot(rhs, col1()),
        Float4::dot(rhs, col2()),
        Float4::dot(rhs, col3()),
    };
  }
};

struct Quaternion : Float4 {
  Float3 xdir() const {
    auto &q = *this;
    return {q.w * q.w + q.x * q.x - q.y * q.y - q.z * q.z,
            (q.x * q.y + q.z * q.w) * 2, (q.z * q.x - q.y * q.w) * 2};
  }
  Float3 ydir() const {
    auto &q = *this;
    return {(q.x * q.y - q.z * q.w) * 2,
            q.w * q.w - q.x * q.x + q.y * q.y - q.z * q.z,
            (q.y * q.z + q.x * q.w) * 2};
  }
  Float3 zdir() const {
    auto &q = *this;
    return {(q.z * q.x + q.y * q.w) * 2, (q.y * q.z - q.x * q.w) * 2,
            q.w * q.w - q.x * q.x - q.y * q.y + q.z * q.z};
  }

  static Quaternion from_axis_angle(const Float3 &axis, float angle) {
    auto v = axis.scale(std::sin(angle / 2));
    return {v.x, v.y, v.z, std::cos(angle / 2)};
  }

  Quaternion operator*(const Quaternion &b) const {
    auto &a = *this;
    return {a.x * b.w + a.w * b.x + a.y * b.z - a.z * b.y,
            a.y * b.w + a.w * b.y + a.z * b.x - a.x * b.z,
            a.z * b.w + a.w * b.z + a.x * b.y - a.y * b.x,
            a.w * b.w - a.x * b.x - a.y * b.y - a.z * b.z};
  }

  Quaternion scale(float f) const {
    return {
        x * f,
        y * f,
        z * f,
        w * f,
    };
  }

  Quaternion conjugage() const { return {-x, -y, -z, w}; }
  Quaternion inverse() const { return conjugage().scale(1.0f / length2()); }

  Float3 rotate(const Float3 &v) const {
    return xdir().scale(v.x) + ydir().scale(v.y) + zdir().scale(v.z);
  }
};

struct RigidTransform {
  Quaternion orientation = {0, 0, 0, 1};
  Float3 position = {0, 0, 0};
  Float3 scale = {1, 1, 1};

  bool uniform_scale() const {
    return scale.x == scale.y && scale.x == scale.z;
  }
  Float4x4 matrix() const {
    return Float4x4::make(Float4::make(orientation.xdir().scale(scale.x), 0),
                          Float4::make(orientation.ydir().scale(scale.y), 0),
                          Float4::make(orientation.zdir().scale(scale.z), 0),
                          Float4::make(position, 1));
  }
  Float3 transform_vector(const Float3 &vec) const {
    return orientation.rotate(vec.mult_each(scale));
  }
  Float3 transform_point(const Float3 &p) const {
    return position + transform_vector(p);
  }
  Float3 detransform_point(const Float3 &p) const {
    return detransform_vector(p - position);
  }
  Float3 detransform_vector(const Float3 &vec) const {
    return orientation.inverse().rotate(vec).div_each(scale);
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

using AddTriangleFunc = std::function<void(const Float4 &rgba, const Float3 &p0,
                                           const Float3 &p1, const Float3 &p2)>;

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

  static GeometryMesh make_lathed_geometry(const Float3 &axis,
                                           const Float3 &arm1,
                                           const Float3 &arm2, int slices,
                                           const std::vector<Float2> &points,
                                           const float eps = 0.0f) {

    const float tau = 6.28318530718f;

    GeometryMesh mesh;
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
};

struct Ray {
  Float3 origin;
  Float3 direction;

  Ray scaling(const float scale) const {
    return {
        .origin = this->origin.scale(scale),
        .direction = this->direction.scale(scale),
    };
  }

  Ray descale(const float scale) const {
    return {
        .origin = this->origin.scale(1.0f / scale),
        .direction = this->direction.scale(1.0f / scale),
    };
  }

  Ray transform(const RigidTransform &p) const {
    return {p.transform_point(this->origin),
            p.transform_vector(this->direction)};
  }

  Ray detransform(const RigidTransform &p) const {
    return {p.detransform_point(this->origin),
            p.detransform_vector(this->direction)};
  }

  Float3 point(float t) const { return origin + direction.scale(t); }

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
