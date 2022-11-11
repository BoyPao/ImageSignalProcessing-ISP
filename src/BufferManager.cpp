// License: GPL-3.0-or-later
/*
 * Buffer manager implementation.
 *
 * Copyright (c) 2019 Peng Hao <635945005@qq.com>
 */

#include "BufferManager.h"

#if DBG_MEM_OVERWRITE_CHECK_ON
void* OverwriteCheckingFunc(void* param)
{
	int32_t rt = ISP_SUCCESS;
	MemThreadParam *pParam = static_cast<MemThreadParam*>(param);
	ILOGDM("Overwrite checking start");

	if (!pParam) {
		rt = ISP_INVALID_PARAM;
		ILOGE("Invalid thread param");
	}

	if (SUCCESS(rt)) {
		while(1 && (pParam->threadOn == 1)) {
			{
				unique_lock <mutex> lock(*(pParam->pLock));
				for (int32_t level = 0; level < MEM_BLK_LEVEL_NUM; level++) {
					if (!pParam->pUsageInfo[level]->size()) {
						continue;
					}
					for (auto blk = pParam->pUsageInfo[level]->begin(); blk != pParam->pUsageInfo[level]->end(); blk++) {
						if (!blk->busyList.size()) {
							continue;
						}
						int32_t segCnt = 0;
						for (auto seg = blk->busyList.begin(); seg != blk->busyList.end(); seg++) {
							uint8_t overwriteDetected = 0;
							for(size_t index = 0; index < OVERWRITE_CHECK_SIZE; index++) {
								overwriteDetected |=
									~(static_cast<u_char*>(seg->pAddr)[seg->size - OVERWRITE_CHECK_SIZE + index]) &
									gOverwiteSymbol[index];
							}
							if (overwriteDetected) {
								ILOGE("Fatal Error 0x%x: ================ MEMORY OVERWRITE DETECTED ==============", overwriteDetected);
								ILOGE("L%d B%d(%u) busy S%d(%u) addr:%p symbleAddr: %p symble:%s",
										level, blk - pParam->pUsageInfo[level]->begin(),
										pParam->pUsageInfo[level]->size(),
										segCnt, blk->busyList.size(), seg->pAddr,
										static_cast<u_char*>(seg->pAddr) + seg->size - OVERWRITE_CHECK_SIZE,
										static_cast<u_char*>(seg->pAddr) + seg->size - OVERWRITE_CHECK_SIZE);
							}
							segCnt++;
						}
					}
				}
			}
			usleep(OVERWRITE_CHECK_TIME_GAP_US);
		}
	}

	ILOGDM("Overwrite checking end");
	return 0;
}
#endif
