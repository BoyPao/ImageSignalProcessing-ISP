// License: GPL-3.0-or-later
/*
 * Default medio infomation.
 *
 * Copyright (c) 2019 Peng Hao <635945005@qq.com>
 */
#include "ParamManager.h"

#ifdef LINUX_SYSTEM
#define INPUT_PATH "/home2/penghao/test_project/ISP/ISP/res/"
#define OUTPUT_PATH "/home2/penghao/test_project/ISP/ISP/res/"
#elif defined WIN32_SYSTEM
#define INPUT_PATH "D:\\test_project\\ISP_NEW\\ISP_NEW\\ISP_NEW\\res\\"
#define OUTPUT_PATH "D:\\test_project\\ISP_NEW\\ISP_NEW\\ISP_NEW\\res\\"
#endif

#define DEFAULT_FILE_NAME "1MCC_IMG_20181229_001526_1"
#define INPUT_NAME DEFAULT_FILE_NAME".raw"
#define OUTPUT_IMG_NAME DEFAULT_FILE_NAME".bmp"
#define OUTPUT_VIDEO_NAME DEFAULT_FILE_NAME".avi"

const MEDIA_INFO defaultMediaInfo {
	/* width, height, bitspp, stride, format, channel order */
	{1920, 1080, 10, 0, ANDROID_RAW10, BO_BGGR},
	/* FPS, max frame num*/
	{30, 30},
	/* media type: IMAGE_MEDIA, VIDEO_MEDIA */
	IMAGE_MEDIA,
//	IMAGE_AND_VIDEO_MEDIA,
};
