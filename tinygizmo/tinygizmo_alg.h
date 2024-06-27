#pragma once
#include <cmath>
#include <functional>
#include <optional>
#include <stdint.h>

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

struct Transform {
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

  Ray transform(const Transform &p) const {
    return {p.transform_point(this->origin),
            p.transform_vector(this->direction)};
  }

  Ray detransform(const Transform &p) const {
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
};

} // namespace tinygizmo
