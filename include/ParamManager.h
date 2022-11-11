// License: GPL-3.0-or-later
/*
 * ParamManager aims to manage multi params.
 *
 * Copyright (c) 2019 Peng Hao <635945005@qq.com>
 */

#pragma once
#include "Utils.h"
#include "Params.h"

#define CHECK_PACKAGED(format)                          (((format) == UNPACKAGED_RAW10_LSB) ||                          \
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

enum MediaType {
	IMAGE_MEDIA = 0,
	VIDEO_MEDIA,
	IMAGE_AND_VIDEO_MEDIA,
	MEDIA_TYPE_NUM
};

enum ParamIndex {
	PARAM_1920x1080_D65_1000Lux = 0,
	PARAM_INDEX_NUM
};

enum ParamMgrSate {
	PM_EMPTY = 0,
	PM_SELECTED
};

enum RawFormat {
	ANDROID_RAW10 = 0,
	ORDINAL_RAW10,
	UNPACKAGED_RAW10_LSB,
	UNPACKAGED_RAW10_MSB,
	RAW_FORMAT_NUM
};

enum ColorSpace {
	CS_Bayer = 0,
	CS_YUV,
	CS_RGB,
	CS_NUM
};

enum DataPackageType {
	DPT_Packaged = 0,
	DPT_Unpackaged,
	DPT_NUM
};

enum BayerOrder {
	BO_BGGR = 0,
	BO_GBRG,
	BO_GRBG,
	BO_RGGB,
	BO_NUM
};

struct ImgInfo
{
	int32_t width;
	int32_t height;
	int32_t bitspp;
	int32_t stride;
	int32_t rawFormat;
	int32_t bayerOrder;
};

struct VideoInfo
{
	int32_t fps;
	int32_t frameNum;
};

struct MediaInfo
{
	ImgInfo img;
	VideoInfo video;
	int32_t type;
};

struct ISPCfgParams {
	void *pBlcParam;
	void *pLscParam;
	void *pWbParam;
	void *pCcParam;
	void *pGammaParam;
	void *pWnrParam;
	void *pEeParam;
};

class ISPParamManager {
public:
	ISPParamManager();
	~ISPParamManager();

	int32_t SelectParams(int32_t paramIndex);
	int32_t SetMediaInfo(MediaInfo* info);
	int32_t SetImgInfo(ImgInfo* info);
	int32_t SetVideoInfo(VideoInfo* info);
	int32_t GetImgDimension(int32_t* width, int32_t* height);
	int32_t GetVideoFPS(int32_t* fps);
	int32_t GetVideoFrameNum(int32_t* num);

	int32_t GetImgInfo(void* pParams);
	int32_t GetParamByCMD(void* pParams, int32_t cmd);
	int32_t GetBLCParam(void* pParams);
	int32_t GetLSCParam(void* pParams);
	int32_t GetWBParam(void* pParams);
	int32_t GetCCParam(void* pParams);
	int32_t GetGAMMAParam(void* pParams);
	int32_t GetWNRParam(void* pParams);
	int32_t GetEEParam(void* pParams);

private:
	ISPCfgParams mISPConfigParams;
	MediaInfo mMediaInfo;
	int32_t mState;
};
