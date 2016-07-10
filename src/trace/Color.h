#pragma once

struct alignas(16) Color {
    float r, g, b, a;

    float& operator[](size_t index) {
        return (&r)[index];
    }

    float const& operator[](size_t index) const {
        return (&r)[index];
    }

    Color& operator+=(Color const& c) {
        r += c.r;
        g += c.g;
        b += c.b;
        a += c.a;
        return *this;
    }

    Color operator+(Color const& c) const {
        return {r + c.r, g + c.g, b + c.b, a + c.a};
    }

    Color operator*(Color const& c) const {
        return {r * c.r, g * c.g, b * c.b, a * c.a};
    }

    template<typename S>
    Color operator*(S const& s) const {
        auto k = s.ToFloat();
        return {r * k, g * k, b * k, a * k};
    }

    template<>
    Color operator*(float const& s) const {
        return {r * s, g * s, b * s, a * s};
    }
};
