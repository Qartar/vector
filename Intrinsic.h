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
        return _mm_andnot_ps(a._value, _mm_set_ps1(-0.f));
    }

    friend Scalar sqrt(Scalar const& a) {
        return _mm_sqrt_ps(a._value);
    }

private:
    __m128 _value;

private:
    friend class Vector;

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
        return _mm_movemask_ps(_mm_cmpneq_ps(_value, a._value)) == 0xf;
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
    Vector(__m128 const& value)
        : _value(value) {}
};

static_assert(alignof(Vector) == alignof(__m128), "Bad alignment!");

} // namespace intrinsic
