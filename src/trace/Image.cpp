#include "Image.h"

#include <algorithm>
#include <Windows.h>

bool Image::Save(char const* filename) const
{
    std::vector<uint8_t> data(_width * _height * 3);
    for (size_t ii = 0; ii < _height; ++ii) {
        for (size_t jj = 0; jj < _width; ++jj) {
            data[(ii * _width + jj) * 3 + 0] = std::min<int>(255, int((*this)[ii][jj].b * 255.0f));
            data[(ii * _width + jj) * 3 + 1] = std::min<int>(255, int((*this)[ii][jj].g * 255.0f));
            data[(ii * _width + jj) * 3 + 2] = std::min<int>(255, int((*this)[ii][jj].r * 255.0f));
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
