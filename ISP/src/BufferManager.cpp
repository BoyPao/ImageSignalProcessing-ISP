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
	}

	ILOGDM("Overwrite checking end");
	return 0;
}
#endif
