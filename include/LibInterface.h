//////////////////////////////////////////////////////////////////////////////////////
// Author: Peng Hao <635945005@qq.com>
// License: GPL
//////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////
// @file: LibInterface.h
// @brief: ISP algorithm library interface head file.
//////////////////////////////////////////////////////////////////////////////////////

#pragma once
#include <stdint.h>

#define CCM_WIDTH 3
#define CCM_HEIGHT 3
#define LSC_LUT_WIDTH 17
#define LSC_LUT_HEIGHT 13
#define GAMMA_LUT_SIZE 1024

enum LIB_RAW_TYPE {
	LIB_RAW10_MIPI_RGGB = 0,
	LIB_RAW10_MIPI_BGGR,
	LIB_RAW10_UNPACKAGED_RGGB,
	LIB_RAW10_UNPACKAGED_BGGR,
	LIB_RAW_TYPE_NUM
};

struct LIB_IMG_INFO
{
	int32_t width;
	int32_t height;
	int32_t bitspp;
	int32_t stride;
	bool	packaged;
	LIB_RAW_TYPE rawType;
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

struct ISP_CALLBACKS {
	void (*ISP_Notify)(int32_t argNum, ...);
};

struct LIB_FUNCS
{
	//Bayer Process
	void (*LIB_BLC)			(void* data, LIB_PARAMS* params, ISP_CALLBACKS CBs, ...);
	void (*LIB_LSC)			(void* data, LIB_PARAMS* params, ISP_CALLBACKS CBs, ...);

	//RGB Process
	void (*LIB_WB)			(void* data, LIB_PARAMS* params, ISP_CALLBACKS CBs, ...);
	void (*LIB_CC)			(void* data, LIB_PARAMS* params, ISP_CALLBACKS CBs, ...);
	void (*LIB_Gamma)		(void* data, LIB_PARAMS* params, ISP_CALLBACKS CBs, ...);

	//YUVProcess
	void (*LIB_WNR)			(void* data, LIB_PARAMS* params, ISP_CALLBACKS CBs, ...);
	void (*LIB_EE)			(void* data, LIB_PARAMS* params, ISP_CALLBACKS CBs, ...);
	void (*LIB_TAIL)			(void* data, LIB_PARAMS* params, ISP_CALLBACKS CBs, ...);

	//CST
	void (*LIB_Demosaic)	(void* src, void* dst, LIB_PARAMS* params, ISP_CALLBACKS CBs, ...);
	void (*LIB_CST_RGB2YUV)	(void* src, void* dst, LIB_PARAMS* params, ISP_CALLBACKS CBs, ...);
};

void RegisterISPLibFuncs(LIB_FUNCS* pLibFuncs);