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

struct BlcSetting {
	uint32_t bitNum;
	uint16_t BlcDefaultValue;
};

struct LscSetting {
	float gainCh1[LSC_LUT_HEIGHT][LSC_LUT_WIDTH];
	float gainCh2[LSC_LUT_HEIGHT][LSC_LUT_WIDTH];
	float gainCh3[LSC_LUT_HEIGHT][LSC_LUT_WIDTH];
	float gainCh4[LSC_LUT_HEIGHT][LSC_LUT_WIDTH];
};

struct WbGain {
	float rGain;
	float gGain;
	float bGain;
};

struct WbSetting {
	bool Wb1stGamma2rd;
	WbGain gainType1;
	WbGain gainType2;
};

struct CcSetting {
	float ccm[CCM_HEIGHT][CCM_WIDTH];
};

struct GammaSetting {
	uint16_t lut[GAMMA_LUT_SIZE];
};

struct WnrSetting {
	int32_t ch1Threshold[3];
	int32_t ch2Threshold[3];
	int32_t ch3Threshold[3];
};

struct EeSetting {
	float alpha;
	int32_t coreSize;
	int32_t sigma;
};
