// License: GPL-3.0-or-later
/*
 * Default medio infomation.
 *
 * Copyright (c) 2019 Peng Hao <635945005@qq.com>
 */
#include "ParamManager.h"
#include "ISPConfig.h"

#define INPUT_PATH (WORK_PATH RES_PATH PATH_FMT)
#define OUTPUT_PATH (WORK_PATH RES_PATH PATH_FMT)

#define DEFAULT_FILE_NAME "1MCC_IMG_20181229_001526_1"
#define INPUT_NAME DEFAULT_FILE_NAME".raw"
#define OUTPUT_IMG_NAME DEFAULT_FILE_NAME".bmp"
#define OUTPUT_VIDEO_NAME DEFAULT_FILE_NAME".avi"

const MediaInfo defaultMediaInfo {
	/* width, height, bitspp, stride, format, channel order */
	{1920, 1080, 10, 0, ANDROID_RAW10, BO_BGGR},
	/* FPS, max frame num*/
	{30, 30},
	/* media type: IMAGE_MEDIA, VIDEO_MEDIA */
	IMAGE_MEDIA,
//	IMAGE_AND_VIDEO_MEDIA,
};
