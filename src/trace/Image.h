#pragma once

#include <algorithm>
#include <vector>

#include "Color.h"
#include "Platform.h"

bool WriteBitmapBGR(char const* filename, size_t width, size_t height, std::vector<uint8_t> const& pixels);

inline uint8_t ClampByte(float c)
{
    return std::min<int>(255, int(c * 255.f));
}

inline uint8_t EncodeGamma(float c)
{
    //      {  0.0,                          0         <= cl
    //      {  12.92 * c,                    0         <  cl < 0.0031308
    // cs = {  1.055 * cl^0.41666 - 0.055,   0.0031308 <= cl < 1
    //      {  1.0,                                       cl >= 1

    if (c < 0.f) {
        return 0;
    } else if (c < .0031308f) {
        return ClampByte(12.92f * c);
    } else if (c < 1.f) {
        return ClampByte(1.055f * std::pow(c, 0.41666f) - 0.055f);
    } else {
        return 255;
    }
}

template<typename M, typename V, typename S>
class Image {
public:
    using Color = ::Color<M, V, S>;

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

    bool Save(char const* filename) const {
        printf_s("saving %s...", filename);
        std::vector<uint8_t> data(_width * _height * 3);
        for (size_t ii = 0; ii < _height; ++ii) {
            for (size_t jj = 0; jj < _width; ++jj) {
                data[(ii * _width + jj) * 3 + 0] = EncodeGamma((float)(*this)[ii][jj][2]);
                data[(ii * _width + jj) * 3 + 1] = EncodeGamma((float)(*this)[ii][jj][1]);
                data[(ii * _width + jj) * 3 + 2] = EncodeGamma((float)(*this)[ii][jj][0]);
            }
        }

        auto result =  WriteBitmapBGR(filename, _width, _height, data);
        printf_s("%s\n", result ? "ok" : "failed");
        return result;
    }

private:
    size_t  _width;
    size_t  _height;

    std::vector<Color>   _data;
};
