// License: GPL-3.0-or-later
/*
 * Define some param struct which is used in ISP.
 *
 * Copyright (c) 2019 Peng Hao <635945005@qq.com>
 */

#pragma once

#define CCM_WIDTH 3
#define CCM_HEIGHT 3
#define LSC_LUT_WIDTH 17
#define LSC_LUT_HEIGHT 13
#define GAMMA_LUT_SIZE 1024

struct BLC_PARAM {
	uint32_t bitNum;
	uint16_t BLCDefaultValue;
};

struct LSC_PARAM {
	float GainCh1[LSC_LUT_HEIGHT][LSC_LUT_WIDTH];
	float GainCh2[LSC_LUT_HEIGHT][LSC_LUT_WIDTH];
	float GainCh3[LSC_LUT_HEIGHT][LSC_LUT_WIDTH];
	float GainCh4[LSC_LUT_HEIGHT][LSC_LUT_WIDTH];
};

struct GCC_PARAM {
	float weight;
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
	float CCM[CCM_HEIGHT][CCM_WIDTH];
};

struct GAMMA_PARAM {
	uint16_t lut[GAMMA_LUT_SIZE];
};

struct WNR_PARAM {
	int32_t ch1Threshold[3];
	int32_t ch2Threshold[3];
	int32_t ch3Threshold[3];
};

struct EE_PARAM {
	float alpha;
	int32_t coreSize;
	int32_t sigma;
};
