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




