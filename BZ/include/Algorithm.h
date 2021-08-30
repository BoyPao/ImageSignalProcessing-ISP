//////////////////////////////////////////////////////////////////////////////////////
// Author: Peng Hao <635945005@qq.com>
// License: GPL
//////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////
// @file: Algorithm.h
// @brief: ISP algorithm head file.
//////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "BZUtils.h"
#include "BZLog.h"
#include "LibInterface.h"


#define PIXELS2WORDS_MIPI_PACKAGED(pixelNum, bitspp)	(((bitspp) < BITS_PER_WORD) ?										\
														0 :																	\
														(((bitspp) == BITS_PER_WORD) ?										\
															(pixelNum) :													\
															((pixelNum) * (bitspp) / BITS_PER_WORD)))
#define	PIXELS2WORDS_UNPACKAGED(pixelNum, bitspp)		(((bitspp) < BITS_PER_WORD) ?										\
														0 :																	\
														(((bitspp) == BITS_PER_WORD) ?										\
															(pixelNum) :													\
															((pixelNum) * 2)))
#define PIXELS2WORDS(pixelNum, bitspp, packaged)		(((bitspp) % 2 || (bitspp) > 2 * BITS_PER_WORD || (bitspp) <= 0) ?	\
														0 :																	\
														((packaged) ?														\
															PIXELS2WORDS_MIPI_PACKAGED(pixelNum, bitspp) :					\
															PIXELS2WORDS_UNPACKAGED(pixelNum, bitspp)))


#define ALIGN(x, align)									(align) ? (((x) + (align) - 1) & (~((align) - 1))) : (x)
#define ALIGNx(pixelNum, bitspp, packaged, align)		ALIGN(PIXELS2WORDS(pixelNum, bitspp, packaged), align)

//Bayer Process
void LIB_BlackLevelCorrection(void* data, LIB_PARAMS* pParams, ISP_CALLBACKS CBs, ...);
void LIB_LensShadingCorrection(void* data, LIB_PARAMS* pParams, ISP_CALLBACKS CBs, ...);

//RGB Process
void LIB_WhiteBalance(void* data, LIB_PARAMS* pParams, ISP_CALLBACKS CBs, ...);
void LIB_ColorCorrection(void* data, LIB_PARAMS* pParams, ISP_CALLBACKS CBs, ...);
void LIB_GammaCorrection(void* data, LIB_PARAMS* pParams, ISP_CALLBACKS CBs, ...);

//YUVProcess
void LIB_WaveletNR(void* data, LIB_PARAMS* pParams, ISP_CALLBACKS CBs, ...);
void LIB_EdgeEnhancement(void* data, LIB_PARAMS* pParams, ISP_CALLBACKS CBs, ...);

//CST
void LIB_Demosaic	(void* src, void* dst, LIB_PARAMS* pParams, ISP_CALLBACKS CBs, ...);
void LIB_CST_RGB2YUV (void* src, void* dst, LIB_PARAMS* pParams, ISP_CALLBACKS CBs, ...);

//Bayer Process
BZResult BZ_BlackLevelCorrection(void* data, LIB_PARAMS* pParams, ISP_CALLBACKS CBs, ...);
BZResult BZ_LensShadingCorrection(void* data, LIB_PARAMS* pParams, ISP_CALLBACKS CBs, ...);

//RGB Process
BZResult BZ_WhiteBalance(void* data, LIB_PARAMS* pParams, ISP_CALLBACKS CBs, ...);
BZResult BZ_ColorCorrection(void* data, LIB_PARAMS* pParams, ISP_CALLBACKS CBs, ...);
BZResult BZ_GammaCorrection(void* data, LIB_PARAMS* pParams, ISP_CALLBACKS CBs, ...);

//YUVProcess
BZResult BZ_WaveletNR(void* data, LIB_PARAMS* pParams, ISP_CALLBACKS CBs, ...);
BZResult BZ_EdgeEnhancement(void* data, LIB_PARAMS* pParams, ISP_CALLBACKS CBs, ...);

//CST
BZResult BZ_Demosaic(void* src, void* dst, LIB_PARAMS* pParams, ISP_CALLBACKS CBs, ...);
BZResult BZ_CST_RGB2YUV(void* src, void* dst, LIB_PARAMS* pParams, ISP_CALLBACKS CBs, ...);




