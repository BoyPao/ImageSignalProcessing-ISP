// License: GPL-3.0-or-later
/*
 * Processor head file.
 *
 * Copyright (c) 2023 Peng Hao <635945005@qq.com>
 */

#pragma once
#include "BZUtils.h"
#include "Algorithm.h"
#include <thread>
#include <mutex>
#include <queue>

struct Job {
	AlgInfo info;
};

class Processor {
	public:
		Processor(int32_t);
		~Processor();

		bool IsWorkOn();
		int32_t OffWork();
		int32_t GetJob(Job *pJob);
		int32_t CancelJobs();
		int32_t ScheduleJob(Job *pJob);
		int32_t HandleJob(Job *pJob);

	private:
		int32_t SetCtrl(Job* pJob);
		int32_t Process(Job* pJob);

		int32_t mId;
		bool workerStatus;
		thread workerThread;
		queue<Job> workQueue;
		mutex queueLock;
};
