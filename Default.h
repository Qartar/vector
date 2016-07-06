#pragma once

#include <cassert>
#include <cmath>

namespace default {

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

    Vector operator*(float s) const {
        return Vector(x*s, y*s, z*s, w*s);
    }

    friend Vector operator*(float s, Vector const& a) {
        return a * s;
    }

    Vector operator/(float s) const {
        return Vector(x/s, y/s, z/s, w/s);
    }

    Vector operator-() const {
        return Vector(-x, -y, -z, -w);
    }

    float Length() const {
        return std::sqrt(LengthSqr());
    }

    float LengthFast() const {
        return std::sqrt(LengthSqr());
    }

    float LengthSqr() const {
        return x * x + y * y + z * z + w * w;
    }

    Vector Normalize() const {
        return *this / Length();
    }

    Vector NormalizeFast() const {
        return *this / Length();
    }

    //! Dot product in R4.
    float operator*(Vector const& a) const {
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

} // namespace default
