//////////////////////////////////////////////////////////////////////////////////////
// Author: Peng Hao <635945005@qq.com>
// License: GPL
//////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////
// @file: Algorithm.h
// @brief: ISP algorithm head file.
//////////////////////////////////////////////////////////////////////////////////////

#pragma once
#include "Utils.h"
#include "AlgorithmInterface.h"

void BF(unsigned char* b, unsigned char* g, unsigned char* r, int dec, int Colorsigma, int Spacesigma, bool enable);// wait for rewrite alg
ISPResult EdgePreservedNR(Mat YUV, Mat NRYUV, float arph, bool enable);// wait for rewrite alg
ISPResult GreenChannelsCorrection(void* gdata, int32_t argNum, ...); // wait for rewrite alg

//Bayer Process
ISPResult ISP_BlackLevelCorrection(void* data, ISP_PROCESS_CALLBACKS CBs, ...);
ISPResult ISP_LensShadingCorrection(void* data, ISP_PROCESS_CALLBACKS CBs, ...);

//RGB Process
ISPResult ISP_WhiteBalance(void* data, ISP_PROCESS_CALLBACKS CBs, ...);
ISPResult ISP_ColorCorrection(void* data, ISP_PROCESS_CALLBACKS CBs, ...);
ISPResult ISP_GammaCorrection(void* data, ISP_PROCESS_CALLBACKS CBs, ...);

//YUVProcess
ISPResult ISP_WaveletNR(void* data, ISP_PROCESS_CALLBACKS CBs, ...);
ISPResult ISP_EdgeEnhancement(void* data, ISP_PROCESS_CALLBACKS CBs, ...);

//CST
ISPResult ISP_Demosaic	(void* src, void* dst, ISP_PROCESS_CALLBACKS CBs, ...);
ISPResult ISP_CST_RGB2YUV (void* src, void* dst, ISP_PROCESS_CALLBACKS CBs, ...);

/*Mat getim(Mat src, int32_t WIDTH, int32_t HEIGHT,
	int depth, int channel,
	int strength1, int strength2, int strength3);
Mat WDT(const Mat& _src, const string _wname, const int _level);
Mat waveletDecompose(Mat _src, Mat _lowFilter, Mat _highFilter);
void wavelet(const string _wname, Mat& _lowFilter, Mat& _highFilter);*/



