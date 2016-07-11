#pragma once

template<typename M, typename V, typename S>
class Color {
public:
    Color() {}
    Color(float r, float g, float b, float a)
        : _value(r, g, b, a) {}

    auto operator[](size_t index) {
        return _value[index];
    }

    auto operator[](size_t index) const {
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
        return _value.Scale(c._value);
    }

    Color operator*(S const& s) const {
        return _value * s;
    }

private:
    V _value;

private:
    Color(V const& v) 
        : _value(v) {}
};
