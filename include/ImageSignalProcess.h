//////////////////////////////////////////////////////////////////////////////////////
// Author: Peng Hao <635945005@qq.com>
// License: GPL
//////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////
// @file: ImageSignalProcess.h
// @brief: ISP head file
//////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "Utils.h"
using namespace cv;
using namespace std;

typedef int32_t ISPResult;
static const ISPResult ISPSuccess   = 0;
static const ISPResult ISPFailed    = 1;
static const ISPResult ISPNotInited = 2;
static const ISPResult ISPDisabled  = 3;
static const ISPResult ISPNoMemory  = 4;
static const ISPResult ISPInvalied  = 5;

typedef int32_t ISPState;
static const ISPState Uninited = 0;
static const ISPState Inited   = 1;

typedef int32_t ISPRawFormate;
static const ISPRawFormate Mipi10Bit = 0;

enum PROCESS_TYPE {
	BLC = 0,
	LSC,
	GCC,
	WB,
	CC,
	GAMMA,
	SWNR,
	SHARPNESS,
	CST,
	NONE,
	PROCESS_TYPE_MAX
};

ISPResult Mipi10decode(unsigned char* src, int* dst, unsigned int rawSize);
ISPResult ReadChannels(int* data, int* B, int* G, int* R);
ISPResult Demosaic(int* data, int* B, int* G, int* R);
void FirstPixelInsertProcess(int* src, int* dst);
void TwoGPixelInsertProcess(int* src, int* dst);
void LastPixelInsertProcess(int* src, int* dst);
void Compress10to8(int* src, unsigned char* dst);
