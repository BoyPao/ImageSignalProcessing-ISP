//////////////////////////////////////////////////////////////////////////////////////
// Author: Peng Hao <635945005@qq.com>
// License: GPL
//////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////
// @file: ImageFileManager.cpp
// @brief: A file manager to manage image file, provoid file operations
//////////////////////////////////////////////////////////////////////////////////////

#include "FileManager.h"

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
	ISPResult result = ISP_SUCCESS;
	this->mInputImg.pInputPath = info.pInputPath;
	this->mInputImg.rawSize = info.rawSize;
	return result;
}

ISPResult ImageFileManager::SetInputImgInfo(char* path, int32_t rawSize)
{
	ISPResult result = ISP_SUCCESS;
	this->mInputImg.pInputPath = path;
	this->mInputImg.rawSize = rawSize;
	return result;
}

ISPResult ImageFileManager::SetInputImgInfo(char* path)
{
	ISPResult result = ISP_SUCCESS;
	this->mInputImg.pInputPath = path;
	return result;
}

ISPResult ImageFileManager::SetInputImgInfo(int32_t rawSize)
{
	ISPResult result = ISP_SUCCESS;
	this->mInputImg.rawSize = rawSize;
	return result;
}

ISPResult ImageFileManager::SetOutputImgInfo(OutputImgInfo info)
{
	ISPResult result = ISP_SUCCESS;
	this->mOutputImg.pOutputPath = info.pOutputPath;
	this->mOutputImg.width = info.width;
	this->mOutputImg.hight = info.hight;
	return result;
}

ISPResult ImageFileManager::SetOutputImgInfo(char* path, int32_t width, int32_t hight)
{
	ISPResult result = ISP_SUCCESS;
	this->mOutputImg.pOutputPath = path;
	this->mOutputImg.width = width;
	this->mOutputImg.hight = hight;
	return result;
}

ISPResult ImageFileManager::SetOutputImgInfo(char* path)
{
	ISPResult result = ISP_SUCCESS;
	this->mOutputImg.pOutputPath = path;
	return result;
}

ISPResult ImageFileManager::SetOutputImgInfo(int32_t width, int32_t hight)
{
	ISPResult result = ISP_SUCCESS;
	this->mOutputImg.width = width;
	this->mOutputImg.hight = hight;
	return result;
}

ISPResult ImageFileManager::ReadRawData(uint8_t* buffer, size_t bufferSize, ISPRawFormate formate)
{
	ISPResult result = ISP_SUCCESS;

	if (this->mState == Uninited) {
		result = ISP_STATE_ERROR;
	}
	
	if (SUCCESS(result)) {
		ifstream OpenFile(this->mInputImg.pInputPath, ios::in | ios::binary);
		if (OpenFile.fail()) {
			result = ISP_FAILED;
			ISPLoge("Open RAW failed! result:%d", result);
		}

		if (SUCCESS(result)) {
			OpenFile.seekg(0, ios::end);
			streampos fileSize = OpenFile.tellg();
			this->mInputImg.rawSize = (int32_t)fileSize;
			ISPLogi("Raw size:%d", (int32_t)fileSize);
			OpenFile.seekg(0, ios::beg);

			if (bufferSize >= mInputImg.rawSize) {
				if (formate == Mipi10Bit) {
					OpenFile.read((char*)buffer, this->mInputImg.rawSize);
				}
			}
			else {
				buffer = nullptr;
				result = ISP_MEMORY_ERROR;
				ISPLoge("Invalid buffer size! result:%d", result);
			}
		}

		OpenFile.close();
	}
	return result;
}

ISPResult ImageFileManager::SaveBMP(uint8_t* srcData, int32_t channels)
{
	ISPResult result = ISP_SUCCESS;
	if (this->mState == Uninited) {
		result = ISP_FAILED;
		ISPLoge("ImageFile didnot init");
	}

	if(SUCCESS(result)) {
		BYTE* BMPdata = new BYTE[this->mOutputImg.width * this->mOutputImg.hight * channels];
		result = SetBMP(srcData, channels, BMPdata);
		if (SUCCESS(result)) {
			WriteBMP(BMPdata, channels);
		}
		else {
			ISPLoge("SetBMP failed. result:&d", result);
		}
		delete[] BMPdata;
	}
	return result;
}

ISPResult ImageFileManager::SetBMP(uint8_t* srcData, int32_t channels, BYTE* dstData)
{
	ISPResult result = ISP_SUCCESS;
	int32_t size = this->mOutputImg.width * this->mOutputImg.hight;
	int32_t j = 0;
	BYTE temp;

	if(channels != 3 && channels != 1) {
		result = ISP_INVALID_PARAM;
		ISPLoge("Invalid BMP output channnels:%d", channels);
		//Wait for developing here
	}

	if (SUCCESS(result)) {
		memcpy(dstData, srcData, channels * size);

		//矩阵反转	
		while (j < channels * size - j) {
			temp = dstData[channels * size - j - 1];
			dstData[channels * size - j - 1] = dstData[j];
			dstData[j] = temp;
			j++;
		}

		//图像镜像翻转
		for (int32_t row = 0; row < this->mOutputImg.hight; row++) {
			int32_t col = 0;
			while (col < channels * this->mOutputImg.width - col) {
				temp = dstData[row * channels * this->mOutputImg.width + channels * this->mOutputImg.width - col - 1];
				dstData[channels * row * this->mOutputImg.width + channels * this->mOutputImg.width - col - 1] =
					dstData[channels * row * this->mOutputImg.width + col];
				dstData[channels * row * this->mOutputImg.width + col] = temp;
				col++;
			}
		}
	}

	return result;
}

void ImageFileManager::WriteBMP(BYTE* data, int32_t channels) {
	BITMAPFILEHEADER header;
	BITMAPINFOHEADER headerinfo;
	int32_t BMPSize = this->mOutputImg.width * this->mOutputImg.hight * channels;

	headerinfo.biSize = sizeof(BITMAPINFOHEADER);
	headerinfo.biHeight = this->mOutputImg.hight;
	headerinfo.biWidth = this->mOutputImg.width;
	headerinfo.biPlanes = 1;
	headerinfo.biBitCount = (channels == 1) ? 8 : 24;
	headerinfo.biCompression = 0; //BI_RGB
	headerinfo.biSizeImage = BMPSize;
	headerinfo.biClrUsed = (channels == 1) ? 256 : 0;
	headerinfo.biClrImportant = 0;

	header.bfType = ('M' << 8) + 'B'; // 其值为0x4D42;
	header.bfReserved1 = 0;
	header.bfReserved2 = 0;
	header.bfOffBits = (channels == 1) ?
		(sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + 256 * sizeof(RGBQUAD)) :
		(sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER));
	header.bfSize = header.bfOffBits + headerinfo.biSizeImage;

	ISPLogi("BMPPath:%s", this->mOutputImg.pOutputPath);
	ofstream out(this->mOutputImg.pOutputPath, ios::binary);
	out.write((char*)& header, sizeof(BITMAPFILEHEADER));
	out.write((char*)& headerinfo, sizeof(BITMAPINFOHEADER));
	if (channels == 1) {
		static RGBQUAD palette[256];
		for (unsigned int i = 0; i < 256; i++)
		{
			palette[i].rgbBlue = i;
			palette[i].rgbGreen = i;
			palette[i].rgbRed = i;
		}
		out.write((char*)palette, sizeof(RGBQUAD) * 256);
	}
	out.write((char*)data, BMPSize);
	out.close();

	ISPLogi("BMP saved");
}




