// License: GPL-3.0-or-later
/*
 * Define a buffer queue.
 *
 * Copyright (c) 2019 Peng Hao <635945005@qq.com>
 */

#pragma once

#include "Utils.h"
#include <atomic>
#include <mutex>
#include <memory> /* For std::shared_ptr under c++14 standard*/

#define BUF_QUEUE_MAX_SIZE 10

#define MEM_MAX_BLOCK_NUM		8
#define MEM_L0_BLOCK_BYTE_SIZE	(1024 * 256)
#define MEM_L1_BLOCK_BYTE_SIZE	(1024 * 1024 * 2)
#define MEM_L2_BLOCK_BYTE_SIZE	(1024 * 1024 * 16)
#define MEM_L3_BLOCK_BYTE_SIZE	(1024 * 1024 * 128)

enum MEM_BLOCK_LEVEL {
	MEM_BLOCK_L0 = 0,
	MEM_BLOCK_L1,
	MEM_BLOCK_L2,
	MEM_BLOCK_L3,
	MEM_BLOCK_NUM
};

struct Buffer {
	void* pAddr;
	int32_t size;
	mutex lock;
};

class MemoryPool {
	public:


	private:
};

template <typename T>
class Queue {
	public:
		Queue() { mSize = 0; };
		Queue(int32_t size);
		virtual ~Queue();

		virtual bool IsEmpty();
		virtual bool IsFull();

		virtual void PushBack(T* element);
		virtual void PopFront(T* elemrnt);

	protected:
		std::shared_ptr<T> mBuffer;
		int32_t mSize;
		atomic<int32_t> mFIndex;
		atomic<int32_t> mBIndex;

	private:

};

template <typename T>
Queue<T>::Queue(int32_t size)
{
	mSize = size;
	mBuffer = std::make_shared<T>(new T[mSize], [] (T *p) { delete[] p; });
	memset((void*)mBuffer.get(), 0, sizeof(T) * mSize);
	mFIndex = 0;
	mBIndex = 0;
	ILOGD("Queue size:%d index(%d,%d)", mSize, mFIndex.load(), mBIndex.load());
}

template <typename T>
bool Queue<T>::IsEmpty()
{
	return mFIndex == mBIndex;
}

template <typename T>
bool Queue<T>::IsFull()
{
	return (mBIndex.load() + 1) % mSize == mFIndex.load();
}

template <typename T>
void Queue<T>::PushBack(T* element)
{
	if (!IsFull) {
		memcpy((void*)mBuffer.get(), (void*)element, sizeof(T));
		mBIndex = (mBIndex.load() + 1) % mSize;
	} else {
		ILOGW("Queue(%d: %d, %d) is full", mSize, mFIndex.load(), mBIndex.load());
	}
}

template <typename T>
void Queue<T>::PopFront(T* element)
{
	if (element) {
		if (!IsEmpty()) {
			memcpy((void*)element, (void*)mBuffer.get(), sizeof(T));
			memset((void*)(mBuffer.get() + mFIndex.load()), 0, sizeof(T));
			mFIndex = (mFIndex.load() + 1) % mSize;
		} else {
			ILOGW("Queue(%d: %d, %d) is empty", mSize, mFIndex.load(), mBIndex.load());
		}
	} else {
		ILOGE("element is nullptr!");
	}
}
