// License: GPL-3.0-or-later
/*
 * BoZhi exposed library interface.
 *
 * Copyright (c) 2019 Peng Hao <635945005@qq.com>
 */

#pragma once
#include <stdint.h>
#include <stdlib.h>

#define CCM_WIDTH 3
#define CCM_HEIGHT 3
#define LSC_LUT_WIDTH 17
#define LSC_LUT_HEIGHT 13
#define GAMMA_LUT_SIZE 1024

#define LIB_FUNCS_NUM 3
#define SYMBLE_SIZE_MAX 255

extern "C" {
typedef void (*LIB_VOID_FUNC_ADDR)(void*, ...);
}

enum BZRawFormat {
	BZ_ANDROID_RAW10 = 0,
	BZ_ORDINAL_RAW10,
	BZ_UNPACKAGED_RAW10_LSB,
	BZ_UNPACKAGED_RAW10_MSB,
	BZ_RAW_FORMAT_NUM
};

enum BZBayerOrder {
	BZ_BO_BGGR = 0,
	BZ_BO_GBRG,
	BZ_BO_GRBG,
	BZ_BO_RGGB,
	BZ_BO_NUM
};

enum BZParamType {
	BZ_PARAM_TYPE_IMAGE_INFO = 0,
	BZ_PARAM_TYPE_BLC,
	BZ_PARAM_TYPE_LSC,
	BZ_PARAM_TYPE_DMC,
	BZ_PARAM_TYPE_WB,
	BZ_PARAM_TYPE_CC,
	BZ_PARAM_TYPE_Gamma,
	BZ_PARAM_TYPE_WNR,
	BZ_PARAM_TYPE_EE,
	BZ_PARAM_TYPE_RAW2RGB,
	BZ_PARAM_TYPE_RGB2YUV,
	BZ_PARAM_TYPE_YUV2RGB,
	BZ_PARAM_TYPE_NUM
};

struct BZImgInfo
{
	int32_t width;
	int32_t height;
	int32_t bitspp;
	int32_t stride;
	int32_t rawFormat;
	int32_t bayerOrder;
};

struct BZBlcParam {
	uint16_t offset;
};

struct BZLscParam {
	float gainCh1[LSC_LUT_HEIGHT * LSC_LUT_WIDTH];
	float gainCh2[LSC_LUT_HEIGHT * LSC_LUT_WIDTH];
	float gainCh3[LSC_LUT_HEIGHT * LSC_LUT_WIDTH];
	float gainCh4[LSC_LUT_HEIGHT * LSC_LUT_WIDTH];
};

struct BZWbParam {
	float rGain;
	float gGain;
	float bGain;
};

struct BZCcParam {
	float ccm[CCM_HEIGHT * CCM_WIDTH];
};

struct BZGammaParam {
	uint16_t lut[GAMMA_LUT_SIZE];
};

struct BZWnrParam {
	int32_t ch1Threshold[3];
	int32_t ch2Threshold[3];
	int32_t ch3Threshold[3];
};

struct BZEeParam {
	float alpha;
	int32_t coreSize;
	int32_t sigma;
};

struct BZParam {
	BZImgInfo info;
	BZBlcParam blc;
	BZLscParam lsc;
	BZWbParam wb;
	BZCcParam cc;
	BZGammaParam gamma;
	BZWnrParam wnr;
	BZEeParam ee;
};

struct ISPUtilsFuncs {
	void (*Log) (const char* str, ...);
	void (*DumpDataInt) (void* pData, ...);
	void* (*Alloc) (size_t size, ...);
	void* (*Free) (void* pBuf, ...);
};

struct ISPCallbacks {
	void (*ISPNotify) (int32_t argNum, ...);
	ISPUtilsFuncs UtilsFuncs;
};

enum BZCmd {
	BZ_CMD_CREATE_PROC = 0,
	BZ_CMD_PROCESS,
	BZ_CMD_NUM,
};

enum BZMsg {
	MSG_D0 = 0,
	MSG_D1,
	MSG_D2,
	MSG_D3,
	MSG_D4,
	MSG_D5,
	MSG_D6,
	MSG_D7,
	MSG_D8,
	MSG_D9,
	MSG_NUM_MAX
};

struct BZCtrl {
	bool en;
	void *pInfo;
	void *pSrc;
	void *pDst;
	void *pParam;
};

struct BZOps
{
	int32_t (*BZEvent)	(uint32_t *msg);
};


