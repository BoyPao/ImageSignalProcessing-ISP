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
#include "ISPCore.h"

void BF(unsigned char* b, unsigned char* g, unsigned char* r, int dec, int Colorsigma, int Spacesigma, bool enable);// wait for rewrite alg
ISPResult EdgePreservedNR(Mat YUV, Mat NRYUV, float arph, bool enable);// wait for rewrite alg

//Bayer Process
ISPResult BlackLevelCorrection(void* data, int32_t argNum, ...);
ISPResult LensShadingCorrection(void* data, int32_t argNum, ...);
ISPResult GreenChannelsCorrection(void* gdata, int32_t argNum, ...); // wait for rewrite alg

//RGB Process
ISPResult WhiteBalance(void* data, int32_t argNum, ...);
ISPResult ColorCorrection(void* data, int32_t argNum, ...);
ISPResult GammaCorrection(void* data, int32_t argNum, ...);

//YUVProcess
ISPResult WaveletNR(void* data, int32_t argNum, ...);
ISPResult Sharpness(void* data, int32_t argNum, ...);

Mat getim(Mat src, int32_t WIDTH, int32_t HEIGHT,
	int depth, int Imgsizey, int Imgsize, int channel,
	int strength1, int strength2, int strength3);
Mat WDT(const Mat& _src, const string _wname, const int _level);
Mat waveletDecompose(Mat _src, Mat _lowFilter, Mat _highFilter);
void wavelet(const string _wname, Mat& _lowFilter, Mat& _highFilter);

