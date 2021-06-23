//////////////////////////////////////////////////////////////////////////////////////
// Author: Peng Hao <635945005@qq.com>
// License: GPL
//////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////
// @file: Param.h
// @brief: ISP parameter head file.
//////////////////////////////////////////////////////////////////////////////////////

#pragma once
#include "ImageSignalProcess.h"

struct IMG_INFO
{
	int32_t width;
	int32_t height;
};

struct BLC_PARAM {
	uint32_t bitNum;
	uint16_t BLCDefaultValue;
};

struct LSC_PARAM {
	float rGain[13][17];
	float grGain[13][17];
	float gbGain[13][17];
	float bGain[13][17];
};

struct GCC_PARAM {
	double weight;
};

struct WB_GAIN {
	float rGain;
	float gGain;
	float bGain;
};

struct WB_PARM {
	bool WB1stGAMMA2rd;
	WB_GAIN gainType1;
	WB_GAIN gainType2;
};

struct CC_PARAM {
	float CCM[3][3];
};

struct GAMMA_PARAM {
	uint16_t lut[1024];
};

struct WNR_PARAM {
	int32_t L1_threshold;
	int32_t L2_threshold;
	int32_t L3_threshold;
};

struct EE_PARAM {
	double alpha;
	int32_t coreSize;
	int32_t delta;
};

class ISPParameter {
public:
	ISPResult GetIMGDimension(int32_t* width, int32_t* height);
	ISPResult GetBLCParam(uint16_t* offset);
	ISPResult GetLSCParam(float** rGain, float** grGain, float ** gbGain, float ** bGain);
	ISPResult GetGCCParam(double * weight);

	ISPResult GetWBParam(double* rGain, double* gGain, double* bGain);
	ISPResult GetCCParam(float* gain, int32_t row, int32_t col);
	ISPResult GetGAMMAPARAM(uint16_t* plut);

	ISPResult GetWNRPARAM(int32_t* l1Threshold, int32_t* l2Threshold, int32_t* l3Threshold);
	ISPResult GetEERPARAM(double* alph, int32_t* coreSize, int32_t* delta);

	ISPResult SetIMGDimension(int32_t* width, int32_t* height);
	ISPResult SetIMGWidth(int32_t* width);
	ISPResult SetIMGHeight(int32_t* height);
	ISPResult SetBLCParam(uint16_t* offset);
	ISPResult SetLSCParam(float** rGain, float** grGain, float** gbGain, float** bGain);
	ISPResult SetGCCParam(double* weight);

	ISPResult SetWBParam(double* rGain, double* gGain, double* bGain);
	ISPResult SetCCParam(float* gain, int32_t row, int32_t col);
	ISPResult SetGAMMAPARAM(uint16_t* plut);

	ISPResult SetWNRPARAM(int32_t* l1Threshold, int32_t* l2Threshold, int32_t* l3Threshold);
	ISPResult SetEERPARAM(double* alph, int32_t* coreSize, int32_t* delta);
};

