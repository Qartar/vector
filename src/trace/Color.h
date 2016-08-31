#pragma once

template<typename M, typename V, typename S>
class Color {
public:
    Color() {}
    Color(float r, float g, float b, float a)
        : _value(r, g, b, a) {}

    using Elem = decltype((*((V*)0))[0u]);
    using ConstElem = decltype((*((V const*)0))[0u]);

    Elem operator[](size_t index) {
        return _value[index];
    }

    ConstElem operator[](size_t index) const {
        return _value[index];
    }

    Color& operator+=(Color const& c) {
        *this = *this + c;
        return *this;
    }

    Color operator+(Color const& c) const {
        return _value + c._value;
    }

    Color operator*(Color const& c) const {
        return _value.Hadamard(c._value);
    }

    Color operator*(S const& s) const {
        return _value * s;
    }

    friend Color operator*(S const& s, Color const& c) {
        return c._value * s;
    }

private:
    V _value;

private:
    Color(V const& v) 
        : _value(v) {}
};
