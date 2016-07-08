#pragma once

#include "Default.h"
#include <cassert>
#include <cmath>
#include <xmmintrin.h>

namespace aligned {

using Scalar = float;

using std::abs;
using std::sqrt;

class alignas(__m128) Vector {
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

protected:
    float x, y, z, w;
};

static_assert(alignof(Vector) == alignof(__m128), "Bad alignment!");

} // namespace aligned
