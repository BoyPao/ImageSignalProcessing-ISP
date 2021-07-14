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

struct ISP_PROCESS_CALLBACKS {
	ISPResult(*ISP_CBs)(int32_t argNum, ...);
};

struct ISP_LIB_FUNCS
{
	ISPResult(*ISP_BLC)			(void* data, ISP_PROCESS_CALLBACKS CBs, ...);
	ISPResult(*ISP_LSC)			(void* data, ISP_PROCESS_CALLBACKS CBs, ...);
	ISPResult(*ISP_WB)			(void* data, ISP_PROCESS_CALLBACKS CBs, ...);
	ISPResult(*ISP_CC)			(void* data, ISP_PROCESS_CALLBACKS CBs, ...);
	ISPResult(*ISP_Gamma)		(void* data, ISP_PROCESS_CALLBACKS CBs, ...);
	ISPResult(*ISP_WNR)			(void* data, ISP_PROCESS_CALLBACKS CBs, ...);
	ISPResult(*ISP_EE)			(void* data, ISP_PROCESS_CALLBACKS CBs, ...);
	ISPResult(*ISP_Demosaic)	(void* src, void* dst, ISP_PROCESS_CALLBACKS CBs, ...);
	ISPResult(*ISP_CST_RGB2YUV) (void* src, void* dst, ISP_PROCESS_CALLBACKS CBs, ...);
};

ISPResult RegisterISPLibFuncs();
//Bayer Process
ISPResult BlackLevelCorrection(ISPParamManager* pPM, void* data, ...);
ISPResult LensShadingCorrection(ISPParamManager* pPM, void* data, ...);

//RGB Process
ISPResult WhiteBalance(ISPParamManager* pPM, void* data, ...);
ISPResult ColorCorrection(ISPParamManager* pPM, void* data, ...);
ISPResult GammaCorrection(ISPParamManager* pPM, void* data, ...);

//YUVProcess
ISPResult WaveletNR(ISPParamManager* pPM, void* data, ...);
ISPResult Sharpness(ISPParamManager* pPM, void* data, ...);

//CST
ISPResult Demosaic(ISPParamManager* pPM, void* src, void* dst, ...);
ISPResult CST_RGB2YUV(ISPParamManager* pPM, void* src, void* dst, ...);