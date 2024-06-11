#pragma once
#include <array>   // For std::array, used in the relational operator overloads
#include <cmath>   // For various unary math functions, such as std::sqrt
#include <cstdlib> // To resolve std::abs ambiguity on clang
#include <memory>  // For std::unique_ptr
#include <ostream> // For overloads of operator<< to std::ostream& in the operator<< overloads provided by this library
#include <vector>  // For ...

// Visual Studio versions prior to 2015 lack constexpr support
#if defined(_MSC_VER) && _MSC_VER < 1900 && !defined(constexpr)
#define constexpr
#endif

// This library includes an inline version of linalg.h
// (https://github.com/sgorsten/linalg) in a separate minalg namespace. This is
// to reduce the number of files in this library to 2, without a separate header
// specifically for 3d math.
namespace minalg {
// Small, fixed-length vector type, consisting of exactly M elements of type T,
// and presumed to be a column-vector unless otherwise noted
template <class T, int M> struct vec;
template <class T> struct vec<T, 2> {
  T x, y;
  constexpr vec() : x(), y() {}
  constexpr vec(T x_, T y_) : x(x_), y(y_) {}
  constexpr explicit vec(T s) : vec(s, s) {}
  constexpr explicit vec(const T *p) : vec(p[0], p[1]) {}
  template <class U>
  constexpr explicit vec(const vec<U, 2> &v)
      : vec(static_cast<T>(v.x), static_cast<T>(v.y)) {}
  constexpr const T &operator[](int i) const { return (&x)[i]; }
  T &operator[](int i) { return (&x)[i]; }
};
template <class T> struct vec<T, 3> {
  T x, y, z;
  constexpr vec() : x(), y(), z() {}
  constexpr vec(T x_, T y_, T z_) : x(x_), y(y_), z(z_) {}
  constexpr vec(const vec<T, 2> &xy, T z_) : vec(xy.x, xy.y, z_) {}
  constexpr explicit vec(T s) : vec(s, s, s) {}
  constexpr explicit vec(const T *p) : vec(p[0], p[1], p[2]) {}
  template <class U>
  constexpr explicit vec(const vec<U, 3> &v)
      : vec(static_cast<T>(v.x), static_cast<T>(v.y), static_cast<T>(v.z)) {}
  constexpr const T &operator[](int i) const { return (&x)[i]; }
  T &operator[](int i) { return (&x)[i]; }
  constexpr const vec<T, 2> &xy() const {
    return *reinterpret_cast<const vec<T, 2> *>(this);
  }
  vec<T, 2> &xy() { return *reinterpret_cast<vec<T, 2> *>(this); }
};
template <class T> struct vec<T, 4> {
  T x, y, z, w;
  constexpr vec() : x(), y(), z(), w() {}
  constexpr vec(T x_, T y_, T z_, T w_) : x(x_), y(y_), z(z_), w(w_) {}
  constexpr vec(const vec<T, 2> &xy, T z_, T w_) : vec(xy.x, xy.y, z_, w_) {}
  constexpr vec(const vec<T, 3> &xyz, T w_) : vec(xyz.x, xyz.y, xyz.z, w_) {}
  constexpr explicit vec(T s) : vec(s, s, s, s) {}
  constexpr explicit vec(const T *p) : vec(p[0], p[1], p[2], p[3]) {}
  template <class U>
  constexpr explicit vec(const vec<U, 4> &v)
      : vec(static_cast<T>(v.x), static_cast<T>(v.y), static_cast<T>(v.z),
            static_cast<T>(v.w)) {}
  constexpr const T &operator[](int i) const { return (&x)[i]; }
  T &operator[](int i) { return (&x)[i]; }
  constexpr const vec<T, 2> &xy() const {
    return *reinterpret_cast<const vec<T, 2> *>(this);
  }
  constexpr const vec<T, 3> &xyz() const {
    return *reinterpret_cast<const vec<T, 3> *>(this);
  }
  vec<T, 2> &xy() { return *reinterpret_cast<vec<T, 2> *>(this); }
  vec<T, 3> &xyz() { return *reinterpret_cast<vec<T, 3> *>(this); }
};

// Small, fixed-size matrix type, consisting of exactly M rows and N columns of
// type T, stored in column-major order.
template <class T, int M, int N> struct mat;
template <class T, int M> struct mat<T, M, 2> {
  typedef vec<T, M> V;
  V x, y;
  constexpr mat() : x(), y() {}
  constexpr mat(V x_, V y_) : x(x_), y(y_) {}
  constexpr explicit mat(T s) : x(s), y(s) {}
  constexpr explicit mat(const T *p) : x(p + M * 0), y(p + M * 1) {}
  template <class U>
  constexpr explicit mat(const mat<U, M, 2> &m) : mat(V(m.x), V(m.y)) {}
  constexpr vec<T, 2> row(int i) const { return {x[i], y[i]}; }
  constexpr const V &operator[](int j) const { return (&x)[j]; }
  V &operator[](int j) { return (&x)[j]; }
};
template <class T, int M> struct mat<T, M, 3> {
  typedef vec<T, M> V;
  V x, y, z;
  constexpr mat() : x(), y(), z() {}
  constexpr mat(V x_, V y_, V z_) : x(x_), y(y_), z(z_) {}
  constexpr explicit mat(T s) : x(s), y(s), z(s) {}
  constexpr explicit mat(const T *p)
      : x(p + M * 0), y(p + M * 1), z(p + M * 2) {}
  template <class U>
  constexpr explicit mat(const mat<U, M, 3> &m) : mat(V(m.x), V(m.y), V(m.z)) {}
  constexpr vec<T, 3> row(int i) const { return {x[i], y[i], z[i]}; }
  constexpr const V &operator[](int j) const { return (&x)[j]; }
  V &operator[](int j) { return (&x)[j]; }
};
template <class T, int M> struct mat<T, M, 4> {
  typedef vec<T, M> V;
  V x, y, z, w;
  constexpr mat() : x(), y(), z(), w() {}
  constexpr mat(V x_, V y_, V z_, V w_) : x(x_), y(y_), z(z_), w(w_) {}
  constexpr explicit mat(T s) : x(s), y(s), z(s), w(s) {}
  constexpr explicit mat(const T *p)
      : x(p + M * 0), y(p + M * 1), z(p + M * 2), w(p + M * 3) {}
  template <class U>
  constexpr explicit mat(const mat<U, M, 4> &m)
      : mat(V(m.x), V(m.y), V(m.z), V(m.w)) {}
  constexpr vec<T, 4> row(int i) const { return {x[i], y[i], z[i], w[i]}; }
  constexpr const V &operator[](int j) const { return (&x)[j]; }
  V &operator[](int j) { return (&x)[j]; }
};

// Type traits for a binary operation involving linear algebra types, used for
// SFINAE on templated functions and operator overloads
template <class A, class B> struct traits {};
template <class T, int M> struct traits<vec<T, M>, vec<T, M>> {
  typedef T scalar;
  typedef vec<T, M> result;
  typedef vec<bool, M> bool_result;
  typedef vec<decltype(+T()), M> arith_result;
  typedef std::array<T, M> compare_as;
};
template <class T, int M> struct traits<vec<T, M>, T> {
  typedef T scalar;
  typedef vec<T, M> result;
  typedef vec<bool, M> bool_result;
  typedef vec<decltype(+T()), M> arith_result;
};
template <class T, int M> struct traits<T, vec<T, M>> {
  typedef T scalar;
  typedef vec<T, M> result;
  typedef vec<bool, M> bool_result;
  typedef vec<decltype(+T()), M> arith_result;
};
template <class T, int M, int N> struct traits<mat<T, M, N>, mat<T, M, N>> {
  typedef T scalar;
  typedef mat<T, M, N> result;
  typedef mat<bool, M, N> bool_result;
  typedef mat<decltype(+T()), M, N> arith_result;
  typedef std::array<T, M * N> compare_as;
};
template <class T, int M, int N> struct traits<mat<T, M, N>, T> {
  typedef T scalar;
  typedef mat<T, M, N> result;
  typedef mat<bool, M, N> bool_result;
  typedef mat<decltype(+T()), M, N> arith_result;
};
template <class T, int M, int N> struct traits<T, mat<T, M, N>> {
  typedef T scalar;
  typedef mat<T, M, N> result;
  typedef mat<bool, M, N> bool_result;
  typedef mat<decltype(+T()), M, N> arith_result;
};
template <class A, class B = A>
using scalar_t =
    typename traits<A, B>::scalar; // Underlying scalar type when performing
                                   // elementwise operations
template <class A, class B = A>
using result_t = typename traits<A, B>::result; // Result of calling a function
                                                // on linear algebra types
template <class A, class B = A>
using bool_result_t =
    typename traits<A, B>::bool_result; // Result of a comparison or unary not
                                        // operation on linear algebra types
template <class A, class B = A>
using arith_result_t =
    typename traits<A, B>::arith_result; // Result of an arithmetic operation on
                                         // linear algebra types (accounts for
                                         // integer promotion)

// Produce a scalar by applying f(T,T) -> T to adjacent pairs of elements from
// vector/matrix a in left-to-right order (matching the associativity of
// arithmetic and logical operators)
template <class T, class F> constexpr T fold(const vec<T, 2> &a, F f) {
  return f(a.x, a.y);
}
template <class T, class F> constexpr T fold(const vec<T, 3> &a, F f) {
  return f(f(a.x, a.y), a.z);
}
template <class T, class F> constexpr T fold(const vec<T, 4> &a, F f) {
  return f(f(f(a.x, a.y), a.z), a.w);
}
template <class T, int M, class F>
constexpr T fold(const mat<T, M, 2> &a, F f) {
  return f(fold(a.x, f), fold(a.y, f));
}
template <class T, int M, class F>
constexpr T fold(const mat<T, M, 3> &a, F f) {
  return f(f(fold(a.x, f), fold(a.y, f)), fold(a.z, f));
}
template <class T, int M, class F>
constexpr T fold(const mat<T, M, 4> &a, F f) {
  return f(f(f(fold(a.x, f), fold(a.y, f)), fold(a.z, f)), fold(a.w, f));
}

// Produce a vector/matrix by applying f(T,T) to corresponding pairs of elements
// from vectors/matrix a and b
template <class T, class F>
constexpr auto zip(const vec<T, 2> &a, const vec<T, 2> &b,
                   F f) -> vec<decltype(f(T(), T())), 2> {
  return {f(a.x, b.x), f(a.y, b.y)};
}
template <class T, class F>
constexpr auto zip(const vec<T, 3> &a, const vec<T, 3> &b,
                   F f) -> vec<decltype(f(T(), T())), 3> {
  return {f(a.x, b.x), f(a.y, b.y), f(a.z, b.z)};
}
template <class T, class F>
constexpr auto zip(const vec<T, 4> &a, const vec<T, 4> &b,
                   F f) -> vec<decltype(f(T(), T())), 4> {
  return {f(a.x, b.x), f(a.y, b.y), f(a.z, b.z), f(a.w, b.w)};
}
template <class T, int M, class F>
constexpr auto zip(const vec<T, M> &a, T b,
                   F f) -> vec<decltype(f(T(), T())), M> {
  return zip(a, vec<T, M>(b), f);
}
template <class T, int M, class F>
constexpr auto zip(T a, const vec<T, M> &b,
                   F f) -> vec<decltype(f(T(), T())), M> {
  return zip(vec<T, M>(a), b, f);
}
template <class T, int M, class F>
constexpr auto zip(const mat<T, M, 2> &a, const mat<T, M, 2> &b,
                   F f) -> mat<decltype(f(T(), T())), M, 2> {
  return {zip(a.x, b.x, f), zip(a.y, b.y, f)};
}
template <class T, int M, class F>
constexpr auto zip(const mat<T, M, 3> &a, const mat<T, M, 3> &b,
                   F f) -> mat<decltype(f(T(), T())), M, 3> {
  return {zip(a.x, b.x, f), zip(a.y, b.y, f), zip(a.z, b.z, f)};
}
template <class T, int M, class F>
constexpr auto zip(const mat<T, M, 4> &a, const mat<T, M, 4> &b,
                   F f) -> mat<decltype(f(T(), T())), M, 4> {
  return {zip(a.x, b.x, f), zip(a.y, b.y, f), zip(a.z, b.z, f),
          zip(a.w, b.w, f)};
}
template <class T, int M, int N, class F>
constexpr auto zip(const mat<T, M, N> &a, T b,
                   F f) -> mat<decltype(f(T(), T())), M, N> {
  return zip(a, mat<T, M, N>(b), f);
}
template <class T, int M, int N, class F>
constexpr auto zip(T a, const mat<T, M, N> &b,
                   F f) -> mat<decltype(f(T(), T())), M, N> {
  return zip(mat<T, M, N>(a), b, f);
}

// Produce a vector/matrix by applying f(T) to elements from vector/matrix a
template <class T, int M, class F>
constexpr auto map(const vec<T, M> &a, F f) -> vec<decltype(f(T())), M> {
  return zip(a, a, [f](T l, T) { return f(l); });
}
template <class T, int M, int N, class F>
constexpr auto map(const mat<T, M, N> &a, F f) -> mat<decltype(f(T())), M, N> {
  return zip(a, a, [f](T l, T) { return f(l); });
}

// Relational operators are defined to compare the elements of two vectors or
// matrices lexicographically, in column-major order
template <class A, class C = typename traits<A, A>::compare_as>
constexpr bool operator==(const A &a, const A &b) {
  return reinterpret_cast<const C &>(a) == reinterpret_cast<const C &>(b);
}
template <class A, class C = typename traits<A, A>::compare_as>
constexpr bool operator!=(const A &a, const A &b) {
  return reinterpret_cast<const C &>(a) != reinterpret_cast<const C &>(b);
}
template <class A, class C = typename traits<A, A>::compare_as>
constexpr bool operator<(const A &a, const A &b) {
  return reinterpret_cast<const C &>(a) < reinterpret_cast<const C &>(b);
}
template <class A, class C = typename traits<A, A>::compare_as>
constexpr bool operator>(const A &a, const A &b) {
  return reinterpret_cast<const C &>(a) > reinterpret_cast<const C &>(b);
}
template <class A, class C = typename traits<A, A>::compare_as>
constexpr bool operator<=(const A &a, const A &b) {
  return reinterpret_cast<const C &>(a) <= reinterpret_cast<const C &>(b);
}
template <class A, class C = typename traits<A, A>::compare_as>
constexpr bool operator>=(const A &a, const A &b) {
  return reinterpret_cast<const C &>(a) >= reinterpret_cast<const C &>(b);
}

// Lambdas are not permitted inside constexpr functions, so we provide explicit
// function objects instead
namespace op {
template <class T> struct pos {
  constexpr auto operator()(T r) const -> decltype(+r) { return +r; }
};
template <class T> struct neg {
  constexpr auto operator()(T r) const -> decltype(-r) { return -r; }
};
template <class T> struct add {
  constexpr auto operator()(T l, T r) const -> decltype(l + r) { return l + r; }
};
template <class T> struct sub {
  constexpr auto operator()(T l, T r) const -> decltype(l - r) { return l - r; }
};
template <class T> struct mul {
  constexpr auto operator()(T l, T r) const -> decltype(l * r) { return l * r; }
};
template <class T> struct div {
  constexpr auto operator()(T l, T r) const -> decltype(l / r) { return l / r; }
};
template <class T> struct mod {
  constexpr auto operator()(T l, T r) const -> decltype(l % r) { return l % r; }
};
template <class T> struct lshift {
  constexpr auto operator()(T l, T r) const -> decltype(l << r) {
    return l << r;
  }
};
template <class T> struct rshift {
  constexpr auto operator()(T l, T r) const -> decltype(l >> r) {
    return l >> r;
  }
};

template <class T> struct binary_not {
  constexpr auto operator()(T r) const -> decltype(+r) { return ~r; }
};
template <class T> struct binary_or {
  constexpr auto operator()(T l, T r) const -> decltype(l | r) { return l | r; }
};
template <class T> struct binary_xor {
  constexpr auto operator()(T l, T r) const -> decltype(l ^ r) { return l ^ r; }
};
template <class T> struct binary_and {
  constexpr auto operator()(T l, T r) const -> decltype(l & r) { return l & r; }
};

template <class T> struct logical_not {
  constexpr bool operator()(T r) const { return !r; }
};
template <class T> struct logical_or {
  constexpr bool operator()(T l, T r) const { return l || r; }
};
template <class T> struct logical_and {
  constexpr bool operator()(T l, T r) const { return l && r; }
};

template <class T> struct equal {
  constexpr bool operator()(T l, T r) const { return l == r; }
};
template <class T> struct nequal {
  constexpr bool operator()(T l, T r) const { return l != r; }
};
template <class T> struct less {
  constexpr bool operator()(T l, T r) const { return l < r; }
};
template <class T> struct greater {
  constexpr bool operator()(T l, T r) const { return l > r; }
};
template <class T> struct lequal {
  constexpr bool operator()(T l, T r) const { return l <= r; }
};
template <class T> struct gequal {
  constexpr bool operator()(T l, T r) const { return l >= r; }
};

template <class T> struct min {
  constexpr T operator()(T l, T r) const { return l < r ? l : r; }
};
template <class T> struct max {
  constexpr T operator()(T l, T r) const { return l > r ? l : r; }
};
} // namespace op

// Functions for coalescing scalar values
template <class A> constexpr scalar_t<A> any(const A &a) {
  return fold(a, op::logical_or<scalar_t<A>>{});
}
template <class A> constexpr scalar_t<A> all(const A &a) {
  return fold(a, op::logical_and<scalar_t<A>>{});
}
template <class A> constexpr scalar_t<A> sum(const A &a) {
  return fold(a, op::add<scalar_t<A>>{});
}
template <class A> constexpr scalar_t<A> product(const A &a) {
  return fold(a, op::mul<scalar_t<A>>{});
}
template <class T, int M> int argmin(const vec<T, M> &a) {
  int j = 0;
  for (int i = 1; i < M; ++i)
    if (a[i] < a[j])
      j = i;
  return j;
}
template <class T, int M> int argmax(const vec<T, M> &a) {
  int j = 0;
  for (int i = 1; i < M; ++i)
    if (a[i] > a[j])
      j = i;
  return j;
}
template <class T, int M> T minelem(const vec<T, M> &a) { return a[argmin(a)]; }
template <class T, int M> T maxelem(const vec<T, M> &a) { return a[argmax(a)]; }

// Overloads for unary operators on vectors are implemented in terms of
// elementwise application of the operator
template <class A> constexpr arith_result_t<A> operator+(const A &a) {
  return map(a, op::pos<scalar_t<A>>{});
}
template <class A> constexpr arith_result_t<A> operator-(const A &a) {
  return map(a, op::neg<scalar_t<A>>{});
}
template <class A> constexpr arith_result_t<A> operator~(const A &a) {
  return map(a, op::binary_not<scalar_t<A>>{});
}
template <class A> constexpr bool_result_t<A> operator!(const A &a) {
  return map(a, op::logical_not<scalar_t<A>>{});
}

// Mirror the set of unary scalar math functions to apply elementwise to vectors
template <class A> result_t<A> abs(const A &a) {
  return map(a, [](scalar_t<A> l) { return std::abs(l); });
}
template <class A> result_t<A> floor(const A &a) {
  return map(a, [](scalar_t<A> l) { return std::floor(l); });
}
template <class A> result_t<A> ceil(const A &a) {
  return map(a, [](scalar_t<A> l) { return std::ceil(l); });
}
template <class A> result_t<A> exp(const A &a) {
  return map(a, [](scalar_t<A> l) { return std::exp(l); });
}
template <class A> result_t<A> log(const A &a) {
  return map(a, [](scalar_t<A> l) { return std::log(l); });
}
template <class A> result_t<A> log10(const A &a) {
  return map(a, [](scalar_t<A> l) { return std::log10(l); });
}
template <class A> result_t<A> sqrt(const A &a) {
  return map(a, [](scalar_t<A> l) { return std::sqrt(l); });
}
template <class A> result_t<A> sin(const A &a) {
  return map(a, [](scalar_t<A> l) { return std::sin(l); });
}
template <class A> result_t<A> cos(const A &a) {
  return map(a, [](scalar_t<A> l) { return std::cos(l); });
}
template <class A> result_t<A> tan(const A &a) {
  return map(a, [](scalar_t<A> l) { return std::tan(l); });
}
template <class A> result_t<A> asin(const A &a) {
  return map(a, [](scalar_t<A> l) { return std::asin(l); });
}
template <class A> result_t<A> acos(const A &a) {
  return map(a, [](scalar_t<A> l) { return std::acos(l); });
}
template <class A> result_t<A> atan(const A &a) {
  return map(a, [](scalar_t<A> l) { return std::atan(l); });
}
template <class A> result_t<A> sinh(const A &a) {
  return map(a, [](scalar_t<A> l) { return std::sinh(l); });
}
template <class A> result_t<A> cosh(const A &a) {
  return map(a, [](scalar_t<A> l) { return std::cosh(l); });
}
template <class A> result_t<A> tanh(const A &a) {
  return map(a, [](scalar_t<A> l) { return std::tanh(l); });
}
template <class A> result_t<A> round(const A &a) {
  return map(a, [](scalar_t<A> l) { return std::round(l); });
}
template <class A> result_t<A> fract(const A &a) {
  return map(a, [](scalar_t<A> l) { return l - std::floor(l); });
}

// Overloads for vector op vector are implemented in terms of elementwise
// application of the operator, followed by casting back to the original type
// (integer promotion is suppressed)
template <class A, class B>
constexpr arith_result_t<A, B> operator+(const A &a, const B &b) {
  return zip(a, b, op::add<scalar_t<A, B>>{});
}
template <class A, class B>
constexpr arith_result_t<A, B> operator-(const A &a, const B &b) {
  return zip(a, b, op::sub<scalar_t<A, B>>{});
}
template <class A, class B>
constexpr arith_result_t<A, B> operator*(const A &a, const B &b) {
  return zip(a, b, op::mul<scalar_t<A, B>>{});
}
template <class A, class B>
constexpr arith_result_t<A, B> operator/(const A &a, const B &b) {
  return zip(a, b, op::div<scalar_t<A, B>>{});
}
template <class A, class B>
constexpr arith_result_t<A, B> operator%(const A &a, const B &b) {
  return zip(a, b, op::mod<scalar_t<A, B>>{});
}
template <class A, class B>
constexpr arith_result_t<A, B> operator|(const A &a, const B &b) {
  return zip(a, b, op::binary_or<scalar_t<A, B>>{});
}
template <class A, class B>
constexpr arith_result_t<A, B> operator^(const A &a, const B &b) {
  return zip(a, b, op::binary_xor<scalar_t<A, B>>{});
}
template <class A, class B>
constexpr arith_result_t<A, B> operator&(const A &a, const B &b) {
  return zip(a, b, op::binary_and<scalar_t<A, B>>{});
}
template <class A, class B>
constexpr arith_result_t<A, B> operator<<(const A &a, const B &b) {
  return zip(a, b, op::lshift<scalar_t<A, B>>{});
}
template <class A, class B>
constexpr arith_result_t<A, B> operator>>(const A &a, const B &b) {
  return zip(a, b, op::rshift<scalar_t<A, B>>{});
}

// Overloads for assignment operators are implemented trivially
template <class A, class B> result_t<A, A> &operator+=(A &a, const B &b) {
  return a = a + b;
}
template <class A, class B> result_t<A, A> &operator-=(A &a, const B &b) {
  return a = a - b;
}
template <class A, class B> result_t<A, A> &operator*=(A &a, const B &b) {
  return a = a * b;
}
template <class A, class B> result_t<A, A> &operator/=(A &a, const B &b) {
  return a = a / b;
}
template <class A, class B> result_t<A, A> &operator%=(A &a, const B &b) {
  return a = a % b;
}
template <class A, class B> result_t<A, A> &operator|=(A &a, const B &b) {
  return a = a | b;
}
template <class A, class B> result_t<A, A> &operator^=(A &a, const B &b) {
  return a = a ^ b;
}
template <class A, class B> result_t<A, A> &operator&=(A &a, const B &b) {
  return a = a & b;
}
template <class A, class B> result_t<A, A> &operator<<=(A &a, const B &b) {
  return a = a << b;
}
template <class A, class B> result_t<A, A> &operator>>=(A &a, const B &b) {
  return a = a >> b;
}

// Mirror the set of binary scalar math functions to apply elementwise to
// vectors
template <class A, class B>
constexpr result_t<A, B> min(const A &a, const B &b) {
  return zip(a, b, op::min<scalar_t<A, B>>{});
}
template <class A, class B>
constexpr result_t<A, B> max(const A &a, const B &b) {
  return zip(a, b, op::max<scalar_t<A, B>>{});
}
template <class A, class B>
constexpr result_t<A, B> clamp(const A &a, const B &b, const B &c) {
  return min(max(a, b), c);
} // TODO: Revisit
template <class A, class B> result_t<A, B> fmod(const A &a, const B &b) {
  return zip(
      a, b, [](scalar_t<A, B> l, scalar_t<A, B> r) { return std::fmod(l, r); });
}
template <class A, class B> result_t<A, B> pow(const A &a, const B &b) {
  return zip(a, b,
             [](scalar_t<A, B> l, scalar_t<A, B> r) { return std::pow(l, r); });
}
template <class A, class B> result_t<A, B> atan2(const A &a, const B &b) {
  return zip(a, b, [](scalar_t<A, B> l, scalar_t<A, B> r) {
    return std::atan2(l, r);
  });
}
template <class A, class B> result_t<A, B> copysign(const A &a, const B &b) {
  return zip(a, b, [](scalar_t<A, B> l, scalar_t<A, B> r) {
    return std::copysign(l, r);
  });
}

// Functions for componentwise application of equivalence and relational
// operators
template <class A, class B> bool_result_t<A, B> equal(const A &a, const B &b) {
  return zip(a, b, op::equal<scalar_t<A, B>>{});
}
template <class A, class B> bool_result_t<A, B> nequal(const A &a, const B &b) {
  return zip(a, b, op::nequal<scalar_t<A, B>>{});
}
template <class A, class B> bool_result_t<A, B> less(const A &a, const B &b) {
  return zip(a, b, op::less<scalar_t<A, B>>{});
}
template <class A, class B>
bool_result_t<A, B> greater(const A &a, const B &b) {
  return zip(a, b, op::greater<scalar_t<A, B>>{});
}
template <class A, class B> bool_result_t<A, B> lequal(const A &a, const B &b) {
  return zip(a, b, op::lequal<scalar_t<A, B>>{});
}
template <class A, class B> bool_result_t<A, B> gequal(const A &a, const B &b) {
  return zip(a, b, op::gequal<scalar_t<A, B>>{});
}

// Support for vector algebra
template <class T> constexpr T cross(const vec<T, 2> &a, const vec<T, 2> &b) {
  return a.x * b.y - a.y * b.x;
}
template <class T>
constexpr vec<T, 3> cross(const vec<T, 3> &a, const vec<T, 3> &b) {
  return {a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x};
}
template <class T, int M>
constexpr T dot(const vec<T, M> &a, const vec<T, M> &b) {
  return sum(a * b);
}
template <class T, int M> constexpr T length2(const vec<T, M> &a) {
  return dot(a, a);
}
template <class T, int M> T length(const vec<T, M> &a) {
  return std::sqrt(length2(a));
}
template <class T, int M> vec<T, M> normalize(const vec<T, M> &a) {
  return a / length(a);
}
template <class T, int M>
constexpr T distance2(const vec<T, M> &a, const vec<T, M> &b) {
  return length2(b - a);
}
template <class T, int M> T distance(const vec<T, M> &a, const vec<T, M> &b) {
  return length(b - a);
}
template <class T, int M> T uangle(const vec<T, M> &a, const vec<T, M> &b) {
  T d = dot(a, b);
  return d > 1 ? 0 : std::acos(d < -1 ? -1 : d);
}
template <class T, int M> T angle(const vec<T, M> &a, const vec<T, M> &b) {
  return uangle(normalize(a), normalize(b));
}
template <class T, int M>
constexpr vec<T, M> lerp(const vec<T, M> &a, const vec<T, M> &b, T t) {
  return a * (1 - t) + b * t;
}
template <class T, int M>
vec<T, M> nlerp(const vec<T, M> &a, const vec<T, M> &b, T t) {
  return normalize(lerp(a, b, t));
}
template <class T, int M>
vec<T, M> slerp(const vec<T, M> &a, const vec<T, M> &b, T t) {
  T th = uangle(a, b);
  return th == 0 ? a
                 : a * (std::sin(th * (1 - t)) / std::sin(th)) +
                       b * (std::sin(th * t) / std::sin(th));
}
template <class T, int M>
constexpr mat<T, M, 2> outerprod(const vec<T, M> &a, const vec<T, 2> &b) {
  return {a * b.x, a * b.y};
}
template <class T, int M>
constexpr mat<T, M, 3> outerprod(const vec<T, M> &a, const vec<T, 3> &b) {
  return {a * b.x, a * b.y, a * b.z};
}
template <class T, int M>
constexpr mat<T, M, 4> outerprod(const vec<T, M> &a, const vec<T, 4> &b) {
  return {a * b.x, a * b.y, a * b.z, a * b.w};
}

// Support for quaternion algebra using 4D vectors, representing xi + yj + zk +
// w
template <class T> constexpr vec<T, 4> qconj(const vec<T, 4> &q) {
  return {-q.x, -q.y, -q.z, q.w};
}
template <class T> vec<T, 4> qinv(const vec<T, 4> &q) {
  return qconj(q) / length2(q);
}
template <class T> vec<T, 4> qexp(const vec<T, 4> &q) {
  const auto v = q.xyz();
  const auto vv = length(v);
  return std::exp(q.w) *
         vec<T, 4>{v * (vv > 0 ? std::sin(vv) / vv : 0), std::cos(vv)};
}
template <class T> vec<T, 4> qlog(const vec<T, 4> &q) {
  const auto v = q.xyz();
  const auto vv = length(v), qq = length(q);
  return {v * (vv > 0 ? std::acos(q.w / qq) / vv : 0), std::log(qq)};
}
template <class T> vec<T, 4> qpow(const vec<T, 4> &q, const T &p) {
  const auto v = q.xyz();
  const auto vv = length(v), qq = length(q), th = std::acos(q.w / qq);
  return std::pow(qq, p) *
         vec<T, 4>{v * (vv > 0 ? std::sin(p * th) / vv : 0), std::cos(p * th)};
}
template <class T>
constexpr vec<T, 4> qmul(const vec<T, 4> &a, const vec<T, 4> &b) {
  return {a.x * b.w + a.w * b.x + a.y * b.z - a.z * b.y,
          a.y * b.w + a.w * b.y + a.z * b.x - a.x * b.z,
          a.z * b.w + a.w * b.z + a.x * b.y - a.y * b.x,
          a.w * b.w - a.x * b.x - a.y * b.y - a.z * b.z};
}
template <class T, class... R>
constexpr vec<T, 4> qmul(const vec<T, 4> &a, R... r) {
  return qmul(a, qmul(r...));
}

// Support for 3D spatial rotations using quaternions, via qmul(qmul(q, v),
// qconj(q))
template <class T> constexpr vec<T, 3> qxdir(const vec<T, 4> &q) {
  return {q.w * q.w + q.x * q.x - q.y * q.y - q.z * q.z,
          (q.x * q.y + q.z * q.w) * 2, (q.z * q.x - q.y * q.w) * 2};
}
template <class T> constexpr vec<T, 3> qydir(const vec<T, 4> &q) {
  return {(q.x * q.y - q.z * q.w) * 2,
          q.w * q.w - q.x * q.x + q.y * q.y - q.z * q.z,
          (q.y * q.z + q.x * q.w) * 2};
}
template <class T> constexpr vec<T, 3> qzdir(const vec<T, 4> &q) {
  return {(q.z * q.x + q.y * q.w) * 2, (q.y * q.z - q.x * q.w) * 2,
          q.w * q.w - q.x * q.x - q.y * q.y + q.z * q.z};
}
template <class T> constexpr mat<T, 3, 3> qmat(const vec<T, 4> &q) {
  return {qxdir(q), qydir(q), qzdir(q)};
}
template <class T>
constexpr vec<T, 3> qrot(const vec<T, 4> &q, const vec<T, 3> &v) {
  return qxdir(q) * v.x + qydir(q) * v.y + qzdir(q) * v.z;
}
template <class T> T qangle(const vec<T, 4> &q) { return std::acos(q.w) * 2; }
template <class T> vec<T, 3> qaxis(const vec<T, 4> &q) {
  return normalize(q.xyz());
}
template <class T>
vec<T, 4> qnlerp(const vec<T, 4> &a, const vec<T, 4> &b, T t) {
  return nlerp(a, dot(a, b) < 0 ? -b : b, t);
}
template <class T>
vec<T, 4> qslerp(const vec<T, 4> &a, const vec<T, 4> &b, T t) {
  return slerp(a, dot(a, b) < 0 ? -b : b, t);
}

// Support for matrix algebra
template <class T, int M>
constexpr vec<T, M> mul(const mat<T, M, 2> &a, const vec<T, 2> &b) {
  return a.x * b.x + a.y * b.y;
}
template <class T, int M>
constexpr vec<T, M> mul(const mat<T, M, 3> &a, const vec<T, 3> &b) {
  return a.x * b.x + a.y * b.y + a.z * b.z;
}
template <class T, int M>
constexpr vec<T, M> mul(const mat<T, M, 4> &a, const vec<T, 4> &b) {
  return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
}
template <class T, int M, int N>
constexpr mat<T, M, 2> mul(const mat<T, M, N> &a, const mat<T, N, 2> &b) {
  return {mul(a, b.x), mul(a, b.y)};
}
template <class T, int M, int N>
constexpr mat<T, M, 3> mul(const mat<T, M, N> &a, const mat<T, N, 3> &b) {
  return {mul(a, b.x), mul(a, b.y), mul(a, b.z)};
}
template <class T, int M, int N>
constexpr mat<T, M, 4> mul(const mat<T, M, N> &a, const mat<T, N, 4> &b) {
  return {mul(a, b.x), mul(a, b.y), mul(a, b.z), mul(a, b.w)};
}
#if _MSC_VER >= 1910
template <class T, int M, int N, class... R>
constexpr auto mul(const mat<T, M, N> &a, R... r) {
  return mul(a, mul(r...));
}
#else
template <class T, int M, int N, class... R>
constexpr auto mul(const mat<T, M, N> &a,
                   R... r) -> decltype(mul(a, mul(r...))) {
  return mul(a, mul(r...));
}
#endif
template <class T> constexpr vec<T, 2> diagonal(const mat<T, 2, 2> &a) {
  return {a.x.x, a.y.y};
}
template <class T> constexpr vec<T, 3> diagonal(const mat<T, 3, 3> &a) {
  return {a.x.x, a.y.y, a.z.z};
}
template <class T> constexpr vec<T, 4> diagonal(const mat<T, 4, 4> &a) {
  return {a.x.x, a.y.y, a.z.z, a.w.w};
}
template <class T, int M>
constexpr mat<T, M, 2> transpose(const mat<T, 2, M> &m) {
  return {m.row(0), m.row(1)};
}
template <class T, int M>
constexpr mat<T, M, 3> transpose(const mat<T, 3, M> &m) {
  return {m.row(0), m.row(1), m.row(2)};
}
template <class T, int M>
constexpr mat<T, M, 4> transpose(const mat<T, 4, M> &m) {
  return {m.row(0), m.row(1), m.row(2), m.row(3)};
}
template <class T> mat<T, 2, 2> adjugate(const mat<T, 2, 2> &a) {
  return {{a.y.y, -a.x.y}, {-a.y.x, a.x.x}};
}
template <class T> mat<T, 3, 3> adjugate(const mat<T, 3, 3> &a);
template <class T> mat<T, 4, 4> adjugate(const mat<T, 4, 4> &a);
template <class T> T determinant(const mat<T, 2, 2> &a) {
  return a.x.x * a.y.y - a.x.y * a.y.x;
}
template <class T> T determinant(const mat<T, 3, 3> &a) {
  return a.x.x * (a.y.y * a.z.z - a.z.y * a.y.z) +
         a.x.y * (a.y.z * a.z.x - a.z.z * a.y.x) +
         a.x.z * (a.y.x * a.z.y - a.z.x * a.y.y);
}
template <class T> T determinant(const mat<T, 4, 4> &a);
template <class T, int N> mat<T, N, N> inverse(const mat<T, N, N> &a) {
  return adjugate(a) / determinant(a);
}

// Vectors and matrices can be used as ranges
template <class T, int M> T *begin(vec<T, M> &a) { return &a[0]; }
template <class T, int M> const T *begin(const vec<T, M> &a) { return &a[0]; }
template <class T, int M> T *end(vec<T, M> &a) { return begin(a) + M; }
template <class T, int M> const T *end(const vec<T, M> &a) {
  return begin(a) + M;
}
template <class T, int M, int N> vec<T, M> *begin(mat<T, M, N> &a) {
  return &a[0];
}
template <class T, int M, int N> const vec<T, M> *begin(const mat<T, M, N> &a) {
  return &a[0];
}
template <class T, int M, int N> vec<T, M> *end(mat<T, M, N> &a) {
  return begin(a) + N;
}
template <class T, int M, int N> const vec<T, M> *end(const mat<T, M, N> &a) {
  return begin(a) + N;
}

// Factory functions for 3D spatial transformations
enum fwd_axis {
  neg_z,
  pos_z
}; // Should projection matrices be generated assuming forward is {0,0,-1} or
   // {0,0,1}
enum z_range {
  neg_one_to_one,
  zero_to_one
}; // Should projection matrices map z into the range of [-1,1] or [0,1]?
template <class T> vec<T, 4> rotation_quat(const vec<T, 3> &axis, T angle) {
  return {axis * std::sin(angle / 2), std::cos(angle / 2)};
}
template <class T> vec<T, 4> rotation_quat(const mat<T, 3, 3> &m) {
  return copysign(sqrt(max(T(0), T(1) + vec<T, 4>(m.x.x - m.y.y - m.z.z,
                                                  m.y.y - m.x.x - m.z.z,
                                                  m.z.z - m.x.x - m.y.y,
                                                  m.x.x + m.y.y + m.z.z))) /
                      T(2),
                  vec<T, 4>(m.y.z - m.z.y, m.z.x - m.x.z, m.x.y - m.y.x, 1));
}
template <class T>
mat<T, 4, 4> translation_matrix(const vec<T, 3> &translation) {
  return {{1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 1, 0}, {translation, 1}};
}
template <class T> mat<T, 4, 4> rotation_matrix(const vec<T, 4> &rotation) {
  return {{qxdir(rotation), 0},
          {qydir(rotation), 0},
          {qzdir(rotation), 0},
          {0, 0, 0, 1}};
}
template <class T> mat<T, 4, 4> scaling_matrix(const vec<T, 3> &scaling) {
  return {{scaling.x, 0, 0, 0},
          {0, scaling.y, 0, 0},
          {0, 0, scaling.z, 0},
          {0, 0, 0, 1}};
}
template <class T>
mat<T, 4, 4> pose_matrix(const vec<T, 4> &q, const vec<T, 3> &p) {
  return {{qxdir(q), 0}, {qydir(q), 0}, {qzdir(q), 0}, {p, 1}};
}
template <class T>
mat<T, 4, 4> frustum_matrix(T x0, T x1, T y0, T y1, T n, T f,
                            fwd_axis a = neg_z, z_range z = neg_one_to_one) {
  const T s = a == pos_z ? T(1) : T(-1);
  return z == zero_to_one
             ? mat<T, 4, 4>{{2 * n / (x1 - x0), 0, 0, 0},
                            {0, 2 * n / (y1 - y0), 0, 0},
                            {(x0 + x1) / (x1 - x0), (y0 + y1) / (y1 - y0),
                             s * (f + 0) / (f - n), s},
                            {0, 0, -1 * n * f / (f - n), 0}}
             : mat<T, 4, 4>{{2 * n / (x1 - x0), 0, 0, 0},
                            {0, 2 * n / (y1 - y0), 0, 0},
                            {(x0 + x1) / (x1 - x0), (y0 + y1) / (y1 - y0),
                             s * (f + n) / (f - n), s},
                            {0, 0, -2 * n * f / (f - n), 0}};
}
template <class T>
mat<T, 4, 4> perspective_matrix(T fovy, T aspect, T n, T f, fwd_axis a = neg_z,
                                z_range z = neg_one_to_one) {
  T y = n * std::tan(fovy / 2), x = y * aspect;
  return frustum_matrix(-x, x, -y, y, n, f, a, z);
}

// Provide typedefs for common element types and vector/matrix sizes
typedef vec<bool, 3> bool3;
typedef vec<uint8_t, 3> byte3;
typedef vec<int16_t, 3> short3;
typedef vec<uint16_t, 3> ushort3;
typedef vec<bool, 4> bool4;
typedef vec<uint8_t, 4> byte4;
typedef vec<int16_t, 4> short4;
typedef vec<uint16_t, 4> ushort4;
typedef vec<int, 2> int2;
typedef vec<unsigned, 2> uint2;
typedef vec<float, 2> float2;
typedef vec<double, 2> double2;
typedef vec<int, 3> int3;
typedef vec<unsigned, 3> uint3;
typedef vec<float, 3> float3;
typedef vec<double, 3> double3;
typedef vec<int, 4> int4;
typedef vec<unsigned, 4> uint4;
typedef vec<float, 4> float4;
typedef vec<double, 4> double4;
typedef mat<bool, 3, 2> bool3x2;
typedef mat<int, 3, 2> int3x2;
typedef mat<float, 3, 2> float3x2;
typedef mat<double, 3, 2> double3x2;
typedef mat<bool, 3, 3> bool3x3;
typedef mat<int, 3, 3> int3x3;
typedef mat<float, 3, 3> float3x3;
typedef mat<double, 3, 3> double3x3;
typedef mat<bool, 4, 4> bool4x4;
typedef mat<int, 4, 4> int4x4;
typedef mat<float, 4, 4> float4x4;
typedef mat<double, 4, 4> double4x4;

} // end namespace minalg

// Definitions of linalg functions too long to be defined inline
template <class T>
minalg::mat<T, 3, 3> minalg::adjugate(const mat<T, 3, 3> &a) {
  return {{a.y.y * a.z.z - a.z.y * a.y.z, a.z.y * a.x.z - a.x.y * a.z.z,
           a.x.y * a.y.z - a.y.y * a.x.z},
          {a.y.z * a.z.x - a.z.z * a.y.x, a.z.z * a.x.x - a.x.z * a.z.x,
           a.x.z * a.y.x - a.y.z * a.x.x},
          {a.y.x * a.z.y - a.z.x * a.y.y, a.z.x * a.x.y - a.x.x * a.z.y,
           a.x.x * a.y.y - a.y.x * a.x.y}};
}

template <class T>
minalg::mat<T, 4, 4> minalg::adjugate(const mat<T, 4, 4> &a) {
  return {{a.y.y * a.z.z * a.w.w + a.w.y * a.y.z * a.z.w +
               a.z.y * a.w.z * a.y.w - a.y.y * a.w.z * a.z.w -
               a.z.y * a.y.z * a.w.w - a.w.y * a.z.z * a.y.w,
           a.x.y * a.w.z * a.z.w + a.z.y * a.x.z * a.w.w +
               a.w.y * a.z.z * a.x.w - a.w.y * a.x.z * a.z.w -
               a.z.y * a.w.z * a.x.w - a.x.y * a.z.z * a.w.w,
           a.x.y * a.y.z * a.w.w + a.w.y * a.x.z * a.y.w +
               a.y.y * a.w.z * a.x.w - a.x.y * a.w.z * a.y.w -
               a.y.y * a.x.z * a.w.w - a.w.y * a.y.z * a.x.w,
           a.x.y * a.z.z * a.y.w + a.y.y * a.x.z * a.z.w +
               a.z.y * a.y.z * a.x.w - a.x.y * a.y.z * a.z.w -
               a.z.y * a.x.z * a.y.w - a.y.y * a.z.z * a.x.w},
          {a.y.z * a.w.w * a.z.x + a.z.z * a.y.w * a.w.x +
               a.w.z * a.z.w * a.y.x - a.y.z * a.z.w * a.w.x -
               a.w.z * a.y.w * a.z.x - a.z.z * a.w.w * a.y.x,
           a.x.z * a.z.w * a.w.x + a.w.z * a.x.w * a.z.x +
               a.z.z * a.w.w * a.x.x - a.x.z * a.w.w * a.z.x -
               a.z.z * a.x.w * a.w.x - a.w.z * a.z.w * a.x.x,
           a.x.z * a.w.w * a.y.x + a.y.z * a.x.w * a.w.x +
               a.w.z * a.y.w * a.x.x - a.x.z * a.y.w * a.w.x -
               a.w.z * a.x.w * a.y.x - a.y.z * a.w.w * a.x.x,
           a.x.z * a.y.w * a.z.x + a.z.z * a.x.w * a.y.x +
               a.y.z * a.z.w * a.x.x - a.x.z * a.z.w * a.y.x -
               a.y.z * a.x.w * a.z.x - a.z.z * a.y.w * a.x.x},
          {a.y.w * a.z.x * a.w.y + a.w.w * a.y.x * a.z.y +
               a.z.w * a.w.x * a.y.y - a.y.w * a.w.x * a.z.y -
               a.z.w * a.y.x * a.w.y - a.w.w * a.z.x * a.y.y,
           a.x.w * a.w.x * a.z.y + a.z.w * a.x.x * a.w.y +
               a.w.w * a.z.x * a.x.y - a.x.w * a.z.x * a.w.y -
               a.w.w * a.x.x * a.z.y - a.z.w * a.w.x * a.x.y,
           a.x.w * a.y.x * a.w.y + a.w.w * a.x.x * a.y.y +
               a.y.w * a.w.x * a.x.y - a.x.w * a.w.x * a.y.y -
               a.y.w * a.x.x * a.w.y - a.w.w * a.y.x * a.x.y,
           a.x.w * a.z.x * a.y.y + a.y.w * a.x.x * a.z.y +
               a.z.w * a.y.x * a.x.y - a.x.w * a.y.x * a.z.y -
               a.z.w * a.x.x * a.y.y - a.y.w * a.z.x * a.x.y},
          {a.y.x * a.w.y * a.z.z + a.z.x * a.y.y * a.w.z +
               a.w.x * a.z.y * a.y.z - a.y.x * a.z.y * a.w.z -
               a.w.x * a.y.y * a.z.z - a.z.x * a.w.y * a.y.z,
           a.x.x * a.z.y * a.w.z + a.w.x * a.x.y * a.z.z +
               a.z.x * a.w.y * a.x.z - a.x.x * a.w.y * a.z.z -
               a.z.x * a.x.y * a.w.z - a.w.x * a.z.y * a.x.z,
           a.x.x * a.w.y * a.y.z + a.y.x * a.x.y * a.w.z +
               a.w.x * a.y.y * a.x.z - a.x.x * a.y.y * a.w.z -
               a.w.x * a.x.y * a.y.z - a.y.x * a.w.y * a.x.z,
           a.x.x * a.y.y * a.z.z + a.z.x * a.x.y * a.y.z +
               a.y.x * a.z.y * a.x.z - a.x.x * a.z.y * a.y.z -
               a.y.x * a.x.y * a.z.z - a.z.x * a.y.y * a.x.z}};
}

template <class T> T minalg::determinant(const mat<T, 4, 4> &a) {
  return a.x.x * (a.y.y * a.z.z * a.w.w + a.w.y * a.y.z * a.z.w +
                  a.z.y * a.w.z * a.y.w - a.y.y * a.w.z * a.z.w -
                  a.z.y * a.y.z * a.w.w - a.w.y * a.z.z * a.y.w) +
         a.x.y * (a.y.z * a.w.w * a.z.x + a.z.z * a.y.w * a.w.x +
                  a.w.z * a.z.w * a.y.x - a.y.z * a.z.w * a.w.x -
                  a.w.z * a.y.w * a.z.x - a.z.z * a.w.w * a.y.x) +
         a.x.z * (a.y.w * a.z.x * a.w.y + a.w.w * a.y.x * a.z.y +
                  a.z.w * a.w.x * a.y.y - a.y.w * a.w.x * a.z.y -
                  a.z.w * a.y.x * a.w.y - a.w.w * a.z.x * a.y.y) +
         a.x.w * (a.y.x * a.w.y * a.z.z + a.z.x * a.y.y * a.w.z +
                  a.w.x * a.z.y * a.y.z - a.y.x * a.z.y * a.w.z -
                  a.w.x * a.y.y * a.z.z - a.z.x * a.w.y * a.y.z);
}

//////////////////////////
//   Linalg Utilities   //
//////////////////////////

template <class T>
std::ostream &operator<<(std::ostream &a, minalg::vec<T, 2> &b) {
  return a << '{' << b.x << ", " << b.y << '}';
}
template <class T>
std::ostream &operator<<(std::ostream &a, minalg::vec<T, 3> &b) {
  return a << '{' << b.x << ", " << b.y << ", " << b.z << '}';
}
template <class T>
std::ostream &operator<<(std::ostream &a, minalg::vec<T, 4> &b) {
  return a << '{' << b.x << ", " << b.y << ", " << b.z << ", " << b.w << '}';
}
template <class T, int N>
std::ostream &operator<<(std::ostream &a, const minalg::mat<T, 3, N> &b) {
  return a << '\n' << b.row(0) << '\n' << b.row(1) << '\n' << b.row(2) << '\n';
}
template <class T, int N>
std::ostream &operator<<(std::ostream &a, const minalg::mat<T, 4, N> &b) {
  return a << '\n'
           << b.row(0) << '\n'
           << b.row(1) << '\n'
           << b.row(2) << '\n'
           << b.row(3) << '\n';
}

///////////////////////
//   Utility Math    //
///////////////////////

static const float tau = 6.28318530718f;

void flush_to_zero(minalg::float3 &f) {
  if (std::abs(f.x) < 0.02f)
    f.x = 0.f;
  if (std::abs(f.y) < 0.02f)
    f.y = 0.f;
  if (std::abs(f.z) < 0.02f)
    f.z = 0.f;
}

// 32 bit Fowler-Noll-Vo Hash
uint32_t hash_fnv1a(const std::string &str) {
  static const uint32_t fnv1aBase32 = 0x811C9DC5u;
  static const uint32_t fnv1aPrime32 = 0x01000193u;

  uint32_t result = fnv1aBase32;

  for (auto &c : str) {
    result ^= static_cast<uint32_t>(c);
    result *= fnv1aPrime32;
  }
  return result;
}

minalg::float3 snap(const minalg::float3 &value, const float snap) {
  if (snap > 0.0f)
    return minalg::float3(floor(value / snap) * snap);
  return value;
}

minalg::float4 make_rotation_quat_axis_angle(const minalg::float3 &axis,
                                             float angle) {
  return {axis * std::sin(angle / 2), std::cos(angle / 2)};
}

minalg::float4 make_rotation_quat_between_vectors_snapped(
    const minalg::float3 &from, const minalg::float3 &to, const float angle) {
  auto a = normalize(from);
  auto b = normalize(to);
  auto snappedAcos = std::floor(std::acos(dot(a, b)) / angle) * angle;
  return make_rotation_quat_axis_angle(normalize(cross(a, b)), snappedAcos);
}

template <typename T> T clamp(const T &val, const T &min, const T &max) {
  return std::min(std::max(val, min), max);
}

struct rigid_transform {
  rigid_transform() {}
  rigid_transform(const minalg::float4 &orientation,
                  const minalg::float3 &position, const minalg::float3 &scale)
      : orientation(orientation), position(position), scale(scale) {}
  rigid_transform(const minalg::float4 &orientation,
                  const minalg::float3 &position, float scale)
      : orientation(orientation), position(position), scale(scale) {}
  rigid_transform(const minalg::float4 &orientation,
                  const minalg::float3 &position)
      : orientation(orientation), position(position) {}

  minalg::float4 orientation{0, 0, 0, 1};
  minalg::float3 position{0, 0, 0};
  minalg::float3 scale{1, 1, 1};

  bool uniform_scale() const {
    return scale.x == scale.y && scale.x == scale.z;
  }
  minalg::float4x4 matrix() const {
    return {{qxdir(orientation) * scale.x, 0},
            {qydir(orientation) * scale.y, 0},
            {qzdir(orientation) * scale.z, 0},
            {position, 1}};
  }
  minalg::float3 transform_vector(const minalg::float3 &vec) const {
    return qrot(orientation, vec * scale);
  }
  minalg::float3 transform_point(const minalg::float3 &p) const {
    return position + transform_vector(p);
  }
  minalg::float3 detransform_point(const minalg::float3 &p) const {
    return detransform_vector(p - position);
  }
  minalg::float3 detransform_vector(const minalg::float3 &vec) const {
    return qrot(qinv(orientation), vec) / scale;
  }
};
struct ray {
  minalg::float3 origin, direction;
};
ray transform(const rigid_transform &p, const ray &r) {
  return {p.transform_point(r.origin), p.transform_vector(r.direction)};
}
ray detransform(const rigid_transform &p, const ray &r) {
  return {p.detransform_point(r.origin), p.detransform_vector(r.direction)};
}
minalg::float3 transform_coord(const minalg::float4x4 &transform,
                               const minalg::float3 &coord) {
  auto r = mul(transform, minalg::float4(coord, 1));
  return (r.xyz() / r.w);
}
minalg::float3 transform_vector(const minalg::float4x4 &transform,
                                const minalg::float3 &vector) {
  return mul(transform, minalg::float4(vector, 0)).xyz();
}
void transform(const float scale, ray &r) {
  r.origin *= scale;
  r.direction *= scale;
}
void detransform(const float scale, ray &r) {
  r.origin /= scale;
  r.direction /= scale;
}

struct vertex {
  minalg::float3 position;
  minalg::float3 normal;
  minalg::float4 color;
};
struct geometry_mesh {
  std::vector<vertex> vertices;
  std::vector<minalg::uint3> triangles;
};
struct gizmo_renderable {
  geometry_mesh mesh;
  minalg::float4 color;
};
struct gizmo_mesh_component {
  geometry_mesh mesh;
  minalg::float4 base_color, highlight_color;
};

/////////////////////////////////////////
// Ray-Geometry Intersection Functions //
/////////////////////////////////////////

bool intersect_ray_plane(const ray &ray, const minalg::float4 &plane,
                         float *hit_t) {
  float denom = dot(plane.xyz(), ray.direction);
  if (std::abs(denom) == 0)
    return false;
  if (hit_t)
    *hit_t = -dot(plane, minalg::float4(ray.origin, 1)) / denom;
  return true;
}

bool intersect_ray_triangle(const ray &ray, const minalg::float3 &v0,
                            const minalg::float3 &v1, const minalg::float3 &v2,
                            float *hit_t) {
  auto e1 = v1 - v0, e2 = v2 - v0, h = cross(ray.direction, e2);
  auto a = dot(e1, h);
  if (std::abs(a) == 0)
    return false;

  float f = 1 / a;
  auto s = ray.origin - v0;
  auto u = f * dot(s, h);
  if (u < 0 || u > 1)
    return false;

  auto q = cross(s, e1);
  auto v = f * dot(ray.direction, q);
  if (v < 0 || u + v > 1)
    return false;

  auto t = f * dot(e2, q);
  if (t < 0)
    return false;

  if (hit_t)
    *hit_t = t;
  return true;
}

bool intersect_ray_mesh(const ray &ray, const geometry_mesh &mesh,
                        float *hit_t) {
  float best_t = std::numeric_limits<float>::infinity(), t;
  int32_t best_tri = -1;
  for (auto &tri : mesh.triangles) {
    if (intersect_ray_triangle(ray, mesh.vertices[tri[0]].position,
                               mesh.vertices[tri[1]].position,
                               mesh.vertices[tri[2]].position, &t) &&
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

///////////////////////////////
// Geometry + Mesh Utilities //
///////////////////////////////

void compute_normals(geometry_mesh &mesh) {
  static const double NORMAL_EPSILON = 0.0001;

  std::vector<uint32_t> uniqueVertIndices(mesh.vertices.size(), 0);
  for (uint32_t i = 0; i < uniqueVertIndices.size(); ++i) {
    if (uniqueVertIndices[i] == 0) {
      uniqueVertIndices[i] = i + 1;
      auto v0 = mesh.vertices[i].position;
      for (auto j = i + 1; j < mesh.vertices.size(); ++j) {
        auto v1 = mesh.vertices[j].position;
        if (length2(v1 - v0) < NORMAL_EPSILON) {
          uniqueVertIndices[j] = uniqueVertIndices[i];
        }
      }
    }
  }

  uint32_t idx0, idx1, idx2;
  for (auto &t : mesh.triangles) {
    idx0 = uniqueVertIndices[t.x] - 1;
    idx1 = uniqueVertIndices[t.y] - 1;
    idx2 = uniqueVertIndices[t.z] - 1;

    auto &v0 = mesh.vertices[idx0];
    auto &v1 = mesh.vertices[idx1];
    auto &v2 = mesh.vertices[idx2];
    auto n = cross(v1.position - v0.position, v2.position - v0.position);
    v0.normal += n;
    v1.normal += n;
    v2.normal += n;
  }

  for (uint32_t i = 0; i < mesh.vertices.size(); ++i)
    mesh.vertices[i].normal = mesh.vertices[uniqueVertIndices[i] - 1].normal;
  for (auto &v : mesh.vertices)
    v.normal = normalize(v.normal);
}

geometry_mesh make_box_geometry(const minalg::float3 &min_bounds,
                                const minalg::float3 &max_bounds) {
  const auto a = min_bounds, b = max_bounds;
  geometry_mesh mesh;
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

geometry_mesh make_cylinder_geometry(const minalg::float3 &axis,
                                     const minalg::float3 &arm1,
                                     const minalg::float3 &arm2,
                                     uint32_t slices) {
  // Generated curved surface
  geometry_mesh mesh;

  for (uint32_t i = 0; i <= slices; ++i) {
    const float tex_s = static_cast<float>(i) / slices,
                angle = (float)(i % slices) * tau / slices;
    auto arm = arm1 * std::cos(angle) + arm2 * std::sin(angle);
    mesh.vertices.push_back({arm, normalize(arm)});
    mesh.vertices.push_back({arm + axis, normalize(arm)});
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
    mesh.vertices.push_back({arm + axis, normalize(axis)});
    mesh.vertices.push_back({arm, -normalize(axis)});
  }
  for (uint32_t i = 2; i < slices; ++i) {
    mesh.triangles.push_back({base, base + i * 2 - 2, base + i * 2});
    mesh.triangles.push_back({base + 1, base + i * 2 + 1, base + i * 2 - 1});
  }
  return mesh;
}

geometry_mesh make_lathed_geometry(const minalg::float3 &axis,
                                   const minalg::float3 &arm1,
                                   const minalg::float3 &arm2, int slices,
                                   const std::vector<minalg::float2> &points,
                                   const float eps = 0.0f) {
  geometry_mesh mesh;
  for (int i = 0; i <= slices; ++i) {
    const float angle = (static_cast<float>(i % slices) * tau / slices) +
                        (tau / 8.f),
                c = std::cos(angle), s = std::sin(angle);
    const minalg::float3x2 mat = {axis, arm1 * c + arm2 * s};
    for (auto &p : points)
      mesh.vertices.push_back({mul(mat, p) + eps, minalg::float3(0.f)});

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
  compute_normals(mesh);
  return mesh;
}