//////////////////////////////////////////////////////////////////////////////////////
// Author: Peng Hao <635945005@qq.com>
// License: GPL
//////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////
// @file: Utils.cpp
// @brief: Common fuction source file.
//////////////////////////////////////////////////////////////////////////////////////

#include "Utils.h"

using namespace std;

void DumpImgDataAsText(void* data, int32_t height, int32_t width, size_t bitWidth, string TextPath) {
	if (data != nullptr) {
		ofstream OutFile(TextPath);
		for (int32_t i = 0; i < height; i++) {
			OutFile << i << ": ";
			for (int32_t j = 0; j < width; j++) {
				switch (bitWidth) {
				case sizeof(uint16_t):
					OutFile << static_cast<uint16_t*>(data)[i * width + j] << ' ';
					break;
				case sizeof(uint8_t):
					OutFile << static_cast<uint8_t*>(data)[i * width + j] << ' ';
					break;
				default:
					cout << "Dump failed. Unsopported data type." << endl;
				}
			}
			OutFile << endl;
		}
		OutFile.close();
		cout << " Data saved as TXT finished " << endl;
	}
	else {
		cout << " Dump failed. data is nullptr " << endl;
	}
}
