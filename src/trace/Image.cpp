#include "Image.h"

#include <algorithm>
#include <Windows.h>


uint8_t ClampByte(float c) {
    return std::min<int>(255, int(c * 255.f));
}

uint8_t EncodeGamma(float c)
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

bool Image::Save(char const* filename) const
{
    std::vector<uint8_t> data(_width * _height * 3);
    for (size_t ii = 0; ii < _height; ++ii) {
        for (size_t jj = 0; jj < _width; ++jj) {
            data[(ii * _width + jj) * 3 + 0] = EncodeGamma((*this)[ii][jj].b);
            data[(ii * _width + jj) * 3 + 1] = EncodeGamma((*this)[ii][jj].g);
            data[(ii * _width + jj) * 3 + 2] = EncodeGamma((*this)[ii][jj].r);
        }
     }

    DWORD file_size = (DWORD )(sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + data.size());
    DWORD data_size = (DWORD )data.size();

    BITMAPFILEHEADER bmpfh = {
        0x4d42,//'BM',                       //  bfType
        file_size,                  //  bfSize
        0,                          //  bfReserved1
        0,                          //  bfReserved2
        file_size - data_size,      //  bfOffBits
    };

    BITMAPINFOHEADER bmpih = {
        sizeof(BITMAPINFOHEADER),   //  biSize
        (LONG )_width,              //  biWidth
        -(LONG )_height,            //  biHeight
        3,                          //  biPlanes
        24,                         //  biBitCount
        BI_RGB,                     //  biCompression
        data_size,                  //  biSizeImage
        72,                         //  biXPelsPerMeter
        72,                         //  biYPelsPerMeter
        0,                          //  biClrUsed
        0,                          //  biClrImportant
    };

    HANDLE hfile = CreateFileA(filename, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);

    WriteFile(hfile, (LPCVOID )&bmpfh, sizeof(bmpfh), NULL, NULL);
    WriteFile(hfile, (LPCVOID )&bmpih, sizeof(bmpih), NULL, NULL);
    WriteFile(hfile, (LPCVOID )data.data(), (DWORD )data.size(), NULL, NULL);

    CloseHandle(hfile);

    return true;
}
