#pragma once

#include "Features.h"
#include "Platform.h"

#include <cassert>

#include <xmmintrin.h>
#include <smmintrin.h>
#include <immintrin.h>

////////////////////////////////////////////////////////////////////////////////
/**
 * Following the conventions used in Intel documentation, register values are
 * written from most to least significant, i.e.
 *
 *             [127:96][95:64][63:32][31: 0]
 *  V[127:0] = {     w,     z,     y,     x}
 *
 * Intel refers to the scalars at [127:96] and [95:64] as the 'upper elements'
 * or 'high bits' and to the scalars at [63:32] and [31:0] as the 'lower
 * elements' or 'low bits'.
 */

//! Immediate byte argument for _mm_shuffle_ps(src0, src1)
//!     dst[127:0] = { src1[w], src1[z], src0[y], src0[x] }
#define SHUFPS(w, z, y, x)  ((w << 6) | (z << 4) | (y << 2) | (x << 0))

namespace intrinsic {

// Forward declarations
class Scalar;
class VectorScalar;
class Vector;
class Matrix;

#if !_HAS_SSE
#   error Intrinsics implementation requires at least SSE instruction set!
#endif

////////////////////////////////////////////////////////////////////////////////
/**
 */
class Scalar {
public:
    Scalar() {}
    Scalar(float X)
        : _value(_mm_set_ps1(X)) {}

    explicit operator float() const {
        return _mm_cvtss_f32(_value);
    }

    bool VECTORCALL operator==(Scalar const& a) const {
        return _mm_comieq_ss(_value, a._value) == 1;
    }

    bool VECTORCALL operator!=(Scalar const& a) const {
        return _mm_comineq_ss(_value, a._value) == 1;
    }

    bool VECTORCALL operator<(Scalar const& a) const {
        return _mm_comilt_ss(_value, a._value) == 1;
    }

    bool VECTORCALL operator>(Scalar const& a) const {
        return _mm_comigt_ss(_value, a._value) == 1;
    }

    bool VECTORCALL operator<=(Scalar const& a) const {
        return _mm_comile_ss(_value, a._value) == 1;
    }

    bool VECTORCALL operator>=(Scalar const& a) const {
        return _mm_comige_ss(_value, a._value) == 1;
    }

    friend bool VECTORCALL operator<(float a, Scalar const& b) {
        return _mm_comilt_ss(_mm_set_ss(a), b._value) == 1;
    }

    friend bool VECTORCALL operator>(float a, Scalar const& b) {
        return _mm_comigt_ss(_mm_set_ss(a), b._value) == 1;
    }

    friend bool VECTORCALL operator<=(float a, Scalar const& b) {
        return _mm_comile_ss(_mm_set_ss(a), b._value) == 1;
    }

    friend bool VECTORCALL operator>=(float a, Scalar const& b) {
        return _mm_comige_ss(_mm_set_ss(a), b._value) == 1;
    }

    Scalar VECTORCALL operator-() const {
        return _mm_sub_ps(_mm_setzero_ps(), _value);
    }

    Scalar VECTORCALL operator+(Scalar const& a) const {
        return _mm_add_ps(_value, a._value);
    }

    Scalar VECTORCALL operator-(Scalar const& a) const {
        return _mm_sub_ps(_value, a._value);
    }

    Scalar VECTORCALL operator*(Scalar const& a) const {
        return _mm_mul_ps(_value, a._value);
    }

    Scalar VECTORCALL operator/(Scalar const& a) const {
        return _mm_div_ps(_value, a._value);
    }

    friend Scalar VECTORCALL operator+(float a, Scalar const& b) {
        return _mm_add_ps(_mm_set_ps1(a), b._value);
    }

    friend Scalar VECTORCALL operator-(float a, Scalar const& b) {
        return _mm_sub_ps(_mm_set_ps1(a), b._value);
    }

    friend Scalar VECTORCALL operator*(float a, Scalar const& b) {
        return _mm_mul_ps(_mm_set_ps1(a), b._value);
    }

    friend Scalar VECTORCALL operator/(float a, Scalar const& b) {
        return _mm_div_ps(_mm_set_ps1(a), b._value);
    }

    friend Scalar VECTORCALL abs(Scalar const& a) {
        return _mm_andnot_ps(_mm_set_ps1(-0.f), a._value);
    }

    friend Scalar VECTORCALL sqrt(Scalar const& a) {
        return _mm_sqrt_ps(a._value);
    }

    friend Scalar VECTORCALL exp(Scalar const& a) {
        return std::exp(_mm_cvtss_f32(a._value));
    }

    friend Scalar VECTORCALL pow(Scalar const& a, Scalar const& b) {
        return std::pow(_mm_cvtss_f32(a._value), _mm_cvtss_f32(b._value));
    }

private:
    __m128 _value;

private:
    friend VectorScalar;
    friend Vector;
    friend Matrix;

    Scalar(__m128 const& value)
        : _value(value)
    {
#if defined(_DEBUG)
        // Verify that each element of the register contains the same value.
        auto r1 = _mm_shuffle_ps(value, value, SHUFPS(0, 3, 2, 1));
        auto r2 = _mm_cmpeq_ps(r1, value);
        assert(_mm_movemask_ps(r2) == 0xf && "Invalid scalar value!");
#endif
    }
};

////////////////////////////////////////////////////////////////////////////////
/**
 */
class VectorScalar {
public:
    bool VECTORCALL operator==(Scalar const& s) const {
        return s == *this;
    }

    bool VECTORCALL operator!=(Scalar const& s) const {
        return s != *this;
    }

    bool VECTORCALL operator<(Scalar const& s) const {
        return s > *this;
    }

    bool VECTORCALL operator>(Scalar const& s) const {
        return s < *this;
    }

    bool VECTORCALL operator<=(Scalar const& s) const {
        return s >= *this;
    }

    bool VECTORCALL operator>=(Scalar const& s) const {
        return s <= *this;
    }

    VECTORCALL operator Scalar() const {
        switch (_index) {
            case 0: return _mm_shuffle_ps(_value, _value, SHUFPS(0, 0, 0, 0));
            case 1: return _mm_shuffle_ps(_value, _value, SHUFPS(1, 1, 1, 1));
            case 2: return _mm_shuffle_ps(_value, _value, SHUFPS(2, 2, 2, 2));
            case 3: return _mm_shuffle_ps(_value, _value, SHUFPS(3, 3, 3, 3));
            default: UNREACHABLE;
        }
    }

    void VECTORCALL operator=(Scalar const& a) {
        switch (_index) {
            case 0: return AssignX::op(_value, a._value);
            case 1: return AssignY::op(_value, a._value);
            case 2: return AssignZ::op(_value, a._value);
            case 3: return AssignW::op(_value, a._value);
            default: UNREACHABLE;
        }
    }

private:
    friend Vector;

    VectorScalar(__m128& value, size_t index)
        : _value(value)
        , _index(index) {}

    struct AssignX {
        static void VECTORCALL op(__m128& v, __m128 const& s) {
            //  y       x       s       s
            auto r1 = _mm_movelh_ps(s, v);
            //  w       z       y       s
            v = _mm_shuffle_ps(r1, v, SHUFPS(3, 2, 3, 0));
        }
    };

    struct AssignY {
        static void VECTORCALL op(__m128& v, __m128 const& s) {
            //  y       x       s       s
            auto r1 = _mm_movelh_ps(s, v);
            //  w       z       s       x
            v = _mm_shuffle_ps(r1, v, SHUFPS(3, 2, 1, 2));
        }
    };

    struct AssignZ {
        static void VECTORCALL op(__m128& v, __m128 const& s) {
            //  s       s       w       z
            auto r1 = _mm_movehl_ps(s, v);
            //  w       s       y       x
            v = _mm_shuffle_ps(v, r1, SHUFPS(1, 2, 1, 0));
        }
    };

    struct AssignW {
        static void VECTORCALL op(__m128& v, __m128 const& s) {
            //  s       s       w       z
            auto r1 = _mm_movehl_ps(s, v);
            //  s       z       y       x
            v = _mm_shuffle_ps(v, r1, SHUFPS(3, 0, 1, 0));
        }
    };

private:
    __m128& _value;
    size_t _index;
};

////////////////////////////////////////////////////////////////////////////////
/**
 */
class Vector {
public:
    Vector() {}
    Vector(float X, float Y, float Z, float W)
        : _value(_mm_set_ps(W, Z, Y, X)) {}

    VectorScalar VECTORCALL operator[](size_t index) {
        return VectorScalar(_value, index);
    }

    Scalar VECTORCALL operator[](size_t index) const {
        switch (index) {
            case 0: return _mm_shuffle_ps(_value, _value, SHUFPS(0, 0, 0, 0));
            case 1: return _mm_shuffle_ps(_value, _value, SHUFPS(1, 1, 1, 1));
            case 2: return _mm_shuffle_ps(_value, _value, SHUFPS(2, 2, 2, 2));
            case 3: return _mm_shuffle_ps(_value, _value, SHUFPS(3, 3, 3, 3));
            default: UNREACHABLE;
        }
    }

    bool VECTORCALL operator==(Vector const& a) const {
        return _mm_movemask_ps(_mm_cmpeq_ps(_value, a._value)) == 0xf;
    }

    bool VECTORCALL operator!=(Vector const& a) const {
        return _mm_movemask_ps(_mm_cmpneq_ps(_value, a._value)) != 0x0;
    }

    Vector VECTORCALL operator+(Vector const& a) const {
        return _mm_add_ps(_value, a._value);
    }

    Vector VECTORCALL operator-(Vector const& a) const {
        return _mm_sub_ps(_value, a._value);
    }

    Vector VECTORCALL operator*(Scalar const& s) const {
        return _mm_mul_ps(_value, s._value);
    }

    friend Vector VECTORCALL operator*(Scalar const& s, Vector const& a) {
        return a * s;
    }

    Vector VECTORCALL operator/(Scalar const& s) const {
        return _mm_div_ps(_value, s._value);
    }

    Vector VECTORCALL operator-() const {
        return _mm_sub_ps(_mm_setzero_ps(), _value);
    }

    Scalar VECTORCALL Length() const {
        return _mm_sqrt_ps(_vec_dp_ps(_value, _value));
    }

    Scalar VECTORCALL LengthFast() const {
        auto lsqr = _vec_dp_ps(_value, _value);
        return _mm_mul_ps(lsqr, _mm_rsqrt_ps(lsqr));
    }

    Scalar VECTORCALL LengthSqr() const {
        return _vec_dp_ps(_value, _value);
    }

    Vector VECTORCALL Normalize() const {
        return _mm_div_ps(_value, _mm_sqrt_ps(_vec_dp_ps(_value, _value)));
    }

    Vector VECTORCALL NormalizeFast() const {
        return _mm_mul_ps(_value, _mm_rsqrt_ps(_vec_dp_ps(_value, _value)));
    }

    //! Dot product in R4.
    Scalar VECTORCALL operator*(Vector const& a) const {
        return _vec_dp_ps(_value, a._value);
    }

    //! Cross product in R3.
    Vector VECTORCALL operator%(Vector const& a) const {
        //  y3      y1      y0      y2
        auto shuf1 = _mm_shuffle_ps(_value, _value, SHUFPS(3, 1, 0, 2));
        //  y3      y0      y2      y1
        auto shuf2 = _mm_shuffle_ps(_value, _value, SHUFPS(3, 0, 2, 1));

        //  --      y1*z2   y0*z1   y2*z0
        auto prod1 = _mm_mul_ps(shuf1, a._value);
        //  --      y0*z2   y2*z1   y1*z0
        auto prod2 = _mm_mul_ps(shuf2, a._value);

        //  --      y0*z1   y2*z0   y1*z2
        auto shuf3 = _mm_shuffle_ps(prod1, prod1, SHUFPS(3, 1, 0, 2));
        //  --      y1*z0   y0*z2   y2*z1
        auto shuf4 = _mm_shuffle_ps(prod2, prod2, SHUFPS(3, 0, 2, 1));

        return _mm_sub_ps(shuf3, shuf4);
    }

    //! Return the projection of `a` onto this vector.
    Vector VECTORCALL Project(Vector const& a) const {
        auto lsqr = _vec_dp_ps(_value, _value);
        auto dota = _vec_dp_ps(_value, a._value);
        return _mm_mul_ps(_value, _mm_div_ps(dota, lsqr));
    }

    //! Return the rejection of `a` onto this vector.
    Vector VECTORCALL Reject(Vector const& a) const {
        auto proj = Project(a)._value;
        return _mm_sub_ps(a._value, proj);
    }

    //! Return the reflection of `a` onto this vector.
    Vector VECTORCALL Reflect(Vector const& a) const {
        auto proj = Project(a)._value;
#if _HAS_FMA
        auto scale = _mm_set_ps1(-2.0f);
        return _mm_fmadd_ps(proj, scale, a._value);
#else
        auto r1 = _mm_mul_ps(_mm_set_ps1(2.0f, proj));
        return _mm_sub_ps(a._value, r1);
#endif
    }

    //! Return the component-wise product with `a`.
    Vector VECTORCALL Scale(Vector const& a) const {
        return _mm_mul_ps(_value, a._value);
    }

private:
    __m128 _value;

private:
    friend Matrix;

    Vector(__m128 const& value)
        : _value(value) {}

    //! Dot product of `a` and `b`. Returns the scalar value broadcasted to each
    //! register element. Multiple implementations based on platform features.
    static __m128 VECTORCALL _vec_dp_ps(__m128 a, __m128 b) {
#if _HAS_SSE4_1
        return _mm_dp_ps(a, b, 0xff);
#elif _HAS_SSE3
        //  w       z       y       x
        auto r1 = _mm_mul_ps(a, b);
        //  z+w     x+y     z+w     x+y
        auto r2 = _mm_hadd_ps(r1, r1);

        return _mm_hadd_ps(r2, r2);
#else
        //  w       z       y       x
        auto r1 = _mm_mul_ps(a, b);
        //  z       y       x       w
        auto r2 = _mm_shuffle_ps(r1, r1, SHUFPS(2, 1, 0, 3));
        //  z+w     y+z     x+y     w+x
        auto r3 = _mm_add_ps(r1, r2);
        //  x+y     w+x     z+w     y+z
        auto r4 = _mm_shuffle_ps(r3, r3, SHUFPS(1, 0, 3, 2));

        return _mm_add_ps(r3, r4);
#endif
    }
};

static_assert(alignof(Vector) == alignof(__m128), "Bad alignment!");

////////////////////////////////////////////////////////////////////////////////
/**
 */
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

    Vector& VECTORCALL operator[](size_t index) {
        return (&x)[index];
    }

    Vector const& VECTORCALL operator[](size_t index) const {
        return (&x)[index];
    }

    bool VECTORCALL operator==(Matrix const& a) const {
        return x == a.x && y == a.y && z == a.z && w == a.w;
    }

    bool VECTORCALL operator!=(Matrix const& a) const {
        return x != a.x || y != a.y || z != a.z || w != a.w;
    }

    Matrix VECTORCALL operator*(Scalar const& s) const {
        return Matrix(_mm_mul_ps(x._value, s._value),
                      _mm_mul_ps(y._value, s._value),
                      _mm_mul_ps(z._value, s._value),
                      _mm_mul_ps(w._value, s._value));
    }

    Matrix VECTORCALL operator/(Scalar const& s) const {
        return Matrix(_mm_div_ps(x._value, s._value),
                      _mm_div_ps(y._value, s._value),
                      _mm_div_ps(z._value, s._value),
                      _mm_div_ps(w._value, s._value));
    }

    friend Matrix VECTORCALL operator*(Scalar const& s, Matrix const& m) {
        return m * s;
    }

    Vector VECTORCALL operator*(Vector const& v) const {
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

    Matrix VECTORCALL operator*(Matrix const& a) const {
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
