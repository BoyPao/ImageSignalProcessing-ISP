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

#define CHECK_PACKAGED(format)                          (((format) == UNPACKAGED_RAW10_LSB) ||								\
														((format) == UNPACKAGED_RAW10_MSB)) ? 0 : 1

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
