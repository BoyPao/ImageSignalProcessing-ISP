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
#include <cuchar>
#include <map>

#include "Processor.h"

enum BZState {
	BZ_STATE_NEW = 0,
	BZ_STATE_INITED,
	BZ_STATE_NUM
};

class BoZhiItf {
public:
	virtual ~BoZhiItf() {};

	virtual int32_t Init() = 0;
	virtual int32_t DeInit() = 0;
	virtual int32_t RegisterCallbacks(void *pCBs) = 0;
	virtual int32_t Event(uint32_t *msg) = 0;
	virtual ISPCallbacks const* GetCallbacks() = 0;
};

class BoZhi : public BoZhiItf {
public:
	static BoZhi *GetInstance();
	int32_t Init();
	int32_t DeInit();
	int32_t RegisterCallbacks(void *pCBs);
	int32_t Event(uint32_t *msg);
	ISPCallbacks const* GetCallbacks();

private:
	BoZhi();
	virtual ~BoZhi();

	int32_t CreateProcessor(int32_t id);
	int32_t DestroyProcessorById(int32_t id);
	int32_t DestroyAllProcessor();
	Processor *FindProcessorById(int32_t id);
	int32_t Process(uint32_t *msg);
	void PrintMessage(uint32_t *msg);
	ISPCallbacks mISPCBs;
	int32_t mState;
	std::map<int32_t, Processor*> mProcMap;
	mutex procMapLock;
};

int32_t WrapLibInit(void* pOPS);
int32_t WrapLibDeInit();
int32_t WrapRegistCallbacks(void* pOPS);
void* WrapAlloc(size_t size);
void* WrapAlloc(size_t size, size_t num);
void* WrapFree(void* pBuf);

