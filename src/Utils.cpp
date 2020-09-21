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

void intDataSaveAsText(int* data, int height, int width, string TextPath) {
	int i, j;
	ofstream OutFile(TextPath);
	for (i = 0; i < height; i++) {
		OutFile << i << ": ";
		for (j = 0; j < width; j++) {
			OutFile << data[i * width + j] << ' ';
		}
		OutFile << endl;
	}
	OutFile.close();
	cout << " Data saved as TXT finished " << endl;
}

void CharDataSaveAsText(unsigned char* data, int height, int width, string TextPath) {
	int i, j;
	ofstream OutFile(TextPath);
	for (i = 0; i < height; i++) {
		OutFile << i << ": ";
		for (j = 0; j < width; j++) {
			OutFile << (int)data[i * width + j] << ' ';
		}
		OutFile << endl;
	}
	OutFile.close();
	cout << " Data saved as TXT finished " << endl;
}