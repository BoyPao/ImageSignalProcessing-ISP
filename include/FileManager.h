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

#include "Utils.h"
#include "ISPVideo.h"
#ifdef LINUX_SYSTEM
#include "BMP.h"
#elif defined WIN32_SYSTEM
#include <windows.h>
#endif

class FileManager;

typedef int32_t ISPState;
static const ISPState Uninited = 0;
static const ISPState Inited = 1;

enum INPUT_FILE_TYPE {
	INPUT_FILE_TYPE_RAW,
	INPUT_FILE_TYPE_NUM
};

enum OUTPUT_FILE_TYPE {
	OUTPUT_FILE_TYPE_BMP,
	OUTPUT_FILE_TYPE_AVI,
	OUTPUT_FILE_TYPE_NUM
};

struct InputInfo {
	char path[FILE_PATH_MAX_SIZE];
	INPUT_FILE_TYPE type;
	int32_t size;
};

struct OutputImgInfo {
	char path[FILE_PATH_MAX_SIZE];
	OUTPUT_FILE_TYPE type;
	int32_t size;
	int32_t width;
	int32_t hight;
	int32_t channels;
};

struct OutputVideoInfo {
	char path[FILE_PATH_MAX_SIZE];
	OUTPUT_FILE_TYPE type;
	int32_t width;
	int32_t hight;
	int32_t fps;
	int32_t frameNum;
};

struct OutputInfo {
	OutputImgInfo imgInfo;
	OutputVideoInfo videoInfo;
};

struct VideoThreadParam {
	ISPVideo* pVideo;
	FileManager* pFileMgr;
};

class FileManager {
public:
	FileManager();
	~FileManager();
	ISPResult Init();
	ISPResult DeInit();

	InputInfo GetInputInfo() { return mInputInfo; };
	OutputInfo GetOutputInfo() { return mOutputInfo; };
	OutputImgInfo GetOutputImgInfo() { return mOutputInfo.imgInfo; };
	ISPResult GetOutputVideoInfo(OutputVideoInfo* pInfo);
	ISPResult SetInputInfo(InputInfo info);
	ISPResult SetOutputInfo(OutputInfo info);
	ISPResult SetOutputImgInfo(OutputImgInfo info);
	ISPResult SetOutputVideoInfo(OutputVideoInfo info);
	ISPResult ReadData(uint8_t* buffer, int32_t bufferSize);
	ISPResult SaveImgData(uint8_t* srcData);
	ISPResult CreateVideo(void* dst);
	ISPResult SaveVideoData(int32_t frameCount);
	ISPResult DestroyVideo();

private:
	ISPResult ReadRawData(uint8_t* buffer, int32_t bufferSize);
	ISPResult SaveBMPData(uint8_t* srcData, int32_t channels);
	ISPResult SetBMP(uint8_t* srcData, int32_t channels, BYTE* dstData);
	void WriteBMP(BYTE* data, int32_t channels);

	InputInfo mInputInfo;
	OutputInfo mOutputInfo;
	ISPState mState = Uninited;
	std::unique_ptr<ISPVideo> mVideo;
	VideoThreadParam mVTParam;
};

void DumpImgDataAsText(void* data, int32_t height, int32_t width, size_t bitWidth, char* dumpPath);
