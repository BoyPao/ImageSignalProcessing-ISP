// License: GPL-3.0-or-later
/*
 * Memory pool implementation.
 *
 * Copyright (c) 2019 Peng Hao <635945005@qq.com>
 */

#include "MemPool.h"

#if DBG_MEM_OVERWRITE_CHECK_ON
void* OverwriteCheckingFunc(void* param)
{
	MemThreadParam *pParam = static_cast<MemThreadParam*>(param);
	ILOGDM("Overwrite checking start");

	if (!pParam) {
		ILOGE("Invalid thread param");
		return NULL;
	}

	uint8_t overwriteDetected = 0;
	while(1 && !pParam->exit) {
		{
			unique_lock <mutex> lock(*(pParam->pLock));

			if (!pParam->pSymbolMap->empty()) {
				for (auto it = pParam->pSymbolMap->begin(); it != pParam->pSymbolMap->end(); it++) {
					overwriteDetected = 0;
					for(size_t index = 0; index < OVERWRITE_CHECK_SIZE; index++) {
						overwriteDetected |=
							~(static_cast<uchar*>(it->second)[index]) &
							gOverwiteSymbol[index];
					}
					if (overwriteDetected) {
						ILOGE("Fatal Error 0x%x: ================ MEMORY OVERWRITE DETECTED ==============",
								overwriteDetected);
						ILOGE("addr:%p symbleAddr:%p symble:%s",
								it->first, it->second,
								it->second);
						std::abort();
						break;
					}
				}
			}
		}
		usleep(OVERWRITE_CHECK_TIME_GAP_US);
	}

	ILOGDM("Overwrite checking end");
	return 0;
}
#endif
