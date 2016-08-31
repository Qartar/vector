#pragma once

#include <cassert>
#include <cstdint>
#include <cmath>

namespace reference {

////////////////////////////////////////////////////////////////////////////////
/**
 */

using Scalar = float;

using std::abs;
using std::sqrt;
using std::size_t;

////////////////////////////////////////////////////////////////////////////////
/**
 */
class alignas(16) Vector {
public:
    Vector() {}
    Vector(float X, float Y, float Z, float W)
        : x(X), y(Y), z(Z), w(W) {}

    Scalar& operator[](size_t index) {
        return (&x)[index];
    }

    Scalar const& operator[](size_t index) const {
        return (&x)[index];
    }

    bool operator==(Vector const& a) const {
        return (x == a.x && y == a.y && z == a.z && w == a.w);
    }

    bool operator!=(Vector const& a) const {
        return (x != a.x || y != a.y || z != a.z || w != a.w);
    }

    Vector operator+(Vector const& a) const {
        return Vector(x + a.x, y + a.y, z + a.z, w + a.w);
    }

    Vector operator-(Vector const& a) const {
        return Vector(x - a.x, y - a.y, z - a.z, w - a.w);
    }

    Vector operator*(Scalar s) const {
        return Vector(x*s, y*s, z*s, w*s);
    }

    friend Vector operator*(Scalar s, Vector const& a) {
        return a * s;
    }

    Vector operator/(Scalar s) const {
        return Vector(x/s, y/s, z/s, w/s);
    }

    Vector operator-() const {
        return Vector(-x, -y, -z, -w);
    }

    Scalar Length() const {
        return std::sqrt(LengthSqr());
    }

    Scalar LengthFast() const {
        return std::sqrt(LengthSqr());
    }

    Scalar LengthSqr() const {
        return x * x + y * y + z * z + w * w;
    }

    Vector Normalize() const {
        return *this / Length();
    }

    Vector NormalizeFast() const {
        return *this / Length();
    }

    //! Dot product in R4.
    Scalar operator*(Vector const& a) const {
        return x * a.x + y * a.y + z * a.z + w * a.w;
    }

    //! Cross product in R3.
    Vector operator%(Vector const& a) const {
        assert(std::abs(w) == 0.0f && std::abs(a.w) == 0.0f);
        return Vector(y * a.z - z * a.y,
                      z * a.x - x * a.z,
                      x * a.y - y * a.x, 0.0f);
    }

    //! Return the projection of `a` onto this vector.
    Vector Project(Vector const& a) const {
        return (*this * a * *this) / LengthSqr();
    }

    //! Return the rejection of `a` onto this vector.
    Vector Reject(Vector const& a) const {
        return a - Project(a);
    }

    //! Return the reflection of `a` onto this vector.
    Vector Reflect(Vector const& a) const {
        return a - 2.0f * Project(a);
    }

    //! Return the component-wise product with `a`.
    Vector Hadamard(Vector const& a) const {
        return {x * a.x, y * a.y, z * a.z, w * a.w};
    }

protected:
    Scalar x, y, z, w;

protected:
    friend class Matrix;
};

////////////////////////////////////////////////////////////////////////////////
/**
 */
class alignas(16) Matrix {
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

    Vector& operator[](size_t index) {
        return (&x)[index];
    }

    Vector const& operator[](size_t index) const {
        return (&x)[index];
    }

    bool operator==(Matrix const& a) const {
        return x == a.x && y == a.y && z == a.z && w == a.w;
    }

    bool operator!=(Matrix const& a) const {
        return x != a.x || y != a.y || z != a.z || w != a.w;
    }

    Matrix operator+(Matrix const& a) const {
        return Matrix(x + a.x, y + a.y, z + a.z, w + a.w);
    }

    Matrix operator-(Matrix const& a) const {
        return Matrix(x - a.x, y - a.y, z - a.z, w - a.w);
    }

    Matrix operator*(Scalar s) const {
        return Matrix(x * s, y * s, z * s, w * s);
    }

    Matrix operator/(Scalar s) const {
        return Matrix(x / s, y / s, z / s, w / s);
    }

    friend Matrix operator*(Scalar s, Matrix const& m) {
        return m * s;
    }

    Vector operator*(Vector const& x) const {
        Vector v;

        for (size_t ii = 0; ii < 4; ++ii) {
            v[ii] = (*this)[0][ii] * x[0];
            for (size_t kk = 1; kk < 4; ++kk) {
                v[ii] += (*this)[kk][ii] * x[kk];
            }
        }
        return v;
    }

    Matrix operator*(Matrix const& a) const {
        Matrix m;

        for (size_t ii = 0; ii < 4; ++ii) {
            for (size_t jj = 0; jj < 4; ++jj) {
                m[jj][ii] = (*this)[0][ii] * a[jj][0];
                for (size_t kk = 1; kk < 4; ++kk) {
                    m[jj][ii] += (*this)[kk][ii] * a[jj][kk];
                }
            }
        }
        return m;
    }

    Matrix Transpose() const {
        return Matrix(x.x, x.y, x.z, x.w,
                      y.x, y.y, y.z, y.w,
                      z.x, z.y, z.z, z.w,
                      w.x, w.y, w.z, w.w);
    }

    //! Return the component-wise product with `a`.
    Matrix Hadamard(Matrix const& a) const {
        return Matrix(x.Hadamard(a.x),
                      y.Hadamard(a.y),
                      z.Hadamard(a.z),
                      w.Hadamard(a.w));
    }

protected:
    Vector x, y, z, w;
};

} // namespace reference
