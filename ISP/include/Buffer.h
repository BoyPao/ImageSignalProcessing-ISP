// License: GPL-3.0-or-later
/*
 * Define ISP buffer.
 *
 * Copyright (c) 2023 Peng Hao <635945005@qq.com>
 */
#include "Utils.h"
#include <list>
#include <mutex>

#ifndef __ISP_BUFFER_H__
#define __ISP_BUFFER_H__

using namespace std;
namespace asteroidaxis::isp::resource {

#define BUFFER_USE_MEM_T uchar

#define SIZEOF_T(s) (size_t)((sizeof(s) % sizeof(BUFFER_USE_MEM_T)) ? \
	(sizeof(s) / sizeof(BUFFER_USE_MEM_T) + 1) : \
	(sizeof(s) / sizeof(BUFFER_USE_MEM_T)))

class Buffer {
	public:
		static Buffer *Alloc(size_t size);
		static void Free(Buffer **ppBuf);
		void *Addr();
		size_t Size();
		void Reset();

	private:
		Buffer() = delete;
		Buffer(const Buffer&) = delete;
		Buffer& operator = (const Buffer&) = delete;

	private:
		Buffer(void* addr, size_t size);
		~Buffer();
		void CheckFreeDone(void *pAddr);

		void *mAddr = NULL;
		size_t mSize = 0;
};

class BufferMgr {
	public:
		static BufferMgr *GetInstance();
		int32_t PushBufferToList(Buffer *pBuf);
		int32_t PopBufferFromList(Buffer *pBuf);
		int32_t ReleaseAllBuffer();

	private:
		BufferMgr();
		~BufferMgr();
		std::list<Buffer *> bufList;
		mutex bufListLock;
};

}

#endif
