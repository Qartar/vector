#pragma once

#include "Features.h"
#include "Platform.h"

#include "Intrinsic.h"

#include <cassert>
#include <cmath>

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

namespace aliased {

////////////////////////////////////////////////////////////////////////////////

// Forward declarations
class Vector;
class Matrix;

using intrinsic::_v_shuffle_ps;
using intrinsic::_v_blend_ps;
using intrinsic::_v_dp_ps;

using Scalar = float;

using std::abs;
using std::sqrt;
using std::size_t;

////////////////////////////////////////////////////////////////////////////////
/**
 */
class Vector {
public:
    Vector() {}
    Vector(float X, float Y, float Z, float W)
        : _value(_mm_set_ps(W, Z, Y, X)) {}

    Scalar& VECTORCALL operator[](size_t index) {
        return (&x)[index];
    }

    Scalar const& VECTORCALL operator[](size_t index) const {
        return (&x)[index];
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

    Vector VECTORCALL operator*(Scalar s) const {
        return _mm_mul_ps(_value, _mm_set_ps1(s));
    }

    friend Vector VECTORCALL operator*(Scalar s, Vector const& a) {
        return a * s;
    }

    Vector VECTORCALL operator/(Scalar s) const {
        return _mm_div_ps(_value, _mm_set_ps1(s));
    }

    Vector VECTORCALL operator-() const {
        return _mm_sub_ps(_mm_setzero_ps(), _value);
    }

    Scalar VECTORCALL Length() const {
        return _mm_cvtss_f32(_mm_sqrt_ps(_v_dp_ps(_value, _value)));
    }

    Scalar VECTORCALL LengthFast() const {
        auto lsqr = _v_dp_ps(_value, _value);
        return _mm_cvtss_f32(_mm_mul_ps(lsqr, _mm_rsqrt_ps(lsqr)));
    }

    Scalar VECTORCALL LengthSqr() const {
        return _mm_cvtss_f32(_v_dp_ps(_value, _value));
    }

    Vector VECTORCALL Normalize() const {
        return _mm_div_ps(_value, _mm_sqrt_ps(_v_dp_ps(_value, _value)));
    }

    Vector VECTORCALL NormalizeFast() const {
        return _mm_mul_ps(_value, _mm_rsqrt_ps(_v_dp_ps(_value, _value)));
    }

    //! Dot product in R4.
    Scalar VECTORCALL operator*(Vector const& a) const {
        return _mm_cvtss_f32(_v_dp_ps(_value, a._value));
    }

    //! Cross product in R3.
    Vector VECTORCALL operator%(Vector const& a) const {
        //  y3      y1      y0      y2
        auto shuf1 = _v_shuffle_ps<3, 1, 0, 2>(_value, _value);
        //  y3      y0      y2      y1
        auto shuf2 = _v_shuffle_ps<3, 0, 2, 1>(_value, _value);

        //  --      y1*z2   y0*z1   y2*z0
        auto prod1 = _mm_mul_ps(shuf1, a._value);
        //  --      y0*z2   y2*z1   y1*z0
        auto prod2 = _mm_mul_ps(shuf2, a._value);

        //  --      y0*z1   y2*z0   y1*z2
        auto shuf3 = _v_shuffle_ps<3, 1, 0, 2>(prod1, prod1);
        //  --      y1*z0   y0*z2   y2*z1
        auto shuf4 = _v_shuffle_ps<3, 0, 2, 1>(prod2, prod2);

        return _mm_sub_ps(shuf3, shuf4);
    }

    //! Return the projection of `a` onto this vector.
    Vector VECTORCALL Project(Vector const& a) const {
        auto lsqr = _v_dp_ps(_value, _value);
        auto dota = _v_dp_ps(_value, a._value);
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
        auto r1 = _mm_mul_ps(_mm_set_ps1(2.0f), proj);
        return _mm_sub_ps(a._value, r1);
#endif
    }

    //! Return the component-wise product with `a`.
    Vector VECTORCALL Hadamard(Vector const& a) const {
        return _mm_mul_ps(_value, a._value);
    }

private:

#if defined(_MSC_VER)
#pragma warning(push)
// MSVC complains that nameless struct/union is a nonstandard
// extension even though virtually every compiler supports it.
#pragma warning(disable: 4201)
#endif // defined(_MSC_VER)

    union {
        __m128 _value;
        struct {
            float x, y, z, w;
        };
    };

#if defined(_MSC_VER)
#pragma warning(pop)
#endif // defined(_MSC_VER)

private:
    friend Matrix;

    Vector(__m128 const& value)
        : _value(value) {}
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

    Matrix VECTORCALL operator+(Matrix const& a) const {
        return Matrix(_mm_add_ps(x._value, a.x._value),
                      _mm_add_ps(y._value, a.y._value),
                      _mm_add_ps(z._value, a.z._value),
                      _mm_add_ps(w._value, a.w._value));
    }

    Matrix VECTORCALL operator-(Matrix const& a) const {
        return Matrix(_mm_sub_ps(x._value, a.x._value),
                      _mm_sub_ps(y._value, a.y._value),
                      _mm_sub_ps(z._value, a.z._value),
                      _mm_sub_ps(w._value, a.w._value));
    }

    Matrix VECTORCALL operator*(Scalar s) const {
        return Matrix(_mm_mul_ps(x._value, _mm_set_ps1(s)),
                      _mm_mul_ps(y._value, _mm_set_ps1(s)),
                      _mm_mul_ps(z._value, _mm_set_ps1(s)),
                      _mm_mul_ps(w._value, _mm_set_ps1(s)));
    }

    Matrix VECTORCALL operator/(Scalar s) const {
        return Matrix(_mm_div_ps(x._value, _mm_set_ps1(s)),
                      _mm_div_ps(y._value, _mm_set_ps1(s)),
                      _mm_div_ps(z._value, _mm_set_ps1(s)),
                      _mm_div_ps(w._value, _mm_set_ps1(s)));
    }

    friend Matrix VECTORCALL operator*(Scalar s, Matrix const& m) {
        return m * s;
    }

    Vector VECTORCALL operator*(Vector const& v) const {
        auto rx = _v_shuffle_ps<0, 0, 0, 0>(v._value, v._value);
        auto ry = _v_shuffle_ps<1, 1, 1, 1>(v._value, v._value);
        auto rz = _v_shuffle_ps<2, 2, 2, 2>(v._value, v._value);
        auto rw = _v_shuffle_ps<3, 3, 3, 3>(v._value, v._value);

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

    Matrix VECTORCALL Transpose() const {
        //  m41     m31     m21     m11
        //  m42     m32     m22     m12
        //  m43     m33     m23     m13
        //  m44     m34     m24     m14

        //  m22     m21     m12     m11
        auto r0 = _mm_unpacklo_ps(x._value, y._value);
        //  m24     m23     m14     m13
        auto r2 = _mm_unpacklo_ps(z._value, w._value);
        //  m42     m41     m32     m31
        auto r1 = _mm_unpackhi_ps(x._value, y._value);
        //  m44     m43     m34     m33
        auto r3 = _mm_unpackhi_ps(z._value, w._value);

        return Matrix(
            //  m14     m13     m12     m11
            _mm_movelh_ps(r0, r2),
            //  m24     m23     m22     m21
            _mm_movehl_ps(r2, r0),
            //  m34     m33     m32     m31
            _mm_movelh_ps(r1, r3),
            //  m44     m43     m42     m41
            _mm_movehl_ps(r3, r1)
        );
    }

    //! Return the component-wise product with `a`.
    Matrix VECTORCALL Hadamard(Matrix const& a) const {
        return Matrix(_mm_mul_ps(x._value, a.x._value),
                      _mm_mul_ps(y._value, a.y._value),
                      _mm_mul_ps(z._value, a.z._value),
                      _mm_mul_ps(w._value, a.w._value));
    }

protected:
    Vector x, y, z, w;

protected:
    Matrix(__m128 const& X, __m128 const& Y, __m128 const& Z, __m128 const& W)
        : x(X), y(Y), z(Z), w(W) {}
};

static_assert(alignof(Matrix) == alignof(__m128), "Bad alignment!");

} // namespace aliased
