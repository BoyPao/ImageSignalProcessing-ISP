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
#include "BZAlgorithmInterface.h"


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
void ISP_BlackLevelCorrection(void* data, ISP_LIB_PARAMS* pParams, ISP_PROCESS_CALLBACKS CBs, ...);
void ISP_LensShadingCorrection(void* data, ISP_LIB_PARAMS* pParams, ISP_PROCESS_CALLBACKS CBs, ...);

//RGB Process
void ISP_WhiteBalance(void* data, ISP_LIB_PARAMS* pParams, ISP_PROCESS_CALLBACKS CBs, ...);
void ISP_ColorCorrection(void* data, ISP_LIB_PARAMS* pParams, ISP_PROCESS_CALLBACKS CBs, ...);
void ISP_GammaCorrection(void* data, ISP_LIB_PARAMS* pParams, ISP_PROCESS_CALLBACKS CBs, ...);

//YUVProcess
void ISP_WaveletNR(void* data, ISP_LIB_PARAMS* pParams, ISP_PROCESS_CALLBACKS CBs, ...);
void ISP_EdgeEnhancement(void* data, ISP_LIB_PARAMS* pParams, ISP_PROCESS_CALLBACKS CBs, ...);

//CST
void ISP_Demosaic	(void* src, void* dst, ISP_LIB_PARAMS* pParams, ISP_PROCESS_CALLBACKS CBs, ...);
void ISP_CST_RGB2YUV (void* src, void* dst, ISP_LIB_PARAMS* pParams, ISP_PROCESS_CALLBACKS CBs, ...);

//Bayer Process
BZResult BZ_BlackLevelCorrection(void* data, ISP_LIB_PARAMS* pParams, ISP_PROCESS_CALLBACKS CBs, ...);
BZResult BZ_LensShadingCorrection(void* data, ISP_LIB_PARAMS* pParams, ISP_PROCESS_CALLBACKS CBs, ...);

//RGB Process
BZResult BZ_WhiteBalance(void* data, ISP_LIB_PARAMS* pParams, ISP_PROCESS_CALLBACKS CBs, ...);
BZResult BZ_ColorCorrection(void* data, ISP_LIB_PARAMS* pParams, ISP_PROCESS_CALLBACKS CBs, ...);
BZResult BZ_GammaCorrection(void* data, ISP_LIB_PARAMS* pParams, ISP_PROCESS_CALLBACKS CBs, ...);

//YUVProcess
BZResult BZ_WaveletNR(void* data, ISP_LIB_PARAMS* pParams, ISP_PROCESS_CALLBACKS CBs, ...);
BZResult BZ_EdgeEnhancement(void* data, ISP_LIB_PARAMS* pParams, ISP_PROCESS_CALLBACKS CBs, ...);

//CST
BZResult BZ_Demosaic(void* src, void* dst, ISP_LIB_PARAMS* pParams, ISP_PROCESS_CALLBACKS CBs, ...);
BZResult BZ_CST_RGB2YUV(void* src, void* dst, ISP_LIB_PARAMS* pParams, ISP_PROCESS_CALLBACKS CBs, ...);




