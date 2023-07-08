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

enum MemOperations {
	MO_REQ = 0,
	MO_REV,
	MO_NUM
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
		int32_t ReleaseBlock(int32_t level, uint32_t index);
		int32_t PrintPool();
		int32_t MemoryReset(void* pAddr, size_t size, int32_t operation);
		T *RequireFromExistBlock(size_t size);
		T *RequireFromNewBlock(size_t size);
		void *RevertByLevel(void *pVBuf, int32_t level, size_t *size);
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
#if DBG_MEM_OVERWRITE_CHECK_ON
	mThreadParam.exit = 1;
	dbgThread.join();
	mThreadParam.pLock = NULL;
#endif

	for (int32_t level = 0; level < MEM_BLK_LEVEL_NUM; level++) {
		while(mUsageInfo[level].size()) {
			ReleaseBlock(level, 0);
		}
		mMemBlockCfg[level] = NULL;
	}
	PrintPool();
}

template <typename T>
int32_t MemoryPool<T>::AllocBlock(int32_t level)
{
	int32_t rt = ISP_SUCCESS;

	if (mUsageInfo[level].size() >= mMemBlockCfg[level]->max) {
		rt = ISP_INVALID_PARAM;
		ILOGE("Level:%d overflow", level);
		PrintPool();
		return rt;
	}

	MemBlock blk = { 0 };
	blk.blockBase = new uchar[mMemBlockCfg[level]->size];
	if (!blk.blockBase) {
		rt = ISP_MEMORY_ERROR;
		ILOGE("Level:%d alloc %u failed!", level, mMemBlockCfg[level]->size);
		return rt;
	}

	{
		unique_lock <mutex> lock(mUsageInfoLock);
		blk.blockSize = mMemBlockCfg[level]->size;
		memset(blk.blockBase, 0, blk.blockSize);
		blk.busySize = 0;
		MemSegment idleSeg = { 0 };
		idleSeg.pAddr = blk.blockBase;
		idleSeg.size = blk.blockSize;
		blk.idleList.push_back(idleSeg);
		mUsageInfo[level].push_back(blk);
		ILOGDM("L%d B%d: alloc %u", level, mUsageInfo[level].size() - 1, blk.blockSize);
	}

	return rt;
}

template <typename T>
int32_t MemoryPool<T>::ReleaseBlock(int32_t level, uint32_t index)
{
	int32_t rt = ISP_SUCCESS;

	if (index >= mUsageInfo[level].size()) {
		rt = ISP_INVALID_PARAM;
		ILOGE("Invalid block index:%d", index);
		return rt;
	}

	{
		unique_lock <mutex> lock(mUsageInfoLock);
		uchar* p = static_cast<uchar*>(mUsageInfo[level][index].blockBase);
		if (!p) {
			rt = ISP_FAILED;
			ILOGE("Fatal ERROR: cannot find block base");
			return rt;
		}

		while(!mUsageInfo[level][index].idleList.empty()) {
			mUsageInfo[level][index].idleList.pop_back();
		}

		while(!mUsageInfo[level][index].busyList.empty()) {
			auto seg = mUsageInfo[level][index].busyList.end();
			seg--;
			rt = MemoryReset(seg->pAddr, seg->size, MO_REV);
			ILOGW("Revert busy buf (addf:%p size:%u)", seg->pAddr, seg->size);
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
	}

	return rt;
}

template <typename T>
int32_t MemoryPool<T>::PrintPool()
{
	int32_t rt = ISP_SKIP;
	int32_t segCnt = 0;

	for (int32_t level = 0; level < MEM_BLK_LEVEL_NUM; level++) {
		if (!mUsageInfo[level].empty()) {
			rt = ISP_SUCCESS;
			break;
		}
	}

	if (rt != ISP_SUCCESS) {
		return rt;
	}

	ILOGI("<<<<<<<<<<<<<<<<<<<<<< MEM STATE >>>>>>>>>>>>>>>>>>>>>>>");
	for (int32_t level = 0; level < MEM_BLK_LEVEL_NUM; level++) {
		for (auto blk = mUsageInfo[level].begin(); blk != mUsageInfo[level].end(); blk++) {
			ILOGI("L%d B%d(%u) addr:%-8p size:%-8u used:%.2f%%",
					level,
					blk - mUsageInfo[level].begin(),
					mUsageInfo[level].size(),
					blk->blockBase,
					blk->blockSize,
					(float)blk->busySize * 100 / blk->blockSize);
			segCnt = 0;
			for (auto seg = blk->idleList.begin(); seg != blk->idleList.end(); seg++) {
				ILOGI("    Idle: S%d(%u) addr:%-8p size:%-8u",
						segCnt,
						blk->idleList.size(),
						seg->pAddr,
						seg->size);
				segCnt++;
			}
			segCnt = 0;
			for (auto seg = blk->busyList.begin(); seg != blk->busyList.end(); seg++) {
				ILOGI("    Busy: S%d(%u) addr:%-8p size:%-8u",
						segCnt,
						blk->busyList.size(),
						seg->pAddr,
						seg->size);
				segCnt++;
			}
		}
	}

	return rt;
}

template <typename T>
T* MemoryPool<T>::RequireBuffer(size_t TSize)
{
	T* pBuffer = NULL;
	size_t size = TSize * sizeof(T);
#if DBG_MEM_OVERWRITE_CHECK_ON
	size += OVERWRITE_CHECK_SIZE;
#endif

	if (!size || size > mMemBlockCfg[MEM_BLK_LEVEL_NUM - 1]->size) {
		ILOGE("Require invalid size:C%u (N%u x T%u) ", size, sizeof(T), TSize);
		return NULL;
	}

	pBuffer = RequireFromExistBlock(size);
	if (!pBuffer) {
		pBuffer = RequireFromNewBlock(size);
	}

	if (!pBuffer) {
		ILOGE("Faild to require buffer C%u (N%u x T%u)", size, sizeof(T), TSize);
		PrintPool();
	} else {
		ILOGDM("C%u (N%u x T%u) is required", size, sizeof(T), TSize);
	}

	return pBuffer;
}

template <typename T>
T* MemoryPool<T>::RevertBuffer(T* pBuffer)
{
	size_t size = 0;
	void *pVBuf = (void*)pBuffer;

	if (!pBuffer) {
		ILOGE("Invalid buffer:%p", pBuffer);
		return pBuffer;
	}

	for (int32_t level = 0; level < MEM_BLK_LEVEL_NUM; level++) {
		pVBuf = RevertByLevel(pVBuf, level, &size);
		if (!pVBuf) {
			ILOGDM("C%u (N%u x T%u) is reverted", size, sizeof(T), size / sizeof(T));
			break;
		} else if (level == MEM_BLK_LEVEL_NUM - 1) {
			ILOGE("Invalid buffer:%p not in pool", pBuffer);
			PrintPool();
		}
	}

	return pBuffer;
}

template <typename T>
T* MemoryPool<T>::RequireFromExistBlock(size_t size)
{
	T* pBuffer = NULL;

	{
		unique_lock <mutex> lock(mUsageInfoLock);
		for (int32_t level = 0; level < MEM_BLK_LEVEL_NUM; level++) {
			if (size > mMemBlockCfg[level]->size) {
				continue;
			}
			for (auto blk = mUsageInfo[level].begin(); blk != mUsageInfo[level].end(); blk++) {
				if (size > blk->blockSize - blk->busySize) {
					continue;
				}
				for (auto seg = blk->idleList.begin(); seg != blk->idleList.end(); seg++) {
					if (size <= seg->size) {
						MemSegment idleSeg = { 0 };
						MemSegment busySeg = { 0 };
						busySeg.pAddr = seg->pAddr;
						busySeg.size = size;
						MemoryReset(busySeg.pAddr, busySeg.size, MO_REQ);
						blk->busyList.push_front(busySeg);

						pBuffer = static_cast<T*>(busySeg.pAddr);

						idleSeg.pAddr = (void*)(static_cast<T*>(seg->pAddr) + size);
						idleSeg.size = seg->size - size;
						blk->idleList.erase(seg);
						blk->idleList.push_front(idleSeg);

						blk->busySize += size;
						return pBuffer;
					}
				}
			}
		}
	}

	return pBuffer;
}

template <typename T>
T* MemoryPool<T>::RequireFromNewBlock(size_t size)
{
	T* pBuffer = NULL;

	for (int32_t level = 0; level < MEM_BLK_LEVEL_NUM; level++) {
		if (size > mMemBlockCfg[level]->size) {
			if (level == MEM_BLK_LEVEL_NUM - 1) {
				ILOGE("Reguire buffer(%u) failed!", size);
				return NULL;
			}
		} else {
			if (SUCCESS(AllocBlock(level))) {
				unique_lock <mutex> lock(mUsageInfoLock);
				MemBlock* blk = &mUsageInfo[level].back();
				list<MemSegment>::iterator seg = blk->idleList.end();
				seg--;

				MemSegment idleSeg = { 0 };
				MemSegment busySeg = { 0 };
				busySeg.pAddr = seg->pAddr;
				busySeg.size = size;
				MemoryReset(busySeg.pAddr, busySeg.size, MO_REQ);
				blk->busyList.push_front(busySeg);

				pBuffer = static_cast<T*>(busySeg.pAddr);

				idleSeg.pAddr = (void*)(static_cast<T*>(seg->pAddr) + size);
				idleSeg.size = seg->size - size;
				blk->idleList.erase(seg);
				blk->idleList.push_front(idleSeg);

				blk->busySize += size;
				return pBuffer;
			}
		}
	}

	return pBuffer;
}

template <typename T>
void *MemoryPool<T>::RevertByLevel(void *pVBuf, int32_t level, size_t *size)
{
	if (level >= MEM_BLK_LEVEL_NUM) {
		ILOGE("Invalid level:%d", level);
		return pVBuf;
	}

	if (!size) {
		ILOGE("size is null");
		return pVBuf;
	}

	{
		unique_lock <mutex> lock(mUsageInfoLock);
		for (auto blk = mUsageInfo[level].begin(); blk != mUsageInfo[level].end(); blk++) {
			if (static_cast<uchar*>(pVBuf) < blk->blockBase ||
					static_cast<uchar*>(pVBuf) >= static_cast<uchar*>(blk->blockBase) + blk->blockSize) {
				continue;
			}
			size_t cnt = 1;
			for (auto seg = blk->busyList.begin(); seg != blk->busyList.end(); seg++) {
				if (static_cast<uchar*>(pVBuf) == seg->pAddr) {
					bool isNewSeg = true;
					*size = seg->size;
					MemoryReset(seg->pAddr, seg->size, MO_REV);

					MemSegment tmpSeg = { 0 };
					tmpSeg.pAddr = (void*)seg->pAddr;
					tmpSeg.size = seg->size;

					/* 1. Forward merge */
					uchar* nextAddr = static_cast<uchar*>(tmpSeg.pAddr) + tmpSeg.size;
					for (auto idleSeg = blk->idleList.begin(); idleSeg != blk->idleList.end(); idleSeg++) {
						if (nextAddr == idleSeg->pAddr) {
							tmpSeg.size += idleSeg->size;
							blk->idleList.erase(idleSeg);
							break;
						}
					}

					/* 2. Backward merge */
					for (auto idleSeg = blk->idleList.begin(); idleSeg != blk->idleList.end(); idleSeg++) {
						uchar* lastAddr = static_cast<uchar*>(idleSeg->pAddr) + idleSeg->size;
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
					blk->busySize -= *size;
					pVBuf = NULL;
					return pVBuf;
				} else if (cnt == blk->busyList.size()) {
					ILOGE("Fatal Error: cannot match buffer:%p in segment", pVBuf);
					PrintPool();
					return pVBuf;
				}
				cnt++;
			}
		}
	}

	return pVBuf;
}

template <typename T>
int32_t MemoryPool<T>::MemoryReset(void* pAddr, size_t size, int32_t operation)
{
	int32_t rt = ISP_SUCCESS;

	if (operation == MO_REQ) {
#if DBG_MEM_OVERWRITE_CHECK_ON
		memcpy(static_cast<uchar*>(pAddr) + size - OVERWRITE_CHECK_SIZE, gOverwiteSymbol, OVERWRITE_CHECK_SIZE);
		mSymbolMap.insert(pair<void*, void*>(pAddr, static_cast<uchar*>(pAddr) + size - OVERWRITE_CHECK_SIZE));
#endif
		/* No need to reset mem for require buffer */
	} else if (operation == MO_REV) {
#if DBG_MEM_OVERWRITE_CHECK_ON
		mSymbolMap.erase(pAddr);
#endif
		memset(pAddr, 0, size);
	} else {
		rt = ISP_FAILED;
		ILOGE("Invalid operation:%d", operation);
	}

	return rt;
}
