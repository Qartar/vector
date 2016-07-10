#pragma once

#include <vector>

#include "Color.h"

class Image {
public:
    Image(size_t width, size_t height)
        : _width(width)
        , _height(height) {
        _data.resize(width * height);
    }

    size_t Width() const {
        return _width;
    }

    size_t Height() const {
        return _height;
    }

    Color* operator[](size_t row) {
        return &(_data[row * _width]);
    }

    Color const* operator[](size_t row) const {
        return &(_data[row * _width]);
    }

    bool Save(char const* filename) const;

private:
    size_t  _width;
    size_t  _height;

    std::vector<Color>   _data;
};
