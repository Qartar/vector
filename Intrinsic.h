#pragma once

#include <xmmintrin.h>
#include <smmintrin.h>

//! dst[0->3] = { src0[x], src0[y], src1[z], src1[w] }
#define SHUFPS(x, y, z, w)  ((x << 0) | (y << 2) | (z << 4) | (w << 6))

namespace intrinsic {

using Scalar = float;

using std::abs;
using std::sqrt;

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

    Vector operator*(Scalar s) const {
        return _mm_mul_ps(_value, _mm_set_ps1(s));
    }

    friend Vector operator*(Scalar s, Vector const& a) {
        return a * s;
    }

    Vector operator/(Scalar s) const {
        return _mm_mul_ps(_value, _mm_set_ps1(1.0f / s));
    }

    Vector operator-() const {
        return _mm_sub_ps(_mm_setzero_ps(), _value);
    }

    Scalar Length() const {
        return _mm_cvtss_f32(_mm_sqrt_ss(_mm_dp_ps(_value, _value, 0xf1)));
    }

    Scalar LengthFast() const {
        auto lsqr = _mm_dp_ps(_value, _value, 0xf1);
        return _mm_cvtss_f32(_mm_mul_ss(lsqr, _mm_rsqrt_ss(lsqr)));
    }

    Scalar LengthSqr() const {
        return _mm_cvtss_f32(_mm_dp_ps(_value, _value, 0xf1));
    }

    Vector Normalize() const {
        return _mm_div_ps(_value, _mm_sqrt_ps(_mm_dp_ps(_value, _value, 0xff)));
    }

    Vector NormalizeFast() const {
        return _mm_mul_ps(_value, _mm_rsqrt_ps(_mm_dp_ps(_value, _value, 0xff)));
    }

    //! Dot product in R4.
    Scalar operator*(Vector const& a) const {
        return _mm_cvtss_f32(_mm_dp_ps(_value, a._value, 0xf1));
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

private:
    __m128 _value;

private:
    Vector(__m128 value)
        : _value(value) {}
};

static_assert(alignof(Vector) == alignof(__m128), "Bad alignment!");

} // namespace intrinsic
