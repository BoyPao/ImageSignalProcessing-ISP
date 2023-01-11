// License: GPL-3.0-or-later
/*
 * Define memory pool.
 *
 * Copyright (c) 2019 Peng Hao <635945005@qq.com>
 */
#include <vector>
#include <list>
#include <mutex>
#include <unordered_map>
#include "Utils.h"

#if DBG_MEM_OVERWRITE_CHECK_ON
#include <thread>
#endif

#define DBG_MEM_PRINT_ON 0

#define MEM_BLK_L0_MAX_NUM 16
#define MEM_BLK_L1_MAX_NUM 8
#define MEM_BLK_L2_MAX_NUM 4

#define MEM_BLK_L0_SIZE 1024 * 1024
#define MEM_BLK_L1_SIZE MEM_BLK_L0_MAX_NUM * MEM_BLK_L0_SIZE
#define MEM_BLK_L2_SIZE MEM_BLK_L1_MAX_NUM * MEM_BLK_L1_SIZE

using namespace std;

enum MemBlockLevel {
	MEM_BLK_L0 = 0,
	MEM_BLK_L1,
	MEM_BLK_L2,
	MEM_BLK_LEVEL_NUM
};

struct MemBlockInfo {
	int32_t level;
	size_t max;
	size_t size;
};

const MemBlockInfo gMemBlockCfg[MEM_BLK_LEVEL_NUM] = {
	{MEM_BLK_L0, MEM_BLK_L0_MAX_NUM, MEM_BLK_L0_SIZE},
	{MEM_BLK_L1, MEM_BLK_L1_MAX_NUM, MEM_BLK_L1_SIZE},
	{MEM_BLK_L2, MEM_BLK_L2_MAX_NUM, MEM_BLK_L2_SIZE},
};

struct MemSegment {
	void* pAddr;
	size_t size;
};

struct MemBlock {
	void* blockBase;
	size_t blockSize;
	size_t busySize;
	list<MemSegment> idleList;
	list<MemSegment> busyList;
};

enum MemSybmolOperations {
	MSO_PUSH = 0,
	MSO_POP,
	MSO_NUM
};

#if DBG_MEM_OVERWRITE_CHECK_ON
#define OVERWRITE_CHECK_TIME_GAP_US (100)
#define OVERWRITE_CHECK_SIZE	((8 + 1) * sizeof(char)) /* one for '\0' */
const char gOverwiteSymbol[OVERWRITE_CHECK_SIZE] =
		{'@', 'D', 'B', 'G',
		 'M', 'E', 'M', '@',
		 '\0'};

struct MemThreadParam {
	unordered_map<void*, void*>* pSymbolMap;
	uint32_t exit;
	mutex* pLock;
};

void* OverwriteCheckingFunc(void* param);
#endif

template <typename T>
class MemoryPoolItf
{
	public:
		virtual ~MemoryPoolItf() {};
		virtual T* RequireBuffer(size_t TSize) = 0;
		virtual T* RevertBuffer(T* pBuffer) = 0;
};

template <typename T>
class MemoryPool : public MemoryPoolItf<T>
{
	public:
		static MemoryPool<T>* GetInstance();
		virtual T* RequireBuffer(size_t TSize);
		virtual T* RevertBuffer(T* pBuffer);

	private:
		MemoryPool();
		virtual ~MemoryPool();
		int32_t AllocBlock(int32_t level);
		int32_t ReleaseBlock(int32_t level, u_int32_t index);
		int32_t PrintPool();
		int32_t MemoryReset(void* pAddr, size_t size, int32_t operation);
		MemBlockInfo const* mMemBlockCfg[MEM_BLK_LEVEL_NUM];
		vector<MemBlock> mUsageInfo[MEM_BLK_LEVEL_NUM];
		mutex mUsageInfoLock;
#if DBG_MEM_OVERWRITE_CHECK_ON
		unordered_map<void*, void*> mSymbolMap;
		thread dbgThread;
		MemThreadParam mThreadParam;
#endif
};

template <typename T>
MemoryPool<T>* MemoryPool<T>::GetInstance()
{
	static MemoryPool<T> gInstance;
	return &gInstance;
}

template <typename T>
MemoryPool<T>::MemoryPool()
{
	for (int32_t level = 0; level < MEM_BLK_LEVEL_NUM; level++) {
		mMemBlockCfg[level] = &gMemBlockCfg[level];
	}

#if DBG_MEM_OVERWRITE_CHECK_ON
	mThreadParam.pSymbolMap = &mSymbolMap;
	mThreadParam.exit = 0;
	mThreadParam.pLock = &mUsageInfoLock;
	dbgThread = thread(OverwriteCheckingFunc, (void*)&mThreadParam);
#endif
}

template <typename T>
MemoryPool<T>::~MemoryPool()
{
	for (int32_t level = 0; level < MEM_BLK_LEVEL_NUM; level++) {
		while(mUsageInfo[level].size()) {
			ReleaseBlock(level, 0);
		}
		mMemBlockCfg[level] = NULL;
	}
	PrintPool();

#if DBG_MEM_OVERWRITE_CHECK_ON
	mThreadParam.exit = 1;
	dbgThread.join();
	mThreadParam.pLock = NULL;
#endif
}

template <typename T>
int32_t MemoryPool<T>::AllocBlock(int32_t level)
{
	int32_t rt = ISP_SUCCESS;

	if (mUsageInfo[level].size() >= mMemBlockCfg[level]->max) {
		rt = ISP_INVALID_PARAM;
		ILOGE("Level:%d overflow", level);
	}

	if (SUCCESS(rt)) {
		MemBlock blk = { 0 };
		blk.blockBase = new u_char[mMemBlockCfg[level]->size];
		if (!blk.blockBase) {
			rt = ISP_MEMORY_ERROR;
			ILOGE("Level:%d alloc %u failed!", level, mMemBlockCfg[level]->size);
		}

		if (SUCCESS(rt))
		{
			unique_lock <mutex> lock(mUsageInfoLock);
			blk.blockSize = mMemBlockCfg[level]->size;
			blk.busySize = 0;
			MemSegment idleSeg = {0};
			idleSeg.pAddr = blk.blockBase;
			idleSeg.size = blk.blockSize;
			blk.idleList.push_back(idleSeg);
			mUsageInfo[level].push_back(blk);
			memset(blk.blockBase, 0, blk.blockSize);
			ILOGDM("L%d B%d: alloc %u", level, mUsageInfo[level].size() - 1, blk.blockSize);
		}
	}

	return rt;
}

template <typename T>
int32_t MemoryPool<T>::ReleaseBlock(int32_t level, u_int32_t index)
{
	int32_t rt = ISP_SUCCESS;

	if (index >= mUsageInfo[level].size()) {
		rt = ISP_INVALID_PARAM;
		ILOGE("Invalid block index:%d", index);
	}

	if (SUCCESS(rt)) {
		u_char* p = static_cast<u_char*>(mUsageInfo[level][index].blockBase);
		if (p)
		{
			unique_lock <mutex> lock(mUsageInfoLock);
			while(mUsageInfo[level][index].idleList.begin() !=
					mUsageInfo[level][index].idleList.end()) {
				mUsageInfo[level][index].idleList.pop_back();
			}

			while(mUsageInfo[level][index].busyList.begin() !=
					mUsageInfo[level][index].busyList.end()) {
				auto seg = mUsageInfo[level][index].busyList.end();
				seg--;
				rt = MemoryReset(seg->pAddr, seg->size, MSO_POP);
				ILOGDM("%u is reverted", seg->size);
				mUsageInfo[level][index].busyList.pop_back();
			}

			delete[] p;
			size_t blkSize = mUsageInfo[level][index].blockSize;
			mUsageInfo[level][index].blockSize = 0;
			mUsageInfo[level][index].busySize = 0;
			mUsageInfo[level].erase(mUsageInfo[level].begin() + index);
			ILOGDM("L%d B%d: release %u", level, index, blkSize);
			if (!mUsageInfo[level].size()) {
				ILOGDM("L%d all released", level);
			}
		} else {
			rt = ISP_FAILED;
			ILOGE("Fatal ERROR!");
		}
	}

	return rt;
}

template <typename T>
int32_t MemoryPool<T>::PrintPool()
{
	int32_t rt = ISP_SUCCESS;
#if DBG_MEM_PRINT_ON
	int32_t segCnt = 0;
	ILOGDM("<<<<<<<<<<<<<<<<<<<<<< MEM STATE >>>>>>>>>>>>>>>>>>>>>>>");
	for (int32_t level = 0; level < MEM_BLK_LEVEL_NUM; level++) {
		for (auto blk = mUsageInfo[level].begin(); blk != mUsageInfo[level].end(); blk++) {
			ILOGDM("L%d B%d(%u) addr:%-8p size:%-8u used:%.2f%%",
					level,
					blk - mUsageInfo[level].begin(),
					mUsageInfo[level].size(),
					blk->blockBase,
					blk->blockSize,
					(float)blk->busySize * 100 / blk->blockSize);
			segCnt = 0;
			for (auto seg = blk->idleList.begin(); seg != blk->idleList.end(); seg++) {
				ILOGDM("    Idle: S%d(%u) addr:%-8p size:%-8u",
						segCnt,
						blk->idleList.size(),
						seg->pAddr,
						seg->size);
				segCnt++;
			}
			segCnt = 0;
			for (auto seg = blk->busyList.begin(); seg != blk->busyList.end(); seg++) {
				ILOGDM("    Busy: S%d(%u) addr:%-8p size:%-8u",
						segCnt,
						blk->busyList.size(),
						seg->pAddr,
						seg->size);
				segCnt++;
			}
		}
	}
#endif

	return rt;
}

template <typename T>
T* MemoryPool<T>::RequireBuffer(size_t TSize)
{
	int32_t rt = ISP_SUCCESS;
	T* pBuffer = NULL;
	size_t size = TSize * sizeof(T);
#if DBG_MEM_OVERWRITE_CHECK_ON
	size += OVERWRITE_CHECK_SIZE;
#endif

	if (!size || size > mMemBlockCfg[MEM_BLK_LEVEL_NUM - 1]->size) {
		rt = ISP_INVALID_PARAM;
		ILOGE("Require invalid size:C%u (N%u x T%u) ", size, sizeof(T), TSize);
	}

	/* 1. Search valid segment */
	if (SUCCESS(rt)) {
		for (int32_t level = 0; level < MEM_BLK_LEVEL_NUM; level++) {
			if (rt == ISP_SKIP) {
				break;
			}
			if (size > mMemBlockCfg[level]->size) {
				continue;
			}
			for (auto blk = mUsageInfo[level].begin(); blk != mUsageInfo[level].end(); blk++) {
				if (rt == ISP_SKIP) {
					break;
				}
				if (size > blk->blockSize - blk->busySize) {
					continue;
				}
				for (auto seg = blk->idleList.begin(); seg != blk->idleList.end(); seg++) {
					if (size <= seg->size)
					{
						unique_lock <mutex> lock(mUsageInfoLock);
						MemSegment idleSeg = {0};
						MemSegment busySeg = {0};
						busySeg.pAddr = seg->pAddr;
						busySeg.size = size;
						rt = MemoryReset(busySeg.pAddr, busySeg.size, MSO_PUSH);
						blk->busyList.push_front(busySeg);

						pBuffer = static_cast<T*>(busySeg.pAddr);

						idleSeg.pAddr = (void*)(static_cast<T*>(seg->pAddr) + size);
						idleSeg.size = seg->size - size;
						blk->idleList.erase(seg);
						blk->idleList.push_front(idleSeg);

						blk->busySize += size;
						rt = ISP_SKIP;
						break;
					}
				}
			}
		}
	}

	/* 2. Alloc new block */
	if (rt == ISP_SUCCESS) {
		for (int32_t level = 0; level < MEM_BLK_LEVEL_NUM; level++) {
			if (size > mMemBlockCfg[level]->size) {
				if (level == MEM_BLK_LEVEL_NUM - 1) {
					rt = ISP_MEMORY_ERROR;
					ILOGE("Reguire buffer(%u) failed!", size);
				}
			} else {
				rt = AllocBlock(level);
				if (SUCCESS(rt))
				{
					unique_lock <mutex> lock(mUsageInfoLock);
					MemBlock* blk = &mUsageInfo[level].back();
					list<MemSegment>::iterator seg = blk->idleList.end();
					seg--;

					MemSegment idleSeg = {0};
					MemSegment busySeg = {0};
					busySeg.pAddr = seg->pAddr;
					busySeg.size = size;
					rt = MemoryReset(busySeg.pAddr, busySeg.size, MSO_PUSH);
					blk->busyList.push_front(busySeg);

					pBuffer = static_cast<T*>(busySeg.pAddr);

					idleSeg.pAddr = (void*)(static_cast<T*>(seg->pAddr) + size);
					idleSeg.size = seg->size - size;
					blk->idleList.erase(seg);
					blk->idleList.push_front(idleSeg);

					blk->busySize += size;
					rt = ISP_SKIP;
					break;
				}
			}
		}
	}

	if (SUCCESS(rt)) {
		ILOGDM("C%u (N%u x T%u) is required", size, sizeof(T), TSize);
		rt = PrintPool();
	}

	return pBuffer;
}

template <typename T>
T* MemoryPool<T>::RevertBuffer(T* pBuffer)
{
	int32_t rt = ISP_SUCCESS;
	size_t size = 0;
	void *pVBuf = (void*)pBuffer;

	if (!pBuffer) {
		rt = ISP_INVALID_PARAM;
		ILOGE("Invalid buffer:%p", pBuffer);
	}

	for (int32_t level = 0; level < MEM_BLK_LEVEL_NUM; level++) {
		if (rt != ISP_SUCCESS) {
			break;
		}
		for (auto blk = mUsageInfo[level].begin(); blk != mUsageInfo[level].end(); blk++) {
			if (static_cast<u_char*>(pVBuf) < blk->blockBase || static_cast<u_char*>(pVBuf) >= static_cast<u_char*>(blk->blockBase) + blk->blockSize) {
				continue;
			}
			for (auto seg = blk->busyList.begin(); seg != blk->busyList.end(); seg++) {
				if (static_cast<u_char*>(pVBuf) == seg->pAddr)
				{
					unique_lock <mutex> lock(mUsageInfoLock);
					bool isNewSeg = true;
					size = seg->size;
					memset(seg->pAddr, 0, seg->size);
					rt = MemoryReset(seg->pAddr, seg->size, MSO_POP);

					MemSegment tmpSeg = {0};
					tmpSeg.pAddr = (void*)seg->pAddr;
					tmpSeg.size = seg->size;

					/* 1. Forward merge */
					u_char* nextAddr = static_cast<u_char*>(tmpSeg.pAddr) + tmpSeg.size;
					for (auto idleSeg = blk->idleList.begin(); idleSeg != blk->idleList.end(); idleSeg++) {
						if (nextAddr == idleSeg->pAddr) {
							tmpSeg.size += idleSeg->size;
							blk->idleList.erase(idleSeg);
							break;
						}
					}

					/* 2. Backward merge */
					for (auto idleSeg = blk->idleList.begin(); idleSeg != blk->idleList.end(); idleSeg++) {
						u_char* lastAddr = static_cast<u_char*>(idleSeg->pAddr) + idleSeg->size;
						if (lastAddr == tmpSeg.pAddr) {
							idleSeg->size += tmpSeg.size;
							isNewSeg = false;
							break;
						}
					}

					if (isNewSeg) {
						blk->idleList.push_front(tmpSeg);
					}
					blk->busyList.erase(seg);
					blk->busySize -= size;
					pBuffer = NULL;
					rt = ISP_SKIP;
					break;
				}
			}
			if (rt != ISP_SKIP) {
				rt = ISP_INVALID_PARAM;
				ILOGE("Fatal Error: cannot match buffer:%p in segment", pBuffer);
			}
			break;
		}
		if (rt != ISP_SKIP && level == MEM_BLK_LEVEL_NUM - 1) {
			rt = ISP_INVALID_PARAM;
			ILOGE("Invalid buffer:%p not in pool", pBuffer);
		}
	}

	if (SUCCESS(rt)) {
		ILOGDM("C%u (N%u x T%u) is reverted", size, sizeof(T), size / sizeof(T));
		rt = PrintPool();
	}

	return pBuffer;
}

template <typename T>
int32_t MemoryPool<T>::MemoryReset(void* pAddr, size_t size, int32_t operation)
{
	int32_t rt = ISP_SUCCESS;
#if DBG_MEM_OVERWRITE_CHECK_ON
	if (operation == MSO_PUSH) {
		memcpy(static_cast<u_char*>(pAddr) + size - OVERWRITE_CHECK_SIZE, gOverwiteSymbol, OVERWRITE_CHECK_SIZE);
		mSymbolMap.insert(pair<void*, void*>(pAddr, static_cast<u_char*>(pAddr) + size - OVERWRITE_CHECK_SIZE));
	} else if (operation == MSO_POP) {
		mSymbolMap.erase(pAddr);
	} else {
		ILOGE("Invalid operation:%d", operation);
	}
#endif
	return rt;
}
