#pragma once

#include <xmmintrin.h>
#include <smmintrin.h>
#include <immintrin.h>

//! dst[0->3] = { src0[x], src0[y], src1[z], src1[w] }
#define SHUFPS(x, y, z, w)  ((x << 0) | (y << 2) | (z << 4) | (w << 6))

namespace intrinsic {

class Scalar {
public:
    Scalar() {}
    Scalar(float X)
        : _value(_mm_set_ps1(X)) {}

    bool operator==(Scalar const& a) const {
        return _mm_comieq_ss(_value, a._value) == 1;
    }

    bool operator!=(Scalar const& a) const {
        return _mm_comineq_ss(_value, a._value) == 1;
    }

    bool operator<(Scalar const& a) const {
        return _mm_comilt_ss(_value, a._value) == 1;
    }

    bool operator>(Scalar const& a) const {
        return _mm_comigt_ss(_value, a._value) == 1;
    }

    bool operator<=(Scalar const& a) const {
        return _mm_comile_ss(_value, a._value) == 1;
    }

    bool operator>=(Scalar const& a) const {
        return _mm_comige_ss(_value, a._value) == 1;
    }

    friend bool operator<(float a, Scalar const& b) {
        return _mm_comilt_ss(_mm_set_ss(a), b._value) == 1;
    }

    friend bool operator>(float a, Scalar const& b) {
        return _mm_comigt_ss(_mm_set_ss(a), b._value) == 1;
    }

    friend bool operator<=(float a, Scalar const& b) {
        return _mm_comile_ss(_mm_set_ss(a), b._value) == 1;
    }

    friend bool operator>=(float a, Scalar const& b) {
        return _mm_comige_ss(_mm_set_ss(a), b._value) == 1;
    }

    Scalar operator-() const {
        return _mm_sub_ps(_mm_setzero_ps(), _value);
    }

    Scalar operator+(Scalar const& a) const {
        return _mm_add_ps(_value, a._value);
    }

    Scalar operator-(Scalar const& a) const {
        return _mm_sub_ps(_value, a._value);
    }

    Scalar operator*(Scalar const& a) const {
        return _mm_mul_ps(_value, a._value);
    }

    Scalar operator/(Scalar const& a) const {
        return _mm_div_ps(_value, a._value);
    }

    friend Scalar operator+(float a, Scalar const& b) {
        return _mm_add_ps(_mm_set_ps1(a), b._value);
    }

    friend Scalar operator-(float a, Scalar const& b) {
        return _mm_sub_ps(_mm_set_ps1(a), b._value);
    }

    friend Scalar operator*(float a, Scalar const& b) {
        return _mm_mul_ps(_mm_set_ps1(a), b._value);
    }

    friend Scalar operator/(float a, Scalar const& b) {
        return _mm_div_ps(_mm_set_ps1(a), b._value);
    }

    friend Scalar abs(Scalar const& a) {
        return _mm_andnot_ps(_mm_set_ps1(-0.f), a._value);
    }

    friend Scalar sqrt(Scalar const& a) {
        return _mm_sqrt_ps(a._value);
    }

private:
    __m128 _value;

private:
    friend class Vector;
    friend class Matrix;

    Scalar(__m128 const& value)
        : _value(value) {}
};

class Vector {
public:
    Vector() {}
    Vector(float X, float Y, float Z, float W)
        : _value(_mm_set_ps(W, Z, Y, X)) {}

    bool operator==(Vector const& a) const {
        return _mm_movemask_ps(_mm_cmpeq_ps(_value, a._value)) == 0xf;
    }

    bool operator!=(Vector const& a) const {
        return _mm_movemask_ps(_mm_cmpneq_ps(_value, a._value)) != 0x0;
    }

    Vector operator+(Vector const& a) const {
        return _mm_add_ps(_value, a._value);
    }

    Vector operator-(Vector const& a) const {
        return _mm_sub_ps(_value, a._value);
    }

    Vector operator*(Scalar const& s) const {
        return _mm_mul_ps(_value, s._value);
    }

    friend Vector operator*(Scalar const& s, Vector const& a) {
        return a * s;
    }

    Vector operator/(Scalar const& s) const {
        return _mm_div_ps(_value, s._value);
    }

    Vector operator-() const {
        return _mm_sub_ps(_mm_setzero_ps(), _value);
    }

    Scalar Length() const {
        return _mm_sqrt_ps(_mm_dp_ps(_value, _value, 0xff));
    }

    Scalar LengthFast() const {
        auto lsqr = _mm_dp_ps(_value, _value, 0xff);
        return _mm_mul_ps(lsqr, _mm_rsqrt_ss(lsqr));
    }

    Scalar LengthSqr() const {
        return _mm_dp_ps(_value, _value, 0xff);
    }

    Vector Normalize() const {
        return _mm_div_ps(_value, _mm_sqrt_ps(_mm_dp_ps(_value, _value, 0xff)));
    }

    Vector NormalizeFast() const {
        return _mm_mul_ps(_value, _mm_rsqrt_ps(_mm_dp_ps(_value, _value, 0xff)));
    }

    //! Dot product in R4.
    Scalar operator*(Vector const& a) const {
        return _mm_dp_ps(_value, a._value, 0xff);
    }

    //! Cross product in R3.
    Vector operator%(Vector const& a) const {
        //  y2      y0      y1      y3
        auto shuf1 = _mm_shuffle_ps(_value, _value, SHUFPS(2, 0, 1, 3));
        //  y1      y2      y0      y3
        auto shuf2 = _mm_shuffle_ps(_value, _value, SHUFPS(1, 2, 0, 3));

        //  y2*z0   y0*z1   y1*z2   --
        auto prod1 = _mm_mul_ps(shuf1, a._value);
        //  y1*z0   y2*z1   y0*z2   --
        auto prod2 = _mm_mul_ps(shuf2, a._value);

        //  y1*z2   y2*z0   y0*z1   --
        auto shuf3 = _mm_shuffle_ps(prod1, prod1, SHUFPS(2, 0, 1, 3));
        //  y2*z1   y0*z2   y1*z0   --
        auto shuf4 = _mm_shuffle_ps(prod2, prod2, SHUFPS(1, 2, 0, 3));

        return _mm_sub_ps(shuf3, shuf4);
    }

    //! Return the projection of `a` onto this vector.
    Vector Project(Vector const& a) const {
        auto lsqr = _mm_dp_ps(_value, _value, 0xff);
        auto dota = _mm_dp_ps(_value, a._value, 0xff);
        return _mm_mul_ps(_value, _mm_div_ps(dota, lsqr));
    }

    //! Return the rejection of `a` onto this vector.
    Vector Reject(Vector const& a) const {
        auto proj = Project(a)._value;
        return _mm_sub_ps(a._value, proj);
    }

    //! Return the reflection of `a` onto this vector.
    Vector Reflect(Vector const& a) const {
        auto scale = _mm_set_ps1(2.0f);
        auto proj = Project(a)._value;
        return _mm_fmsub_ps(proj, scale, a._value);
    }

private:
    __m128 _value;

private:
    friend class Matrix;

    Vector(__m128 const& value)
        : _value(value) {}
};

static_assert(alignof(Vector) == alignof(__m128), "Bad alignment!");

class Matrix {
public:
    Matrix() {}
    //! Construct with column vectors
    Matrix(Vector const& X, Vector const& Y, Vector const& Z, Vector const& W)
        : x(X), y(Y), z(Z), w(W) {}
    Matrix(float m11, float m12, float m13, float m14,
           float m21, float m22, float m23, float m24,
           float m31, float m32, float m33, float m34,
           float m41, float m42, float m43, float m44)
        : x(m11, m21, m31, m41)
        , y(m12, m22, m32, m42)
        , z(m13, m23, m33, m43)
        , w(m14, m24, m34, m44) {}

    bool operator==(Matrix const& a) const {
        return x == a.x && y == a.y && z == a.z && w == a.w;
    }

    bool operator!=(Matrix const& a) const {
        return x != a.x || y != a.y || z != a.z || w != a.w;
    }

    Matrix operator*(Scalar const& s) const {
        return Matrix(_mm_mul_ps(x._value, s._value),
                      _mm_mul_ps(y._value, s._value),
                      _mm_mul_ps(z._value, s._value),
                      _mm_mul_ps(w._value, s._value));
    }

    Matrix operator/(Scalar const& s) const {
        return Matrix(_mm_div_ps(x._value, s._value),
                      _mm_div_ps(y._value, s._value),
                      _mm_div_ps(z._value, s._value),
                      _mm_div_ps(w._value, s._value));
    }

    friend Matrix operator*(Scalar const& s, Matrix const& m) {
        return m * s;
    }

    Vector operator*(Vector const& v) const {
        auto rx = _mm_shuffle_ps(v._value, v._value, SHUFPS(0, 0, 0, 0));
        auto ry = _mm_shuffle_ps(v._value, v._value, SHUFPS(1, 1, 1, 1));
        auto rz = _mm_shuffle_ps(v._value, v._value, SHUFPS(2, 2, 2, 2));
        auto rw = _mm_shuffle_ps(v._value, v._value, SHUFPS(3, 3, 3, 3));

        auto r1 = _mm_mul_ps(x._value, rx);
        auto r2 = _mm_mul_ps(y._value, ry);
        auto r3 = _mm_mul_ps(z._value, rz);
        auto r4 = _mm_mul_ps(w._value, rw);

        auto r5 = _mm_add_ps(r1, r2);
        auto r6 = _mm_add_ps(r3, r4);
        return _mm_add_ps(r5, r6);
    }

    Matrix operator*(Matrix const& a) const {
        return Matrix((*this) * a.x,
                      (*this) * a.y,
                      (*this) * a.z,
                      (*this) * a.w);
    }

protected:
    Vector x, y, z, w;

protected:
    Matrix(__m128 const& X, __m128 const& Y, __m128 const& Z, __m128 const& W)
        : x(X), y(Y), z(Z), w(W) {}
};

static_assert(alignof(Matrix) == alignof(__m128), "Bad alignment!");

} // namespace intrinsic
