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

enum BZState {
	BZ_STATE_NEW = 0,
	BZ_STATE_INITED,
	BZ_STATE_NUM
};

class BoZhi {
public:
	BoZhi();
	~BoZhi();

	int32_t Init();
	int32_t DeInit();
	int32_t RegisterCallbacks(void *pCBs);
	ISPCallbacks const* GetCallbacks();
	int32_t ExecuteCMD();
	BZMsg mMsg;
	ISPCallbacks mISPCBs;

private:
	int32_t mState;
};

int32_t WrapLibInit(void* pOPS);
int32_t WrapLibDeInit();
int32_t WrapRegistCallbacks(void* pOPS);
void* WrapGetBoZhi();
void* WrapAlloc(size_t size);
void* WrapAlloc(size_t size, size_t num);
void* WrapFree(void* pBuf);

