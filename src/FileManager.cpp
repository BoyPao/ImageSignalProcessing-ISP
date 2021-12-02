//////////////////////////////////////////////////////////////////////////////////////
// Author: Peng Hao <635945005@qq.com>
// License: GPL
//////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////
// @file: FileManager.cpp
// @brief: A file manager to manage image file, provoid file operations
//////////////////////////////////////////////////////////////////////////////////////

#include "FileManager.h"

ImageFileManager::ImageFileManager()
{
	memset(&mInputImg, 0, sizeof(InputImgInfo));
	memset(&mOutputImg, 0, sizeof(OutputImgInfo));
	memset(&mOutputVideo, 0, sizeof(OutputVideoInfo));
	mState = Uninited;
}

ImageFileManager::~ImageFileManager()
{
	memset(&mInputImg, 0, sizeof(InputImgInfo));
	memset(&mOutputImg, 0, sizeof(OutputImgInfo));
	memset(&mOutputVideo, 0, sizeof(OutputVideoInfo));
	mState = Uninited;
}

ISPResult ImageFileManager::Init()
{
	ISPResult result = ISP_SUCCESS;

	//Currentlly nothing to do here
	mState = Inited;

	return result;
}

ISPResult ImageFileManager::DeInit()
{
	ISPResult result = ISP_SUCCESS;

	//Currentlly nothing to do here
	mState = Uninited;

	return result;
}

ISPResult ImageFileManager::SetInputImgInfo(InputImgInfo info)
{
	ISPResult result = ISP_SUCCESS;
	mInputImg.pInputPath = info.pInputPath;
	mInputImg.rawSize = info.rawSize;
	return result;
}

ISPResult ImageFileManager::SetInputImgInfo(char* path, int32_t rawSize)
{
	ISPResult result = ISP_SUCCESS;
	mInputImg.pInputPath = path;
	mInputImg.rawSize = rawSize;
	return result;
}

ISPResult ImageFileManager::SetInputImgInfo(char* path)
{
	ISPResult result = ISP_SUCCESS;
	mInputImg.pInputPath = path;
	return result;
}

ISPResult ImageFileManager::SetInputImgInfo(int32_t rawSize)
{
	ISPResult result = ISP_SUCCESS;
	mInputImg.rawSize = rawSize;
	return result;
}

ISPResult ImageFileManager::SetOutputImgInfo(OutputImgInfo info)
{
	ISPResult result = ISP_SUCCESS;
	mOutputImg.pOutputPath = info.pOutputPath;
	mOutputImg.width = info.width;
	mOutputImg.hight = info.hight;
	return result;
}

ISPResult ImageFileManager::SetOutputImgInfo(char* path, int32_t width, int32_t hight)
{
	ISPResult result = ISP_SUCCESS;
	mOutputImg.pOutputPath = path;
	mOutputImg.width = width;
	mOutputImg.hight = hight;
	return result;
}

ISPResult ImageFileManager::SetOutputImgInfo(char* path)
{
	ISPResult result = ISP_SUCCESS;
	mOutputImg.pOutputPath = path;
	return result;
}

ISPResult ImageFileManager::SetOutputImgInfo(int32_t width, int32_t hight)
{
	ISPResult result = ISP_SUCCESS;
	mOutputImg.width = width;
	mOutputImg.hight = hight;
	return result;
}

ISPResult ImageFileManager::SetOutputVideoInfo(OutputVideoInfo info)
{
	ISPResult result = ISP_SUCCESS;
	mOutputVideo.pOutputPath = info.pOutputPath;
	mOutputVideo.width = info.width;
	mOutputVideo.hight = info.hight;
	mOutputVideo.fps = info.fps;
	mOutputVideo.frameNum = info.frameNum;

	return result;
}

ISPResult ImageFileManager::ReadRawData(uint8_t* buffer, int32_t bufferSize)
{
	ISPResult result = ISP_SUCCESS;

	if (mState == Uninited) {
		result = ISP_STATE_ERROR;
	}

	if (SUCCESS(result)) {
		ISPLogi("Raw input path:%s", mInputImg.pInputPath);
		ifstream OpenFile(mInputImg.pInputPath, ios::in | ios::binary);
		if (OpenFile.fail()) {
			result = ISP_FAILED;
			ISPLoge("Open RAW failed! path:%s result:%d", mInputImg.pInputPath, result);
		}

		if (SUCCESS(result)) {
			OpenFile.seekg(0, ios::end);
			streampos fileSize = OpenFile.tellg();
			mInputImg.rawSize = (int32_t)fileSize;
			ISPLogd("File size:%d", (int32_t)fileSize);
			OpenFile.seekg(0, ios::beg);

			if (bufferSize >= mInputImg.rawSize) {
				OpenFile.read((char*)buffer, mInputImg.rawSize);
			}
			else {
				ISPLogw("File size:%d >  buffer size:%d! result:%d",
						mInputImg.rawSize, bufferSize, result);
				OpenFile.read((char*)buffer, bufferSize);
			}
		}

		OpenFile.close();
	}
	return result;
}

ISPResult ImageFileManager::SaveBMP(uint8_t* srcData, int32_t channels)
{
	ISPResult result = ISP_SUCCESS;
	if (mState == Uninited) {
		result = ISP_FAILED;
		ISPLoge("ImageFile didnot init");
	}

	if(SUCCESS(result)) {
		BYTE* BMPdata = new BYTE[mOutputImg.width * mOutputImg.hight * channels];
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
	int32_t size = mOutputImg.width * mOutputImg.hight;
	int32_t j = 0;
	BYTE temp;

	if(channels != 3 && channels != 1) {
		result = ISP_INVALID_PARAM;
		ISPLoge("Invalid BMP output channnels:%d", channels);
		//TODO: does it need to be support?
	}

	if (SUCCESS(result)) {
		memcpy(dstData, srcData, channels * size);

		/* Convertion for the head & tail of data array */
		while (j < channels * size - j) {
			temp = dstData[channels * size - j - 1];
			dstData[channels * size - j - 1] = dstData[j];
			dstData[j] = temp;
			j++;
		}

		/* mirror flip */
		for (int32_t row = 0; row < mOutputImg.hight; row++) {
			int32_t col = 0;
			while (col < channels * mOutputImg.width - col) {
				temp = dstData[row * channels * mOutputImg.width + channels * mOutputImg.width - col - 1];
				dstData[channels * row * mOutputImg.width + channels * mOutputImg.width - col - 1] =
					dstData[channels * row * mOutputImg.width + col];
				dstData[channels * row * mOutputImg.width + col] = temp;
				col++;
			}
		}
	}

	return result;
}

void ImageFileManager::WriteBMP(BYTE* data, int32_t channels) {
	BITMAPFILEHEADER header;
	BITMAPINFOHEADER headerinfo;
	int32_t BMPSize = mOutputImg.width * mOutputImg.hight * channels;
	int32_t tempSize;

	ISPLogd("BYTE:%d WORD:%d DWORD:%d LONG:%d", sizeof(BYTE), sizeof(WORD), sizeof(DWORD), sizeof(LONG));
	memset(&header, 0, sizeof(BITMAPFILEHEADER));
	memset(&headerinfo, 0, sizeof(BITMAPINFOHEADER));
	headerinfo.biSize = sizeof(BITMAPINFOHEADER);
	headerinfo.biHeight = mOutputImg.hight;
	headerinfo.biWidth = mOutputImg.width;
	headerinfo.biPlanes = 1;
	headerinfo.biBitCount = (channels == 1) ? 8 : 24;
	headerinfo.biCompression = 0; //BI_RGB
	headerinfo.biSizeImage = BMPSize;
	headerinfo.biClrUsed = (channels == 1) ? 256 : 0;
	headerinfo.biClrImportant = 0;

	header.bfType = ('M' << 8) + 'B'; // equals to 0x4D42;
	header.bfReserved1 = 0;
	header.bfReserved2 = 0;

	tempSize = (channels == 1) ?
		(sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + 256 * sizeof(RGBQUAD)) :
		(sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER));

#ifdef LINUX_SYSTEM
	header.bfOffBits[0] = (tempSize % 0x10000) & 0xffff;
	header.bfOffBits[1] = (tempSize / 0x10000) & 0xffff;
	ISPLogd("ch:%d head:%d headinfo:%d bfOffBits:%d(%x %x)",
			channels, sizeof(BITMAPFILEHEADER), sizeof(BITMAPINFOHEADER),
			tempSize, header.bfOffBits[1], header.bfOffBits[0]);
#elif defined WIN32_SYSTEM
	header.bfOffBits = tempSize;
#endif

	tempSize += headerinfo.biSizeImage;
#ifdef LINUX_SYSTEM
	header.bfSize[0] = (tempSize % 0x10000) & 0xffff;
	header.bfSize[1] = (tempSize / 0x10000) & 0xffff;
	ISPLogd("biSizeImage:%d bfSize:%d(%x %x)", headerinfo.biSizeImage,
			tempSize, header.bfSize[1], header.bfSize[0]);
#elif defined WIN32_SYSTEM
	header.bfSize = tempSize;
#endif

	ISPLogi("BMP output path:%s", mOutputImg.pOutputPath);
	ofstream out(mOutputImg.pOutputPath, ios::binary);
	if (out.fail()) {
		ISPLoge("Cannot open ouput file:%s", mOutputImg.pOutputPath);
	}
	else {
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
	}
	out.close();
}

void DumpImgDataAsText(void* data, int32_t height, int32_t width, size_t bitWidth, char* dumpPath) 
{
	if (data != nullptr) {
		if (dumpPath) {
			ofstream OutFile(dumpPath);
			for (int32_t i = 0; i < height; i++) {
				OutFile << i << ": ";
				for (int32_t j = 0; j < width; j++) {
					switch (bitWidth) {
						case sizeof(uint16_t) :
							OutFile << (int)static_cast<uint16_t*>(data)[i * width + j] << ' ';
							break;
							case sizeof(uint8_t) :
								OutFile << (int)static_cast<uint8_t*>(data)[i * width + j] << ' ';
								break;
							default:
								ISPLoge("Dump failed. Unsopported data type");
					}
				}
				OutFile << endl;
			}
			OutFile.close();
			ISPLoge("Data saved as TXT finished");
		}
		else {
			ISPLoge("dumpPath is null");
		}
	}
	else {
		ISPLoge("Dump failed. data is nullptr");
	}
}
