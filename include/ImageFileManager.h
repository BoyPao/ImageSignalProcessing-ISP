//////////////////////////////////////////////////////////////////////////////////////
// Author: Peng Hao <635945005@qq.com>
// License: GPL
//////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////
// @file: ImageFileManager.cpp
// @brief: A file manager to manage image file, provoid file operations
//////////////////////////////////////////////////////////////////////////////////////

#pragma once
#include "Utils.h"
#include "ImageSignalProcess.h"

//#define PATH_MAX_LENGTH 40

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
	ImageFileManager() { mState = this->Init(); };
	~ImageFileManager() { mState = this->Deinit(); };
	ISPState Init();
	ISPState Deinit();
	InputImgInfo GetInputImgInfo() { return mInputImg; };
	OutputImgInfo GetOutputImgInfo() { return mOutputImg; };
	ISPResult SetInputImgInfo(InputImgInfo info);
	ISPResult SetInputImgInfo(char* path, unsigned int size);
	ISPResult SetInputImgInfo(char* path);
	ISPResult SetInputImgInfo(unsigned int size);
	ISPResult SetOutputImgInfo(OutputImgInfo info);
	ISPResult SetOutputImgInfo(char* path, unsigned int width, unsigned int hight);
	ISPResult SetOutputImgInfo(char* path);
	ISPResult SetOutputImgInfo(unsigned width, unsigned int hight);
	ISPResult ReadRawData(unsigned char* container, ISPRawFormate formate);
	void SetBMP(unsigned char* srcData, unsigned int channels, BYTE* dstData);
	void WriteBMP(BYTE* data, unsigned channels);
	ISPResult SaveBMP(unsigned char* srcData, unsigned int channels);
};
