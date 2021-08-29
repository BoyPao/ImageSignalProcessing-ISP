//////////////////////////////////////////////////////////////////////////////////////
// Author: Peng Hao <635945005@qq.com>
// License: GPL
//////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////
// @file: AlgorithmInterface.h
// @brief: ISP algorithm interface head file.
//////////////////////////////////////////////////////////////////////////////////////

#pragma once
#include <stdint.h>

#define CCM_WIDTH 3
#define CCM_HEIGHT 3
#define LSC_LUT_WIDTH 17
#define LSC_LUT_HEIGHT 13
#define GAMMA_LUT_SIZE 1024

enum ISP_RAW_TYPE {
	ISP_RAW10_MIPI_RGGB = 0,
	ISP_RAW10_MIPI_BGGR,
	ISP_RAW10_UNPACKAGED_RGGB,
	ISP_RAW10_UNPACKAGED_BGGR,
	ISP_RAW_TYPE_NUM
};

struct ISP_IMG_INFO
{
	int32_t width;
	int32_t height;
	int32_t bitspp;
	int32_t stride;
	bool	packaged;
	ISP_RAW_TYPE rawType;
};

struct ISP_BLC_PARAM {
	uint16_t offset;
};

struct ISP_LSC_PARAM {
	float GainCh1[LSC_LUT_HEIGHT * LSC_LUT_WIDTH];
	float GainCh2[LSC_LUT_HEIGHT * LSC_LUT_WIDTH];
	float GainCh3[LSC_LUT_HEIGHT * LSC_LUT_WIDTH];
	float GainCh4[LSC_LUT_HEIGHT * LSC_LUT_WIDTH];
};

struct ISP_WB_PARAM {
	float rGain;
	float gGain;
	float bGain;
};

struct ISP_CC_PARAM {
	float CCM[CCM_HEIGHT * CCM_WIDTH];
};

struct ISP_GAMMA_PARAM {
	uint16_t lut[GAMMA_LUT_SIZE];
};

struct ISP_WNR_PARAM {
	int32_t L1_threshold;
	int32_t L2_threshold;
	int32_t L3_threshold;
};

struct ISP_EE_PARAM {
	float alpha;
	int32_t coreSize;
	int32_t delta;
};

struct ISP_LIB_PARAMS {
	ISP_IMG_INFO info;
	ISP_BLC_PARAM BLC_param;
	ISP_LSC_PARAM LSC_param;
	ISP_WB_PARAM WB_param;
	ISP_CC_PARAM CC_param;
	ISP_GAMMA_PARAM Gamma_param;
	ISP_WNR_PARAM WNR_param;
	ISP_EE_PARAM EE_param;
};

struct ISP_PROCESS_CALLBACKS {
	void (*ISP_CBs)(int32_t argNum, ...);
};

struct ISP_LIB_FUNCS
{
	void (*ISP_BLC)			(void* data, ISP_LIB_PARAMS* params, ISP_PROCESS_CALLBACKS CBs, ...);
	void (*ISP_LSC)			(void* data, ISP_LIB_PARAMS* params, ISP_PROCESS_CALLBACKS CBs, ...);
	void (*ISP_WB)			(void* data, ISP_LIB_PARAMS* params, ISP_PROCESS_CALLBACKS CBs, ...);
	void (*ISP_CC)			(void* data, ISP_LIB_PARAMS* params, ISP_PROCESS_CALLBACKS CBs, ...);
	void (*ISP_Gamma)		(void* data, ISP_LIB_PARAMS* params, ISP_PROCESS_CALLBACKS CBs, ...);
	void (*ISP_WNR)			(void* data, ISP_LIB_PARAMS* params, ISP_PROCESS_CALLBACKS CBs, ...);
	void (*ISP_EE)			(void* data, ISP_LIB_PARAMS* params, ISP_PROCESS_CALLBACKS CBs, ...);
	void (*ISP_Demosaic)	(void* src, void* dst, ISP_LIB_PARAMS* params, ISP_PROCESS_CALLBACKS CBs, ...);
	void (*ISP_CST_RGB2YUV)	(void* src, void* dst, ISP_LIB_PARAMS* params, ISP_PROCESS_CALLBACKS CBs, ...);
};

void RegisterISPLibFuncs();
//Bayer Process
void Lib_BlackLevelCorrection(void* data, ISP_LIB_PARAMS* pParams, ...);
void Lib_LensShadingCorrection(void* data, ISP_LIB_PARAMS* pParams, ...);

//RGB Process
void Lib_WhiteBalance(void* data, ISP_LIB_PARAMS* pParams, ...);
void Lib_ColorCorrection(void* data, ISP_LIB_PARAMS* pParams, ...);
void Lib_GammaCorrection(void* data, ISP_LIB_PARAMS* pParams, ...);

//YUVProcess
void Lib_WaveletNR(void* data, ISP_LIB_PARAMS* pParams, ...);
void Lib_EdgeEnhancement(void* data, ISP_LIB_PARAMS* pParams, ...);

//CST
void Lib_Demosaic(void* src, void* dst, ISP_LIB_PARAMS* pParams, ...);
void Lib_CST_RGB2YUV(void* src, void* dst, ISP_LIB_PARAMS* pParams, ...);
