// License: GPL-3.0-or-later
/*
 * A file manager to manage files, provoid file operations.
 *
 * Copyright (c) 2019 Peng Hao <635945005@qq.com>
 */

#pragma once
#include <fstream>

#include "Utils.h"
#include "ISPVideo.h"
#include "IOHelper.h"
#ifdef LINUX_SYSTEM
#include "BMP.h"
#elif defined WIN32_SYSTEM
#include <windows.h>
#endif

#define OUTPUT_FILE_TYPE_SIZE 4 /* size of 'bmp\0'/'avi\0' is 4 */

class FileManager;

typedef int32_t ISPState;
static const ISPState Uninited = 0;
static const ISPState Inited = 1;

enum InputFileType {
	INPUT_FILE_TYPE_RAW,
	INPUT_FILE_TYPE_NUM
};

enum OutputFileType {
	OUTPUT_FILE_TYPE_BMP,
	OUTPUT_FILE_TYPE_AVI,
	OUTPUT_FILE_TYPE_NUM
};

struct InputInfo {
	char path[FILE_PATH_MAX_SIZE];
	int32_t type;
	int32_t size;
};

struct OutputImgInfo {
	char path[FILE_PATH_MAX_SIZE];
	int32_t type;
	int32_t size;
	int32_t width;
	int32_t height;
	int32_t channels;
};

struct OutputVideoInfo {
	char path[FILE_PATH_MAX_SIZE];
	int32_t type;
	int32_t width;
	int32_t height;
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

class FileManager:public IOHelper {
public:
	FileManager();
	~FileManager();
	int32_t Init();
	int32_t DeInit();

	InputInfo GetInputInfo() { return mInputInfo; };
	OutputInfo GetOutputInfo() { return mOutputInfo; };
	OutputImgInfo GetOutputImgInfo() { return mOutputInfo.imgInfo; };
	int32_t GetOutputVideoInfo(OutputVideoInfo* pInfo);
	int32_t SetInputInfo(InputInfo info);
	int32_t SetOutputInfo(OutputInfo info);
	int32_t SetOutputImgInfo(OutputImgInfo info);
	int32_t SetOutputVideoInfo(OutputVideoInfo info);
	int32_t ReadData(uint8_t* buffer, int32_t bufferSize);
	int32_t SaveImgData(uint8_t* srcData);
	int32_t CreateVideo(void* dst);
	int32_t SaveVideoData(int32_t frameCount);
	int32_t DestroyVideo();
	int32_t Mipi10decode(void* src, void* dst, ImgInfo* info);
	int32_t Input(IOInfo in);
	int32_t GetIOInfo(void* pInfo);

private:
	int32_t ReadRawData(uint8_t* buffer, int32_t bufferSize);
	int32_t SaveBMPData(uint8_t* srcData, int32_t channels);
	int32_t SetBMP(uint8_t* srcData, int32_t channels, BYTE* dstData);
	void WriteBMP(BYTE* data, int32_t channels);
	void HelpMenu();
	void SupportInfo();

	InputInfo mInputInfo;
	OutputInfo mOutputInfo;
	ISPState mState = Uninited;
	std::unique_ptr<ISPVideo> mVideo;
	VideoThreadParam mVTParam;
	int32_t IOInfoFlag; /* 0: Dynamic info  1: Static info*/
};

void DumpDataInt(void* pData, ...
		/* int32_t height, int32_t width, int32_t bitWidth, char* dumpPath */);

