//////////////////////////////////////////////////////////////////////////////////////
// Author: Peng Hao <635945005@qq.com>
// License: GPL
//////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////
// @file: BMP.h
// @brief: A head file that defined some structure for BMP.
//////////////////////////////////////////////////////////////////////////////////////

#pragma once
typedef unsigned char		BYTE;
typedef unsigned short      WORD;
typedef unsigned int		DWORD;
typedef int32_t				LONG;

struct BITMAPFILEHEADER {
        WORD    bfType;
        WORD	bfSize[2];
        WORD    bfReserved1;
        WORD    bfReserved2;
        WORD	bfOffBits[2];
};

struct RGBQUAD {
        BYTE    rgbBlue;
        BYTE    rgbGreen;
        BYTE    rgbRed;
        BYTE    rgbReserved;
};

struct BITMAPINFOHEADER{
		DWORD      biSize;
        LONG       biWidth;
        LONG       biHeight;
        WORD       biPlanes;
        WORD       biBitCount;
        DWORD      biCompression;
        DWORD      biSizeImage;
        LONG       biXPelsPerMeter;
        LONG       biYPelsPerMeter;
        DWORD      biClrUsed;
        DWORD      biClrImportant;
};
