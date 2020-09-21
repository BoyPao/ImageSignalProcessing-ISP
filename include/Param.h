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
	int width;
	int height;
};

struct BLC_PARAM {
	int bitNum;
	int BLCDefaultValue;
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
	unsigned int gain[1024];
};

struct WNR_PARAM {
	int L1_threshold;
	int L2_threshold;
	int L3_threshold;
};

class ISPParameter {
public:
	ISPResult GetIMGDimension(int32_t* width, int32_t* height);
	ISPResult GetBLCParam(int32_t* offset);
	ISPResult GetLSCParam(float** rGain, float** grGain, float ** gbGain, float ** bGain);
	ISPResult GetGCCParam(double * weight);

	ISPResult GetWBParam(double* rGain, double* gGain, double* bGain);
	ISPResult GetCCParam(float* gain, int row, int col);
	ISPResult GetGAMMAPARAM(unsigned int* plut);

	ISPResult GetWNRPARAM(int* l1Threshold, int* l2Threshold, int* l3Threshold);
};