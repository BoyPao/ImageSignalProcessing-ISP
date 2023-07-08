// License: GPL-3.0-or-later
/*
 * Memory pool test.
 *
 * Copyright (c) 2019 Peng Hao <635945005@qq.com>
 */

#include "Utils.h"
#include "MemPool.h"

#define OVERWRITE_OFFSET 0

int main() {
	ILOGI("MEM TEST S");
	MemoryPoolItf<char> *gMemPool = MemoryPool<char>::GetInstance();

	if (gMemPool) {
		char* pc = NULL;
		size_t size = 1920 * 1080;
		char* p1 = gMemPool->RequireBuffer(size);
		char* p2 = gMemPool->RequireBuffer(size);
		pc = p2;
		memset(pc, 1, size);
		ILOGI("memset p:%p size:%u", pc, size);
		char* p3 = gMemPool->RequireBuffer(size);
#if DBG_MEM_OVERWRITE_CHECK_ON
		pc = p3;
		memset(pc, 1, size + OVERWRITE_OFFSET);
		ILOGE("memset p:%p size:%u", pc, size + OVERWRITE_OFFSET);
		pc = p3;
		memset(pc, 1, size + OVERWRITE_OFFSET);
		ILOGE("memset p:%p size:%u", pc, size + OVERWRITE_OFFSET);
#endif
		char* p4 = gMemPool->RequireBuffer(size);
#if DBG_MEM_OVERWRITE_CHECK_ON
		pc = p4;
		memset(pc, 1, size + OVERWRITE_OFFSET);
		ILOGE("memset p:%p size:%u", pc, size + OVERWRITE_OFFSET);
		usleep(100 * OVERWRITE_CHECK_TIME_GAP_US);
#endif

		size = 4608 * 3456;
		char* p5 = gMemPool->RequireBuffer(size);
		gMemPool->RevertBuffer(p2);
		gMemPool->RevertBuffer(p4);
		gMemPool->RevertBuffer(p3);

		size = 1920 * 1080;
		p2 = gMemPool->RequireBuffer(size);
		gMemPool->RevertBuffer(p2);
		gMemPool->RevertBuffer(p1);
		gMemPool->RevertBuffer(p5);

		float* pf = NULL;
		size = 1920 * 1080;
		void* p6 = (void*)gMemPool->RequireBuffer(size * sizeof(float));
		pf = static_cast<float*>(p6);
		memset((void*)pf, 0.1,  size * sizeof(float));
		ILOGI("memset p:%p size:%u", pf, size * sizeof(float));

		void* p7 = (void*)gMemPool->RequireBuffer(size * sizeof(float));
#if DBG_MEM_OVERWRITE_CHECK_ON
		pf = static_cast<float*>(p7);
		memset((void*)pf, 0.1, (size + OVERWRITE_OFFSET) * sizeof(float));
		ILOGE("memset p:%p size:%u", pf, (size + OVERWRITE_OFFSET) * sizeof(float));
		usleep(100 * OVERWRITE_CHECK_TIME_GAP_US);
#endif

		void* p8 = (void*)gMemPool->RequireBuffer(size * sizeof(float));
		gMemPool->RevertBuffer(static_cast<char*>(p6));
		gMemPool->RevertBuffer(static_cast<char*>(p7));
		gMemPool->RevertBuffer(static_cast<char*>(p8));
	} else {
		ILOGE("Failed to create MemoryPool");
	}

	ILOGI("MEM TEST E");

	return 0;
}
