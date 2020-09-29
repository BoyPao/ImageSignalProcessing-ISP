//////////////////////////////////////////////////////////////////////////////////////
// Author: Peng Hao <635945005@qq.com>
// License: GPL
//////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////
// @file: ImageFileManager.cpp
// @brief: A file manager to manage image file, provoid file operations
//////////////////////////////////////////////////////////////////////////////////////

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

ISPResult ImageFileManager::SetInputImgInfo(char* path, int32_t rawSize)
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

ISPResult ImageFileManager::SetInputImgInfo(int32_t rawSize)
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

ISPResult ImageFileManager::SetOutputImgInfo(char* path, int32_t width, int32_t hight)
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

ISPResult ImageFileManager::SetOutputImgInfo(int32_t width, int32_t hight)
{
	ISPResult result = ISPSuccess;
	this->mOutputImg.width = width;
	this->mOutputImg.hight = hight;
	return result;
}

ISPResult ImageFileManager::ReadRawData(uint8_t* buffer, size_t bufferSize, ISPRawFormate formate)
{
	ISPResult result = ISPSuccess;

	if (this->mState == Uninited) {
		result = ISPFailed;
	}
	
	if (result == ISPSuccess) {
		ifstream OpenFile(this->mInputImg.pInputPath, ios::in | ios::binary);
		if (OpenFile.fail()) {
			cout << __FUNCTION__ << " Open RAW failed!" << endl;
			return ISPFailed;
		}
		OpenFile.seekg(0, ios::end);
		streampos fileSize = OpenFile.tellg();
		this->mInputImg.rawSize = (int32_t)fileSize;
		cout << __FUNCTION__ << " Raw size:" << fileSize << endl;
		OpenFile.seekg(0, ios::beg);

		if (bufferSize >= mInputImg.rawSize) {
			if (formate == Mipi10Bit) {
				OpenFile.read((char*)buffer, this->mInputImg.rawSize);
			}
		}else{
			cout << __FUNCTION__ << " Invalid buffer size!" << endl;
			buffer = nullptr;
			result = ISPNoMemory;
		}

		OpenFile.close();
	}
	return result;
}

ISPResult ImageFileManager::SaveBMP(uint8_t* srcData, int32_t channels)
{
	ISPResult result = ISPSuccess;
	if (this->mState == Uninited) {
		result = ISPFailed;
		cout << __FUNCTION__ << " ImageFile didnot init" << endl;
	}

	if(result == ISPSuccess) {
		BYTE* BMPdata = new BYTE[this->mOutputImg.width * this->mOutputImg.hight * channels];
		SetBMP(srcData, channels, BMPdata);
		WriteBMP(BMPdata, channels);
		delete[] BMPdata;
	}
	return result;
}

void ImageFileManager::SetBMP(uint8_t* srcData, int32_t channels, BYTE* dstData)
{
	if (channels == 3) {
		int32_t size = this->mOutputImg.width * this->mOutputImg.hight;
		for (int32_t i = 0; i < size; i++) {
			dstData[i * 3] = srcData[i * 3];
			dstData[i * 3 + 1] = srcData[i * 3 + 1];
			dstData[i * 3 + 2] = srcData[i * 3 + 2];
		}
		//¾ØÕó·´×ª
		int32_t j = 0;
		BYTE temp;
		while (j < 3 * size - j) {
			temp = dstData[3 * size - j - 1];
			dstData[3 * size - j - 1] = dstData[j];
			dstData[j] = temp;
			j++;
		}
		//Í¼Ïñ¾µÏñ·­×ª
		for (int32_t row = 0; row < this->mOutputImg.hight; row++) {
			int32_t col = 0;
			while (col < 3 * this->mOutputImg.width - col) {
				temp = dstData[row * 3 * this->mOutputImg.width + 3 * this->mOutputImg.width - col - 1];
				dstData[3 * row * this->mOutputImg.width + 3 * this->mOutputImg.width - col - 1] = 
					dstData[3 * row * this->mOutputImg.width + col];
				dstData[3 * row * this->mOutputImg.width + col] = temp;
				col++;
			}
		}
	}
	else {
		cout << __FUNCTION__ << " Current Bitmap support only for 3 channnels"<<endl;
		//Wait for developing here
	}
}

void ImageFileManager::WriteBMP(BYTE* data, int32_t channels) {
	BITMAPFILEHEADER header;
	BITMAPINFOHEADER headerinfo;
	int32_t BMPSize = this->mOutputImg.width * this->mOutputImg.hight * channels;

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


