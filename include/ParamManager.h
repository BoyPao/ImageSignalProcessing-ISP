//////////////////////////////////////////////////////////////////////////////////////
// Author: Peng Hao <635945005@qq.com>
// License: GPL
//////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////
// @file: ParamManager.h
// @brief: ParamManager head file. ParamManager aims to manage multi params.
//////////////////////////////////////////////////////////////////////////////////////

#pragma once
#include "Utils.h"
#include "Params.h"
#include "../BZ/interface/LibInterface.h"

enum PARAM_INDEX {
	PARAM_1920x1080_D65_1000Lux = 0,
	PARAM_INDEX_NUM
};

enum PARAM_MANAGER_STATE {
	PM_EMPTY = 0,
	PM_SELECTED
};

enum RAW_FORMAT {
	ANDROID_RAW10 = 0,
	ORDINAL_RAW10,
	UNPACKAGED_RAW10_LSB,
	UNPACKAGED_RAW10_MSB,
	RAW_FORMAT_NUM
};

enum BAYER_ORDER {
	RGGB = 0,
	BGGR,
	GRBG,
	GBRG,
	BAYER_ORDER_NUM
};

struct IMG_INFO
{
	int32_t width;
	int32_t height;
	int32_t bitspp;
	int32_t stride;
	RAW_FORMAT rawFormat;
	BAYER_ORDER bayerOrder;
};

struct ISP_Config_Params {
	BLC_PARAM* pBLC_param;
	LSC_PARAM* pLSC_param;
	GCC_PARAM* pGCC_param;
	WB_PARM* pWB_param;
	CC_PARAM* pCC_param;
	GAMMA_PARAM* pGamma_param;
	WNR_PARAM* pWNR_param;
	EE_PARAM* pEE_param;
};

class ISPParamManager {
public:
	ISPParamManager();
	~ISPParamManager();

	ISPResult SelectParams(int32_t paramIndex);
	ISPResult SetIMGInfo(IMG_INFO* info);
	ISPResult GetIMGDimension(int32_t* width, int32_t* height);

	/*
	ISPResult GetIMGInfo(void* imgInfo);
	ISPResult GetRawType(RAW_FORMAT* pType);
	ISPResult GetBLCParam(uint16_t* offset);
	ISPResult GetLSCParam(float* pRGain, float* pGrGain, float* pGbGain, float* pBGain);
	ISPResult GetGCCParam(double * weight);

	ISPResult GetWBParam(double* rGain, double* gGain, double* bGain);
	ISPResult GetCCParam(float* pCcm);
	ISPResult GetGAMMAPARAM(uint16_t* plut);

	ISPResult GetWNRPARAM(int32_t* l1Threshold, int32_t* l2Threshold, int32_t* l3Threshold);
	ISPResult GetEERPARAM(double* alph, int32_t* coreSize, int32_t* delta);

	ISPResult SetIMGDimension(int32_t* width, int32_t* height);
	ISPResult SetIMGWidth(int32_t* width);
	ISPResult SetIMGHeight(int32_t* height);
	ISPResult SetBLCParam(uint16_t* offset);
	ISPResult SetLSCParam(float* pRGain, float* pGrGain, float* pGbGain, float* pBGain);
	ISPResult SetGCCParam(double* weight);

	ISPResult SetWBParam(double* rGain, double* gGain, double* bGain);
	ISPResult SetCCParam(float* pCcm);
	ISPResult SetGAMMAPARAM(uint16_t* plut);

	ISPResult SetWNRPARAM(int32_t* l1Threshold, int32_t* l2Threshold, int32_t* l3Threshold);
	ISPResult SetEERPARAM(double* alph, int32_t* coreSize, int32_t* delta);*/

	ISPResult GetImgInfo(LIB_PARAMS* pParams);
	ISPResult GetBLCParam(LIB_PARAMS* pParams);
	ISPResult GetLSCParam(LIB_PARAMS* pParams);
	ISPResult GetWBParam(LIB_PARAMS* pParams);
	ISPResult GetCCParam(LIB_PARAMS* pParams);
	ISPResult GetGAMMAParam(LIB_PARAMS* pParams);
	ISPResult GetWNRParam(LIB_PARAMS* pParams);
	ISPResult GetEEParam(LIB_PARAMS* pParams);

private:
	ISP_Config_Params mISP_ConfigParams;
	IMG_INFO mImg_Info;
	PARAM_MANAGER_STATE mState;
};
