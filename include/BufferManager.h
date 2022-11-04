// License: GPL-3.0-or-later
/*
 * Define memory pool.
 *
 * Copyright (c) 2019 Peng Hao <635945005@qq.com>
 */
#include <vector>
#include <list>
#include "Utils.h"
#define MEM_BLK_L0_MAX_NUM 16
#define MEM_BLK_L1_MAX_NUM 8
#define MEM_BLK_L2_MAX_NUM 2

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

template <typename T>
class MemoryPool
{
	public:
		MemoryPool();
		~MemoryPool();

		T* RequireBuffer(size_t size);
		T* RevertBuffer(T* pBuffer);

	private:
		ISPResult AllocBlock(int32_t level);
		ISPResult ReleaseBlock(int32_t level, u_int32_t index);
		ISPResult PrintPool();
		MemBlockInfo const* mMemBlockCfg[MEM_BLK_LEVEL_NUM];
		vector<MemBlock> mUsageInfo[MEM_BLK_LEVEL_NUM];
};

template <typename T>
MemoryPool<T>::MemoryPool()
{
	for (int32_t level = 0; level < MEM_BLK_LEVEL_NUM; level++) {
		mMemBlockCfg[level] = &gMemBlockCfg[level];
	}
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
}

template <typename T>
ISPResult MemoryPool<T>::AllocBlock(int32_t level)
{
	ISPResult rt = ISP_SUCCESS;

	if (mUsageInfo[level].size() >= mMemBlockCfg[level]->max) {
		rt = ISP_INVALID_PARAM;
		ILOGE("Level:%d overflow", level);
	}

	if (SUCCESS(rt)) {
		MemBlock blk = {0};
		blk.blockBase = new T[mMemBlockCfg[level]->size];
		if (!blk.blockBase) {
			rt = ISP_MEMORY_ERROR;
			ILOGE("Level:%d alloc %u failed!", level, mMemBlockCfg[level]->size);
		}

		if (SUCCESS(rt)) {
			blk.blockSize = mMemBlockCfg[level]->size;
			blk.busySize = 0;
			MemSegment idleSeg = {0};
			idleSeg.pAddr = blk.blockBase;
			idleSeg.size = blk.blockSize;
			blk.idleList.push_back(idleSeg);
			mUsageInfo[level].push_back(blk);
			memset(blk.blockBase, 0, sizeof(T) * blk.blockSize);
			ILOGDM("L%d B%d: alloc %u", level, mUsageInfo[level].size() - 1, mMemBlockCfg[level]->size);
		}
	}

	return rt;
}

template <typename T>
ISPResult MemoryPool<T>::ReleaseBlock(int32_t level, u_int32_t index)
{
	ISPResult rt = ISP_SUCCESS;

	if (index >= mUsageInfo[level].size()) {
		rt = ISP_INVALID_PARAM;
		ILOGE("Invalid block index:%d", index);
	}

	if (SUCCESS(rt)) {
		T* p = static_cast<T*>(mUsageInfo[level][index].blockBase);
		if (p) {
			while(mUsageInfo[level][index].idleList.begin() !=
					mUsageInfo[level][index].idleList.end()) {
				mUsageInfo[level][index].idleList.pop_back();
			}

			while(mUsageInfo[level][index].busyList.begin() !=
					mUsageInfo[level][index].busyList.end()) {
				mUsageInfo[level][index].busyList.pop_back();
			}

			delete[] p;
			mUsageInfo[level][index].blockSize = 0;
			mUsageInfo[level][index].busySize = 0;
			mUsageInfo[level].erase(mUsageInfo[level].begin() + index);
			ILOGDM("Level:%d index:%d released. Rest:%u", level, index, mUsageInfo[level].size());
		} else {
			rt = ISP_FAILED;
			ILOGE("Fatal ERROR!");
		}
	}

	return rt;
}

template <typename T>
ISPResult MemoryPool<T>::PrintPool()
{
	ISPResult rt = ISP_SUCCESS;
	int32_t segCnt = 0;

	for (int32_t level = 0; level < MEM_BLK_LEVEL_NUM; level++) {
		for (auto blk = mUsageInfo[level].begin(); blk != mUsageInfo[level].end(); blk++) {
			ILOGDM("L%d B%d(%u) addr:%-8p size:%-8u used:%.2f%%",
					level,
					blk - mUsageInfo[level].begin(),
					mUsageInfo[level].size() - 1,
					blk->blockBase,
					blk->blockSize,
					(float)blk->busySize * 100 / blk->blockSize);
			segCnt = 0;
			for (auto seg = blk->idleList.begin(); seg != blk->idleList.end(); seg++) {
				ILOGDM("    Idle: S%d(%u) addr:%-8p size:%-8u",
						segCnt,
						blk->idleList.size() - 1,
						seg->pAddr,
						seg->size);
				segCnt++;
			}
			segCnt = 0;
			for (auto seg = blk->busyList.begin(); seg != blk->busyList.end(); seg++) {
				ILOGDM("    Busy: S%d(%u) addr:%-8p size:%-8u",
						segCnt,
						blk->busyList.size() - 1,
						seg->pAddr,
						seg->size);
				segCnt++;
			}
		}
	}

	return rt;
}

template <typename T>
T* MemoryPool<T>::RequireBuffer(size_t size)
{
	ISPResult rt = ISP_SUCCESS;
	T* pBuffer = NULL;

	if (!size || size > mMemBlockCfg[MEM_BLK_LEVEL_NUM - 1]->size) {
		rt = ISP_INVALID_PARAM;
		ILOGE("Require invalid size:%u", size);
	}

	/* 1. Search valid segment */
	if (SUCCESS(rt)) {
		for (int32_t level = 0; level < MEM_BLK_LEVEL_NUM; level++) {
			if (size < mMemBlockCfg[level]->size) {
				for (auto blk = mUsageInfo[level].begin(); blk != mUsageInfo[level].end(); blk++) {
					if (size < blk->blockSize - blk->busySize) {
						for (auto seg = blk->idleList.begin(); seg != blk->idleList.end(); seg++) {
							if (size < seg->size) {
								MemSegment idleSeg = {0};
								MemSegment busySeg = {0};
								busySeg.pAddr = seg->pAddr;
								busySeg.size = size;
								blk->busyList.push_front(busySeg);

								pBuffer = static_cast<T*>(busySeg.pAddr);

								idleSeg.pAddr = (void*)(static_cast<T*>(seg->pAddr) + size * sizeof(T));
								idleSeg.size = seg->size - size;
								blk->idleList.erase(seg);
								blk->idleList.push_front(idleSeg);

								blk->busySize += size;
								rt = ISP_SKIP;
								break;
							}
						}
					}
					if (rt == ISP_SKIP) {
						break;
					}
				}
			}
			if (rt == ISP_SKIP) {
				break;
			}
		}
	}

	/* 2. Allock new block */
	if (rt == ISP_SUCCESS) {
		for (int32_t level = 0; level < MEM_BLK_LEVEL_NUM; level++) {
			if (size < mMemBlockCfg[level]->size) {
				rt = AllocBlock(level);
				if (SUCCESS(rt)) {
					MemBlock* blk = &mUsageInfo[level].back();
					list<MemSegment>::iterator seg = blk->idleList.end();
					seg--;

					MemSegment idleSeg = {0};
					MemSegment busySeg = {0};
					busySeg.pAddr = seg->pAddr;
					busySeg.size = size;
					blk->busyList.push_front(busySeg);

					pBuffer = static_cast<T*>(busySeg.pAddr);

					idleSeg.pAddr = (void*)(static_cast<T*>(seg->pAddr) + size * sizeof(T));
					idleSeg.size = seg->size - size;
					blk->idleList.erase(seg);
					blk->idleList.push_front(idleSeg);

					blk->busySize += size;
					rt = ISP_SKIP;
					break;
				}
			}

			if (level == MEM_BLK_LEVEL_NUM - 1) {
				rt = ISP_MEMORY_ERROR;
				ILOGE("Reguire buffer(%u) failed!", size);
			}
		}
	}

	if (SUCCESS(rt)) {
		ILOGDM("%u is required", size);
		rt = PrintPool();
	}

	return pBuffer;
}

template <typename T>
T* MemoryPool<T>::RevertBuffer(T* pBuffer)
{
	ISPResult rt = ISP_SUCCESS;
	size_t size = 0;

	if (!pBuffer) {
		rt = ISP_INVALID_PARAM;
		ILOGE("Invalid buffer:%p", pBuffer);
	}

	for (int32_t level = 0; level < MEM_BLK_LEVEL_NUM; level++) {
		for (auto blk = mUsageInfo[level].begin(); blk != mUsageInfo[level].end(); blk++) {
			if (pBuffer >= blk->blockBase && pBuffer < static_cast<T*>(blk->blockBase) + blk->blockSize * sizeof(T)) {
				for (auto seg = blk->busyList.begin(); seg != blk->busyList.end(); seg++) {
					if (pBuffer == seg->pAddr) {
						bool isNewSeg = true;
						size = seg->size;
						memset(seg->pAddr, 0, sizeof(T) * seg->size);

						MemSegment tmpSeg = {0};
						tmpSeg.pAddr = (void*)seg->pAddr;
						tmpSeg.size = seg->size;

						/* 1. Forward merge */
						T* nextAddr = static_cast<T*>(tmpSeg.pAddr) + tmpSeg.size * sizeof(T);
						for (auto idleSeg = blk->idleList.begin(); idleSeg != blk->idleList.end(); idleSeg++) {
							if (nextAddr == idleSeg->pAddr) {
								tmpSeg.size += idleSeg->size;
								blk->idleList.erase(idleSeg);
								break;
							}
						}

						/* 2. Backward merge */
						for (auto idleSeg = blk->idleList.begin(); idleSeg != blk->idleList.end(); idleSeg++) {
							T* lastAddr = static_cast<T*>(idleSeg->pAddr) + idleSeg->size * sizeof(T);
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
				if (rt == ISP_SKIP) {
					break;
				} else {
					rt = ISP_INVALID_PARAM;
					ILOGE("Invalid buffer:%p not in segment", pBuffer);
				}
			}
		}
		if (rt != ISP_SUCCESS) {
			break;
		}
		if (level == MEM_BLK_LEVEL_NUM - 1) {
			rt = ISP_INVALID_PARAM;
			ILOGE("Invalid buffer:%p not in pool", pBuffer);
		}
	}

	if (SUCCESS(rt)) {
		ILOGDM("%u is reverted", size);
		rt = PrintPool();
	}

	return pBuffer;
}

