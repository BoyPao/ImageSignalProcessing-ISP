// License: GPL-3.0-or-later
/*
 * Processor source file.
 *
 * Copyright (c) 2023 Penint32_tHao <635945005@qq.com>
 */

#include "Processor.h"
#include "BZLog.h"

#define WORKER_WAIT_US 100

void* WorkerFunc(void *pParam)
{
	Processor *pProc = static_cast<Processor*>(pParam);

	if (!pProc) {
		BLOGE("Worker thread param is null");
		return 0;
	}

	BLOGDP("Work S");
	while (1) {
		if (!pProc->IsWorkOn()) {
			BLOGDP("Work E");
			return NULL;
		}

		int32_t rt = BZ_SUCCESS;
		Job job = { 0 };
		rt = pProc->GetJob(&job);
		if (rt == BZ_SUCCESS) {
			rt = pProc->HandleJob(&job);
			if (!SUCCESS(rt)) {
				BLOGE("Faild to handle job, %d", rt);
			}
		} else {
			usleep(WORKER_WAIT_US);
		}
	}
}

Processor::Processor(int32_t id)
{
	mId = id;
	workerStatus = true;
	workerThread = thread(WorkerFunc, (void*)this);
}

Processor::~Processor()
{
	OffWork();
	workerThread.join();
}

bool Processor::IsWorkOn()
{
	return workerStatus;
}

int32_t Processor::OffWork()
{
	int32_t rt = BZ_SUCCESS;

	rt = CancelJobs();
	workerStatus = false;

	return rt;
}

int32_t Processor::CancelJobs()
{
	int32_t rt = BZ_SUCCESS;

	{
		unique_lock <mutex> lock(queueLock);

		int32_t cnt = 1;
		while (!workQueue.empty()) {
			BLOGDP("Job drop cnt:%d", cnt);
			workQueue.pop();
			cnt++;
		}
	}

	return rt;
}

int32_t Processor::GetJob(Job *pJob)
{
	int32_t rt = BZ_SUCCESS;

	if (!pJob) {
		rt = BZ_INVALID_PARAM;
		BLOGE("Input is null");
		return rt;
	}

	{
		unique_lock <mutex> lock(queueLock);

		if (!workQueue.empty()) {
			Job job = workQueue.front();
			memcpy(pJob, &job, sizeof(Job));
			workQueue.pop();
		} else {
			rt = BZ_SKIP;
		}
	}

	return rt;
}

int32_t Processor::ScheduleJob(Job *pJob)
{
	int32_t rt = BZ_SUCCESS;

	if (!pJob) {
		rt = BZ_INVALID_PARAM;
		BLOGE("Input is null");
		return rt;
	}

	{
		unique_lock <mutex> lock(queueLock);
		workQueue.push(*pJob);
		BLOGDP("job is added to queue%d(%d)", mId, workQueue.size());
	}

	return rt;
}

int32_t Processor::HandleJob(Job *pJob)
{
	int32_t rt = BZ_SUCCESS;

	if (!pJob) {
		rt = BZ_INVALID_PARAM;
		BLOGE("Input is null");
		return rt;
	}

	rt = SetCtrl(pJob);
	if (!SUCCESS(rt)) {
		return rt;
	}

	rt = Process(pJob);

	return rt;
}

int32_t Processor::SetCtrl(Job *pJob)
{
	int32_t rt = BZ_SUCCESS;

	if (!pJob) {
		rt = BZ_INVALID_PARAM;
		BLOGE("Input is null");
		return rt;
	}

	return rt;
}

int32_t Processor::Process(Job *pJob)
{
	int32_t rt = BZ_SUCCESS;

	if (!pJob) {
		rt = BZ_INVALID_PARAM;
		BLOGE("Job is null");
		return rt;
	}

	switch(pJob->info.type) {
		case BZ_PARAM_TYPE_BLC:
			rt = WrapBlackLevelCorrection(&pJob->info);
			break;
		case BZ_PARAM_TYPE_LSC:
			rt = WrapLensShadingCorrection(&pJob->info);
			break;
		case BZ_PARAM_TYPE_DMC:
			rt = WrapDemosaic(&pJob->info);
			break;
		case BZ_PARAM_TYPE_WB:
			rt = WrapWhiteBalance(&pJob->info);
			break;
		case BZ_PARAM_TYPE_CC:
			rt = WrapColorCorrection(&pJob->info);
			break;
		case BZ_PARAM_TYPE_Gamma:
			rt = WrapGammaCorrection(&pJob->info);
			break;
		case BZ_PARAM_TYPE_WNR:
			rt = WrapWaveletNR(&pJob->info);
			break;
		case BZ_PARAM_TYPE_EE:
			rt = WrapEdgeEnhancement(&pJob->info);
			break;
		case BZ_PARAM_TYPE_RAW2RGB:
			rt = WrapCST_RAW2RGB(&pJob->info);
			break;
		case BZ_PARAM_TYPE_RGB2YUV:
			rt = WrapCST_RGB2YUV(&pJob->info);
			break;
		case BZ_PARAM_TYPE_YUV2RGB:
			rt = WrapCST_YUV2RGB(&pJob->info);
			break;
		case BZ_PARAM_TYPE_NUM:
		default:
			BLOGW("Nothing todo for job type:%d", pJob->info.type);
			break;
	}

	int32_t argNum = 3;
	BoZhi::GetInstance()->GetCallbacks()->ISPNotify(argNum, mId, pJob->info.type, rt);

	return rt;
}
