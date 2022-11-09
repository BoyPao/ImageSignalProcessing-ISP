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

#include "LibInterface.h"
#include "BZUtils.h"
#include <cuchar>

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
	BZResult RegisterCallbacks(void *pCBs);
	ISP_CALLBACKS const* GetCallbacks();
	BZResult ExecuteCMD();
	LIB_MSG mMsg;
	ISP_CALLBACKS mISPCBs;

private:
	BZ_STATE mState;
};

int32_t WrapLibInit(void* pOPS);
int32_t WrapLibDeInit();
int32_t WrapRegistCallbacks(void* pOPS);
void* WrapGetBoZhi();
u_char* WrapAlloc(size_t size);
u_char* WrapFree(u_char* pBuf);

