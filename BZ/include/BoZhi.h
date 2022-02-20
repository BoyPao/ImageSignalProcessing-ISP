// License: GPL-3.0-or-later
/*
 * BoZhi object head file.
 *
 * Copyright (c) 2019 Peng Hao <635945005@qq.com>
 */

#include <thread>
#include <mutex>
#include <atomic>
#include <condition_variable>

#include "BZLog.h"
#include "LibInterface.h"

enum BZ_STATE {
	BZ_STATE_NEW = 0,
	BZ_STATE_INITED,
	BZ_STATE_NUM
};

class BoZhi {
public:
	BoZhi();
	~BoZhi();

	BZResult Init();
	BZResult DeInit();
	BZResult ExecuteCMD();

	ISP_CALLBACKS mISPCBs;
	LIB_MSG mMsg;

private:
	BZ_STATE mState;
};

int32_t WrapLibInit(void* pOPS);
int32_t WrapLibDeInit();
int32_t WrapRegistCallbacks(void* pOPS);
