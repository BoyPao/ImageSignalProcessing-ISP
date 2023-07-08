// License: GPL-3.0-or-later
/*
 * ParamManager aims to manage multi params.
 *
 * Copyright (c) 2019 Peng Hao <635945005@qq.com>
 */

#pragma once
#include <mutex>
#include <list>
#include "Utils.h"
#include "Settings.h"

using namespace std;

#define CHECK_PACKAGED(format)				\
	(((format) == UNPACKAGED_RAW10_LSB) ||	\
	 ((format) == UNPACKAGED_RAW10_MSB)) ? 0 : 1

#define PIXELS2WORDS_MIPI_PACKAGED(pixelNum, bitspp)	\
	(((bitspp) < BITS_PER_WORD) ? 0 :					\
	 (((bitspp) == BITS_PER_WORD) ?	(pixelNum) :		\
	  ((pixelNum) * (bitspp) / BITS_PER_WORD)))

#define	PIXELS2WORDS_UNPACKAGED(pixelNum, bitspp)		\
	(((bitspp) < BITS_PER_WORD) ? 0 :					\
	 (((bitspp) == BITS_PER_WORD) ?	(pixelNum) :		\
	  ((pixelNum) * 2)))

#define PIXELS2WORDS(pixelNum, bitspp, packaged)							\
	(((bitspp) % 2 || (bitspp) > 2 * BITS_PER_WORD || (bitspp) <= 0) ? 0 :	\
	 ((packaged) ? PIXELS2WORDS_MIPI_PACKAGED(pixelNum, bitspp) :			\
	  PIXELS2WORDS_UNPACKAGED(pixelNum, bitspp)))

#define ALIGN(x, align) \
	(align) ? (((x) + (align) - 1) & (~((align) - 1))) : (x)

#define ALIGNx(pixelNum, bitspp, packaged, align) \
	ALIGN(PIXELS2WORDS(pixelNum, bitspp, packaged), align)

enum MediaType {
	IMAGE_MEDIA = 0,
	VIDEO_MEDIA,
	IMAGE_AND_VIDEO_MEDIA,
	MEDIA_TYPE_NUM
};

enum SettingIndex {
	SETTING_1920x1080_D65_1000Lux = 0,
	SETTING_INDEX_NUM
};

enum ParamMgrSate {
	PM_STATE_UNINIT = 0,
	PM_STATE_MEDIA_INFO_SET,
	PM_STATE_NUM
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

struct ISPParamBuf {
	void *addr;
	size_t size;
};

struct ISPParamInfo {
	int32_t id;
	int32_t settingIndex;
	ISPParamBuf buf;
};

struct ISPSetting {
	void *pBlc;
	void *pLsc;
	void *pWb;
	void *pCc;
	void *pGamma;
	void *pWnr;
	void *pEe;
};

class ISPParamManagerItf {
public:
	virtual ~ISPParamManagerItf() {};

	virtual int32_t SetMediaInfo(MediaInfo* info) = 0;
	virtual int32_t GetImgDimension(int32_t* width, int32_t* height) = 0;
	virtual int32_t GetVideoFPS(int32_t* fps) = 0;
	virtual int32_t GetVideoFrameNum(int32_t* num) = 0;
	virtual int32_t CreateParam(int32_t hostId, int32_t settingId) = 0;
	virtual int32_t DeleteParam(int32_t hostId) = 0;
	virtual void* GetParam(int32_t id, int32_t type) = 0;
};

class ISPParamManager : public ISPParamManagerItf {
public:
	static ISPParamManager* GetInstance();
	virtual int32_t SetMediaInfo(MediaInfo* info);
	virtual int32_t GetImgDimension(int32_t* width, int32_t* height);
	virtual int32_t GetVideoFPS(int32_t* fps);
	virtual int32_t GetVideoFrameNum(int32_t* num);
	virtual int32_t CreateParam(int32_t hostId, int32_t settingId);
	virtual int32_t DeleteParam(int32_t hostId);
	virtual void* GetParam(int32_t id, int32_t type);

private:
	ISPParamManager();
	virtual ~ISPParamManager();

	ISPParamInfo *GetParamInfoById(int id);
	void* GetParamByType(ISPParamInfo *pInfo, int32_t type);
	int32_t FillinParam(ISPParamInfo *pParamInfo);
	int32_t FillinParamByType(ISPParamInfo *pInfo, int32_t type);
	int32_t SetImgInfo(ImgInfo* info);
	int32_t SetVideoInfo(VideoInfo* info);
	int32_t FillinImgInfo(void *pInfo);
	int32_t FillinBLCParam(void *pParam, ISPSetting *pSetting);
	int32_t FillinLSCParam(void *pParam, ISPSetting *pSetting);
	int32_t FillinWBParam(void *pParam, ISPSetting *pSetting);
	int32_t FillinCCParam(void *pParam, ISPSetting *pSetting);
	int32_t FillinGAMMAParam(void *pParam, ISPSetting *pSetting);
	int32_t FillinWNRParam(void *pParam, ISPSetting *pSetting);
	int32_t FillinEEParam(void *pParam, ISPSetting *pSetting);
	void DumpParamInfo();

	mutex mParamListLock;
	list<ISPParamInfo> mActiveParamList;
	MediaInfo mMediaInfo;
	int32_t mState;
};
