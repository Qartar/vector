#pragma once

#include <cassert>
#include <cmath>

namespace default {

using Scalar = float;

using std::abs;
using std::sqrt;

class Vector {
public:
    Vector() {}
    Vector(float X, float Y, float Z, float W)
        : x(X), y(Y), z(Z), w(W) {}

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

protected:
    float x, y, z, w;
};

} // namespace default
