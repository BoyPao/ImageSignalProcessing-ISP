//////////////////////////////////////////////////////////////////////////////////////
// Author: Peng Hao <635945005@qq.com>
// License: GPL
//////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////
// @file: Param.cpp
// @brief: ISP parameter source file. Include a parameter manager
//////////////////////////////////////////////////////////////////////////////////////

#include "ParamManager.h"

#include "Param_1920x1080_D65_1000Lux.h"

ISPResult ISPParameter::GetIMGDimension(int32_t* width, int32_t* height)
{
	ISPResult result = ISP_SUCCESS;
	*width = IMGINFO.width;
	*height = IMGINFO.height;
	return result;
}

ISPResult ISPParameter::GetBLCParam(uint16_t* offset)
{
	ISPResult result = ISP_SUCCESS;

	int difBitNum = BLCPARAM.bitNum - 8;
	if (0 < difBitNum && difBitNum < 4) {
		*offset = (BLCPARAM.BLCDefaultValue << difBitNum);
	}
	else {
		*offset = BLCPARAM.BLCDefaultValue;
	}
	return result;
}

ISPResult ISPParameter::GetLSCParam(float** rGain, float** grGain, float ** gbGain, float ** bGain)
{
	ISPResult result = ISP_SUCCESS;
	for (int32_t i = 0; i < 13; i++) {
		memcpy(rGain[i], LSCPARM.rGain[i], 17 * sizeof(float));
		memcpy(grGain[i], LSCPARM.grGain[i], 17 * sizeof(float));
		memcpy(gbGain[i], LSCPARM.gbGain[i], 17 * sizeof(float));
		memcpy(bGain[i], LSCPARM.bGain[i], 17 * sizeof(float));
	}

	return result;
}

ISPResult ISPParameter::GetGCCParam(double * weight)
{
	ISPResult result = ISP_SUCCESS;
	*weight = GCCPARAM.weight;

	return result;
}

ISPResult ISPParameter::GetWBParam(double* rGain, double* gGain, double* bGain)
{
	ISPResult result = ISP_SUCCESS;

	*rGain = WBPARAM.WB1stGAMMA2rd ? WBPARAM.gainType1.rGain : WBPARAM.gainType2.rGain;
	*gGain = WBPARAM.WB1stGAMMA2rd ? WBPARAM.gainType1.gGain : WBPARAM.gainType2.gGain;
	*bGain = WBPARAM.WB1stGAMMA2rd ? WBPARAM.gainType1.bGain : WBPARAM.gainType2.bGain;

	return result;
}

ISPResult ISPParameter::GetCCParam(float* gain, int32_t row, int32_t col)
{
	ISPResult result = ISP_SUCCESS;
	*gain = CCPARAM.CCM[row][col];

	return result;
}

ISPResult ISPParameter::GetGAMMAPARAM(uint16_t* plut)
{
	ISPResult result = ISP_SUCCESS;
	if (plut) {
		memcpy(plut, &GAMMAPARAM.lut, 1024 * sizeof(uint16_t));
	}
	else {
		ISPLoge("null ptr");
		result = ISP_INVALID_PARAM;
	}
	return result;
}

ISPResult ISPParameter::GetWNRPARAM(int32_t* l1Threshold, int32_t* l2Threshold, int32_t* l3Threshold)
{
	ISPResult result = ISP_SUCCESS;

	*l1Threshold = WNRPARAM.L1_threshold;
	*l2Threshold = WNRPARAM.L2_threshold;
	*l3Threshold = WNRPARAM.L3_threshold;

 	return result;
}

ISPResult ISPParameter::GetEERPARAM(double* alpha, int32_t* coreSize, int32_t* delta)
{
	ISPResult result = ISP_SUCCESS;

	*alpha = EEPARAM.alpha;
	*coreSize = EEPARAM.coreSize;
	*delta = EEPARAM.delta;

	return result;
}

ISPResult SetIMGDimension(int32_t* width, int32_t* height)
{
	ISPResult result = ISP_SUCCESS;

	IMGINFO.width = *width;
	IMGINFO.height = *height;

	return result;
}

ISPResult SetIMGWidth(int32_t* width)
{
	ISPResult result = ISP_SUCCESS;

	IMGINFO.width = *width;

	return result;
}

ISPResult SetIMGHeight(int32_t* height)
{
	ISPResult result = ISP_SUCCESS;

	IMGINFO.height = *height;

	return result;
}

