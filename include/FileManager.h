//////////////////////////////////////////////////////////////////////////////////////
// Author: Peng Hao <635945005@qq.com>
// License: GPL
//////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////
// @file: FileManager.cpp
// @brief: A file manager to manage image file, provoid file operations.
//////////////////////////////////////////////////////////////////////////////////////

#pragma once
#include <fstream>

#include "BMP.h"
#include "Utils.h"

typedef int32_t ISPState;
static const ISPState Uninited = 0;
static const ISPState Inited = 1;

typedef int32_t ISPRawFormate;
static const ISPRawFormate Mipi10Bit = 0;

struct InputImgInfo
{
	char* pInputPath;
	int32_t rawSize;
};

struct OutputImgInfo {
	char* pOutputPath;
	int32_t width;
	int32_t hight;
};

class ImageFileManager {
private:
	InputImgInfo mInputImg;
	OutputImgInfo mOutputImg;
	ISPState mState = Uninited;

public:
	ImageFileManager() { mState = Init(); };
	~ImageFileManager() { mState = Deinit(); };
	ISPState Init();
	ISPState Deinit();
	InputImgInfo GetInputImgInfo() { return mInputImg; };
	OutputImgInfo GetOutputImgInfo() { return mOutputImg; };
	ISPResult SetInputImgInfo(InputImgInfo info);
	ISPResult SetInputImgInfo(char* path, int32_t size);
	ISPResult SetInputImgInfo(char* path);
	ISPResult SetInputImgInfo(int32_t size);
	ISPResult SetOutputImgInfo(OutputImgInfo info);
	ISPResult SetOutputImgInfo(char* path, int32_t width, int32_t hight);
	ISPResult SetOutputImgInfo(char* path);
	ISPResult SetOutputImgInfo(int32_t width, int32_t hight);
	ISPResult ReadRawData(uint8_t* buffer, int32_t bufferSize, ISPRawFormate formate);
	ISPResult SetBMP(uint8_t* srcData, int32_t channels, BYTE* dstData);
	void WriteBMP(BYTE* data, int32_t channels);
	ISPResult SaveBMP(uint8_t* srcData, int32_t channels);
};

void DumpImgDataAsText(void* data, int32_t height, int32_t width, size_t bitWidth, char* dumpPath);
