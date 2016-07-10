#pragma once

struct Color {
    float r, g, b, a;
    float& operator[](size_t index) {
        return (&r)[index];
    }
    float const& operator[](size_t index) const {
        return (&r)[index];
    }
};
