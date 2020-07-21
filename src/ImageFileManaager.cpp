//////////////////////////////////////////////////////////////////////////////////////
// Author: Peng Hao
// License: GPL
//////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////
// @file: ImageFileManager.cpp
// @brief A file manager to manage image file, provoid file operations
//////////////////////////////////////////////////////////////////////////////////////

#include <fstream> 
#include <iostream>

#include "ImageFileManager.h"

using namespace std;

ISPState ImageFileManager::Init()
{
	this->mInputImg.pInputPath = nullptr;
	this->mInputImg.rawSize = 640 * 480;
	this->mOutputImg.pOutputPath = nullptr;
	this->mOutputImg.width = 640;
	this->mOutputImg.hight = 480;
	return Inited;
}

ISPState ImageFileManager::Deinit()
{
	this->mInputImg.pInputPath = nullptr;
	this->mInputImg.rawSize = 640 * 480; 
	this->mOutputImg.pOutputPath = nullptr;
	this->mOutputImg.width = 640;
	this->mOutputImg.hight = 480;
	return Uninited;
}

ISPResult ImageFileManager::SetInputImgInfo(InputImgInfo info)
{
	ISPResult result = ISPSuccess;
	this->mInputImg.pInputPath = info.pInputPath;
	this->mInputImg.rawSize = info.rawSize;
	return result;
}

ISPResult ImageFileManager::SetInputImgInfo(char* path, unsigned int rawSize)
{
	ISPResult result = ISPSuccess;
	this->mInputImg.pInputPath = path;
	this->mInputImg.rawSize = rawSize;
	return result;
}

ISPResult ImageFileManager::SetInputImgInfo(char* path)
{
	ISPResult result = ISPSuccess;
	this->mInputImg.pInputPath = path;
	return result;
}

ISPResult ImageFileManager::SetInputImgInfo(unsigned int rawSize)
{
	ISPResult result = ISPSuccess;
	this->mInputImg.rawSize = rawSize;
	return result;
}

ISPResult ImageFileManager::SetOutputImgInfo(OutputImgInfo info)
{
	ISPResult result = ISPSuccess;
	this->mOutputImg.pOutputPath = info.pOutputPath;
	this->mOutputImg.width = info.width;
	this->mOutputImg.hight = info.hight;
	return result;
}

ISPResult ImageFileManager::SetOutputImgInfo(char* path, unsigned int width, unsigned int hight)
{
	ISPResult result = ISPSuccess;
	this->mOutputImg.pOutputPath = path;
	this->mOutputImg.width = width;
	this->mOutputImg.hight = hight;
	return result;
}

ISPResult ImageFileManager::SetOutputImgInfo(char* path)
{
	ISPResult result = ISPSuccess;
	this->mOutputImg.pOutputPath = path;
	return result;
}

ISPResult ImageFileManager::SetOutputImgInfo(unsigned width, unsigned int hight)
{
	ISPResult result = ISPSuccess;
	this->mOutputImg.width = width;
	this->mOutputImg.hight = hight;
	return result;
}

ISPResult ImageFileManager::ReadRawData(unsigned char* container, ISPRawFormate formate)
{
	ISPResult result = ISPSuccess;

	if (this->mState == Uninited) {
		result = ISPInvalied;
	}
	
	if (result == ISPSuccess) {
		ifstream OpenFile(this->mInputImg.pInputPath, ios::in | ios::binary);
		if (OpenFile.fail()) {
			cout << __FUNCTION__ << " Open RAW failed!" << endl;
			return ISPFailed;
		}
		OpenFile.seekg(0, ios::end);
		streampos fileSize = OpenFile.tellg();
		this->mInputImg.rawSize = (unsigned int)fileSize;
		cout << __FUNCTION__ << " Raw size:" << fileSize << endl;
		OpenFile.seekg(0, ios::beg);

		if (formate == Mipi10Bit) {
			unsigned char* data = new unsigned char[this->mInputImg.rawSize];
			OpenFile.read((char*)data, this->mInputImg.rawSize);
			memcpy((void* )container, (const void*)data, this->mInputImg.rawSize);
			delete[] data;
		}
		OpenFile.close();
	}
	return result;
}

ISPResult ImageFileManager::SaveBMP(unsigned char* srcData,unsigned int channels)
{
	ISPResult result = ISPSuccess;
	if (this->mState == Uninited) {
		result = ISPFailed;
		cout << __FUNCTION__ << " ImageFile didnot init" << endl;
	}
	else {
		BYTE* BMPdata = new BYTE[this->mOutputImg.width * this->mOutputImg.hight * channels];
		memset(BMPdata, 0x00, this->mOutputImg.width * this->mOutputImg.hight * channels);
		SetBMP(srcData, channels, BMPdata);
		WriteBMP(BMPdata, channels);
		delete BMPdata;
	}
	return result;
}

void ImageFileManager::SetBMP(unsigned char* srcData, unsigned int channels, BYTE* dstData)
{
	unsigned int j = 0;
	unsigned int row = 0;
	unsigned int col = 0;
	unsigned int size = this->mOutputImg.width * this->mOutputImg.hight;
	BYTE temp;
	if (channels == 3) {
		for (int i = 0; i < size; i++) {
			dstData[i * 3] = srcData[i * 3];
			dstData[i * 3 + 1] = srcData[i * 3 + 1];
			dstData[i * 3 + 2] = srcData[i * 3 + 2];
		}
		//¾ØÕó·´×ª
		while (j < 3 * size - j) {
			temp = dstData[3 * size - j - 1];
			dstData[3 * size - j - 1] = dstData[j];
			dstData[j] = temp;
			j++;
		}
		//Í¼Ïñ¾µÏñ·­×ª
		for (row = 0; row < this->mOutputImg.hight; row++) {
			while (col < 3 * this->mOutputImg.width - col) {
				temp = dstData[row * 3 * this->mOutputImg.width + 3 * this->mOutputImg.width - col - 1];
				dstData[3 * row * this->mOutputImg.width + 3 * this->mOutputImg.width - col - 1] = 
					dstData[3 * row * this->mOutputImg.width + col];
				dstData[3 * row * this->mOutputImg.width + col] = temp;
				col++;
			}
			col = 0;
		}
	}
	else {
		cout << __FUNCTION__ << " Current Bitmap support only for 3 channnels"<<endl;
		//Wait for developing here
	}
}

void ImageFileManager::WriteBMP(BYTE* data, unsigned channels) {
	BITMAPFILEHEADER header;
	BITMAPINFOHEADER headerinfo;
	unsigned int BMPSize = this->mOutputImg.width * this->mOutputImg.hight * channels;

	header.bfType = 0x4D42;
	header.bfReserved1 = 0;
	header.bfReserved2 = 0;
	header.bfSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + BMPSize;
	header.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
	headerinfo.biSize = sizeof(BITMAPINFOHEADER);
	headerinfo.biHeight = this->mOutputImg.hight;
	headerinfo.biWidth = this->mOutputImg.width;
	headerinfo.biPlanes = 1;
	headerinfo.biBitCount = 24;
	headerinfo.biCompression = 0; //BI_RGB
	headerinfo.biSizeImage = BMPSize;
	headerinfo.biClrUsed = 0;
	headerinfo.biClrImportant = 0;
	cout << " BMPPath: " << this->mOutputImg.pOutputPath << endl;
	ofstream out(this->mOutputImg.pOutputPath, ios::binary);
	out.write((char*)& header, sizeof(BITMAPFILEHEADER));
	out.write((char*)& headerinfo, sizeof(BITMAPINFOHEADER));
	out.write((char*)data, BMPSize);
	out.close();
	cout << " BMP saved " << endl;
}


