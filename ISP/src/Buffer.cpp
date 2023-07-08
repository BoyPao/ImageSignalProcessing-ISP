// License: GPL-3.0-or-later
/*
 * ISP buffer implementation.
 *
 * Copyright (c) 2023 Peng Hao <635945005@qq.com>
 */

#include "Utils.h"
#include "Buffer.h"
#include "MemPool.h"

namespace asteroidaxis::isp::resource {

Buffer::Buffer(void* addr, size_t size)
{
	mAddr = addr;
	mSize = size;
}

Buffer::~Buffer()
{
	if (mAddr) {
		mAddr = MemoryPool<BUFFER_USE_MEM_T>::GetInstance()->RevertBuffer(
				static_cast<BUFFER_USE_MEM_T *>(mAddr));
	}
	mSize = 0;
}

Buffer *Buffer::Alloc(size_t size)
{
	Buffer *pBuf =  new Buffer(
			MemoryPool<BUFFER_USE_MEM_T>::GetInstance()->RequireBuffer(size),
			size);
	BufferMgr::GetInstance()->PushBufferToList(pBuf);
	return pBuf;
}

void Buffer::Free(Buffer **ppBuf)
{
	if (!ppBuf || !*ppBuf) {
		return;
	}
	BufferMgr::GetInstance()->PopBufferFromList(*ppBuf);
	(*ppBuf)->CheckFreeDone(MemoryPool<BUFFER_USE_MEM_T>::GetInstance()->RevertBuffer(
			static_cast<BUFFER_USE_MEM_T *>((*ppBuf)->Addr())));
	delete *ppBuf;
	*ppBuf = NULL;
}

void *Buffer::Addr()
{
	return mAddr;
}

size_t Buffer::Size()
{
	return mSize;
}

void Buffer::Reset()
{
	if (mAddr) {
		memset(mAddr, 0, mSize);
	}
}

void Buffer::CheckFreeDone(void *pAddr)
{
	if (!pAddr) {
		mAddr = NULL;
		mSize = 0;
	}
}

BufferMgr *BufferMgr::GetInstance()
{
	static BufferMgr gInstance;
	return &gInstance;
}

BufferMgr::BufferMgr()
{
}

BufferMgr::~BufferMgr()
{
	ReleaseAllBuffer();
}

int32_t BufferMgr::PushBufferToList(Buffer *pBuf)
{
	int32_t rt = ISP_SUCCESS;
	if (!pBuf) {
		rt = ISP_INVALID_PARAM;
		ILOGE("Buffer is null");
		return rt;
	}

	if (!pBuf->Addr()) {
		rt = ISP_INVALID_PARAM;
		ILOGE("Buffer is invalid");
		return rt;
	}

	{
		unique_lock <mutex> lock(bufListLock);
		bufList.push_back(pBuf);
	}
	return rt;
}

int32_t BufferMgr::PopBufferFromList(Buffer *pBuf)
{
	int32_t rt = ISP_SUCCESS;
	if (!pBuf) {
		rt = ISP_INVALID_PARAM;
		ILOGE("Buffer is null");
		return rt;
	}

	{
		unique_lock <mutex> lock(bufListLock);
		for (auto iter = bufList.begin(); iter != bufList.end(); iter++) {
			if (!*iter) {
				rt = ISP_FAILED;
				ILOGE("Fatal: list node is null");
				return rt;
			}
			if ((*iter)->Addr() == pBuf->Addr()) {
				bufList.erase(iter);
				break;
			}
		}
	}
	return rt;
}

int32_t BufferMgr::ReleaseAllBuffer()
{
	int32_t rt = ISP_SUCCESS;
	list<Buffer *>::iterator iter;

	while(!bufList.empty()) {
		{
			unique_lock <mutex> lock(bufListLock);
			iter = bufList.begin();
			if (iter == bufList.end()) {
				ILOGW("Buffer list is empty");
				return rt;
			}
			if (!*iter) {
				rt = ISP_FAILED;
				ILOGE("Fatal: list node is null");
				return rt;
			}
		}
		Buffer::Free(&*iter);
	}

	return rt;
}

}
