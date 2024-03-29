// License: GPL-3.0-or-later
/*
 * I/O helper.
 *
 * Copyright (c) 2019 Peng Hao <635945005@qq.com>
 */

#include "IOHelper.h"

IOHelper::IOHelper()
{
	pStaticInfo = (void*)&defaultMediaInfo;
	pDynamicInfo = (void*) new MediaInfo;
	if (!pDynamicInfo) {
		ILOGE("Faild to malloc dynamic info buffer!");
	}
}

IOHelper::~IOHelper()
{
	pStaticInfo = NULL;
	if (pDynamicInfo) {
		delete static_cast<MediaInfo*>(pDynamicInfo);
		pDynamicInfo = NULL;
	}
}
