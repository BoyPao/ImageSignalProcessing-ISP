//////////////////////////////////////////////////////////////////////////////////////
// Author: Peng Hao <635945005@qq.com>
// License: GPL
//////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////
// @file: Pipeline.cpp
// @brief: Pipeline is a structure of process steps. Aims to let process construction
//         more flexable.
//////////////////////////////////////////////////////////////////////////////////////

#include "Pipeline.h"
#include "Algorithm.h"

using namespace std;

ISPResult ISPNode::Init(PROCESS_TYPE type)//, Args&&... args) {
{
	ISPResult result = ISPSuccess;

	mName = PROCESSNAME[type];
	mType = type;
	//mEnable = true;
	pProcess = nullptr;
	//isNecessary = true;  //Wait to do. Distinwish necessary process.	
	next = nullptr;
	mInited = true;
	return result;
}

ISPResult ISPNode::Process(void* data, int32_t argNum, ...)
{
	ISPResult rt = ISPSuccess;
	if (!mInited) {
		return ISPNotInited;
	}
	va_list va;
	bool enable;
	switch (mType) {
	case BLC:
		pProcess = &BlackLevelCorrection;
		rt = pProcess(data, argNum);
		break;
	case LSC:
		pProcess = &LensShadingCorrection;
		rt = pProcess(data, argNum);
		break;
	case GCC:
		pProcess = &GreenChannelsCorrection;
		rt = pProcess(data, argNum);
		break;
	case WB:
		pProcess = &WhiteBalance;
		rt = pProcess(data, argNum);
		break;
	case CC:
		pProcess = &ColorCorrection;
		rt = pProcess(data, argNum);
		break;
	case GAMMA:
		pProcess = &GammaCorrection;
		rt = pProcess(data, argNum);
		break;
	case SWNR:
		pProcess = &SmallWaveNR;
		int32_t Imgsizey, Imgsizex;
		__crt_va_start(va, argNum);
		Imgsizey = static_cast<int32_t>(__crt_va_arg(va, int32_t));
		Imgsizex = static_cast<int32_t>(__crt_va_arg(va, int32_t));
		__crt_va_end(va);
		rt = pProcess(data, argNum, Imgsizey, Imgsizex);
		break;
	case SHARPNESS:
		pProcess = &Sharpness;
		rt = pProcess(data, argNum);
		break;
	case NONE:
	case PROCESS_TYPE_MAX:
	default:
		pProcess = nullptr;
		rt = ISPInvalid;
	}

	return rt;
}

ISPResult ISPNode::getNodeName(string* name)
{ 
	ISPResult rt = ISPSuccess;
	if (mInited) {
		name = &mName;
	} else {
		rt = ISPNotInited;
	}
	return rt;
}

/*Pipeline* CreateDefaultPipeline()
{
	Pipeline* pPipeline = new Pipeline;
	pPipeline->AddNode(CST);
}*/