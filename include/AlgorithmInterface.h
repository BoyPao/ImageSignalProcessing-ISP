//////////////////////////////////////////////////////////////////////////////////////
// Author: Peng Hao <635945005@qq.com>
// License: GPL
//////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////
// @file: AlgorithmInterface.h
// @brief: ISP algorithm interface head file.
//////////////////////////////////////////////////////////////////////////////////////

#pragma once
#include "Utils.h"
#include "ParamManager.h"

ISPResult ISPLibInit(ISPParamManager* pPM, ...);
ISPResult InitISPCallbacks(ISP_CALLBACKS* pCbs);

/* Bayer Process */
ISPResult BlackLevelCorrection(void* data, ISPParamManager* pPM, ...);
ISPResult LensShadingCorrection(void* data, ISPParamManager* pPM, ...);

/* RGB Process */
ISPResult Demosaic(void* data, ISPParamManager* pPM, ...);
ISPResult WhiteBalance(void* data, ISPParamManager* pPM, ...);
ISPResult WhiteBalance(void* data, ISPParamManager* pPM, ...);
ISPResult ColorCorrection(void* data, ISPParamManager* pPM, ...);
ISPResult GammaCorrection(void* data, ISPParamManager* pPM, ...);

/* YUVProcess */
ISPResult WaveletNR(void* data, ISPParamManager* pPM, ...);
ISPResult EdgeEnhancement(void* data, ISPParamManager* pPM, ...);

/* CST */
ISPResult CST_RAW2RGB(void* src, void* dst, ISPParamManager* pPM, ...);
ISPResult CST_RGB2YUV(void* src, void* dst, ISPParamManager* pPM, ...);
ISPResult CST_YUV2RGB(void* src, void* dst, ISPParamManager* pPM, ...);
