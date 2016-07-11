#include "Image.h"

#include <Windows.h>

bool WriteBitmapBGR(char const* filename, size_t width, size_t height, std::vector<uint8_t> const& pixels)
{
    DWORD file_size = (DWORD )(sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + pixels.size());
    DWORD data_size = (DWORD )pixels.size();

    BITMAPFILEHEADER bmpfh = {
        0x4d42,//'BM',                       //  bfType
        file_size,                  //  bfSize
        0,                          //  bfReserved1
        0,                          //  bfReserved2
        file_size - data_size,      //  bfOffBits
    };

    BITMAPINFOHEADER bmpih = {
        sizeof(BITMAPINFOHEADER),   //  biSize
        (LONG )width,               //  biWidth
        -(LONG )height,             //  biHeight
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
    WriteFile(hfile, (LPCVOID )pixels.data(), (DWORD )pixels.size(), NULL, NULL);

    CloseHandle(hfile);
    return true;
}
