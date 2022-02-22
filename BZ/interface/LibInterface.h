// License: GPL-3.0-or-later
/*
 * BoZhi exposed library interface.
 *
 * Copyright (c) 2019 Peng Hao <635945005@qq.com>
 */

#pragma once
#include <stdint.h>

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

enum LIB_RAW_FORMAT {
	LIB_ANDROID_RAW10 = 0,
	LIB_ORDINAL_RAW10,
	LIB_UNPACKAGED_RAW10_LSB,
	LIB_UNPACKAGED_RAW10_MSB,
	LIB_RAW_FORMAT_NUM
};

enum LIB_BAYER_ORDER {
	LIB_RGGB = 0,
	LIB_BGGR,
	LIB_GRBG,
	LIB_GBRG,
	LIB_BAYER_ORDER_NUM
};

struct LIB_IMG_INFO
{
	int32_t width;
	int32_t height;
	int32_t bitspp;
	int32_t stride;
	LIB_RAW_FORMAT rawFormat;
	LIB_BAYER_ORDER bayerOrder;
};

struct LIB_BLC_PARAM {
	uint16_t offset;
};

struct LIB_LSC_PARAM {
	float GainCh1[LSC_LUT_HEIGHT * LSC_LUT_WIDTH];
	float GainCh2[LSC_LUT_HEIGHT * LSC_LUT_WIDTH];
	float GainCh3[LSC_LUT_HEIGHT * LSC_LUT_WIDTH];
	float GainCh4[LSC_LUT_HEIGHT * LSC_LUT_WIDTH];
};

struct LIB_WB_PARAM {
	float rGain;
	float gGain;
	float bGain;
};

struct LIB_CC_PARAM {
	float CCM[CCM_HEIGHT * CCM_WIDTH];
};

struct LIB_GAMMA_PARAM {
	uint16_t lut[GAMMA_LUT_SIZE];
};

struct LIB_WNR_PARAM {
	int32_t L1_threshold;
	int32_t L2_threshold;
	int32_t L3_threshold;
};

struct LIB_EE_PARAM {
	float alpha;
	int32_t coreSize;
	int32_t delta;
};

struct LIB_PARAMS {
	LIB_IMG_INFO info;
	LIB_BLC_PARAM BLC_param;
	LIB_LSC_PARAM LSC_param;
	LIB_WB_PARAM WB_param;
	LIB_CC_PARAM CC_param;
	LIB_GAMMA_PARAM Gamma_param;
	LIB_WNR_PARAM WNR_param;
	LIB_EE_PARAM EE_param;
};

struct ISP_UTILS_FUNCS {
	void (*Log) (const char* str, ...);
	void (*DumpDataInt) (void* pData, ...);
};

struct ISP_CALLBACKS {
	void (*ISP_Notify) (int32_t argNum, ...);
	ISP_UTILS_FUNCS UtilsFuncs;
};

enum LIB_CMD {
	LIB_CMD_BLC = 0,
	LIB_CMD_LSC,
	LIB_CMD_DEMOSAIC,
	LIB_CMD_WB,
	LIB_CMD_CC,
	LIB_CMD_GAMMA,
	LIB_CMD_WNR,
	LIB_CMD_EE,
	LIB_CMD_RAW2RGB,
	LIB_CMD_RGB2YUV,
	LIB_CMD_YUV2RGB,
	LIB_CMD_NUM,
};

struct LIB_MSG {
	LIB_CMD cmd;
	void* pSrc;
	void* pDst;
	LIB_PARAMS* pParam;
	bool enable;
};

struct LIB_OPS
{
	int32_t (*LIB_Event)	(void* msg);
};


