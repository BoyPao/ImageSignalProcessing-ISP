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
static const ISPResult ISPInvalid  = 5;

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

ISPResult Mipi10decode(void* src, void* dst, int32_t rawSize);
ISPResult ReadChannels(uint16_t* data, uint16_t* B, uint16_t* G, uint16_t* R);
ISPResult Demosaic(uint16_t* data, uint16_t* B, uint16_t* G, uint16_t* R);
void FirstPixelInsertProcess(uint16_t* src, uint16_t* dst);
void TwoGPixelInsertProcess(uint16_t* src, uint16_t* dst);
void LastPixelInsertProcess(uint16_t* src, uint16_t* dst);
void Compress10to8(uint16_t* src, unsigned char* dst, int32_t size, bool need_420_521);
