//////////////////////////////////////////////////////////////////////////////////////
// Author: Peng Hao
// License: GPL
//////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////
// @file: ImageSignalProcess.h
// @brief: ISP head file
//////////////////////////////////////////////////////////////////////////////////////

#pragma once
#include<stdint.h>
#include<string>
#include <iostream>

#include "opencv2/core/core.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/calib3d/calib3d.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/photo.hpp"

using namespace cv;
using namespace std;

typedef int32_t ISPResult;
static const ISPResult ISPSuccess   = 0;
static const ISPResult ISPFailed    = 1;
static const ISPResult ISPNotInited = 2;
static const ISPResult ISPDisabled  = 3;
static const ISPResult ISPNoMemory  = 4;
static const ISPResult ISPInvalied  = 5;

typedef int32_t ISPState;
static const ISPState Uninited = 0;
static const ISPState Inited   = 1;

typedef int32_t ISPRawFormate;
static const ISPRawFormate Mipi10Bit = 0;

enum PROCESS_TYPE {
	BLC = 0,
	LSC,
	GCC,
	WB,
	CC,
	GAMMA,
	SWNR,
	SHARPNESS,
	PROCESS_TYPE_MAX
};

ISPResult Mipi10decode(unsigned char* src, int* dst, unsigned int rawSize);
ISPResult BlackLevelCorrection(void* data, uint32_t argNum, ...);
ISPResult ReadChannels(int* data, int* B, int* G, int* R);
void intDataSaveAsText(int* data, int height, int width, string TextPath);
void CharDataSaveAsText(unsigned char* data, int height, int width, string TextPath);
ISPResult Demosaic(int* data, int* B, int* G, int* R);
void FirstPixelInsertProcess(int* src, int* dst);
void TwoGPixelInsertProcess(int* src, int* dst);
void LastPixelInsertProcess(int* src, int* dst);
void Compress10to8(int* src, unsigned char* dst);
ISPResult GammaCorrection(void* data, uint32_t argNum, ...);
ISPResult GreenImbalanceCorrection(void* gdata, uint32_t argNum, ...); // wait for rewrite alg
ISPResult WhiteBalance(void* data, uint32_t argNum, ...);
void BF(unsigned char* b, unsigned char* g, unsigned char* r, int dec, int Colorsigma, int Spacesigma, bool enable);
ISPResult ColorCorrection(void* data, uint32_t argNum, ...);

ISPResult LensShadingCorrection(void* data, uint32_t argNum, ...);
void EdgePreservedNR(Mat YUV, Mat NRYUV, float arph, bool enable);

Mat getim(Mat src, int depth, int Imgsizey, int Imgsize, int channel, int strength1, int strength2, int strength3);
Mat WDT(const Mat& _src, const string _wname, const int _level);
Mat waveletDecompose(Mat _src, Mat _lowFilter, Mat _highFilter);
void wavelet(const string _wname, Mat& _lowFilter, Mat& _highFilter);
ISPResult SmallWaveNR(void* data, uint32_t argNum, ...);
ISPResult Sharpness(void* data, uint32_t argNum, ...);
