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

ISPResult ISPNode::Init(PROCESS_TYPE type){//, Args&&... args) {

	ISPResult result = ISPSuccess;

	mName = PROCESSNAME[type];
	mType = type;
	mEnable = true;
	pProcess = nullptr;
	//isNecessary = true;  //Wait to do. Distinwish necessary process.	
	next = nullptr;
	mInited = true;
	return result;
}

ISPResult ISPNode::Process(void* data, uint32_t argNum, ...) {
	ISPResult rt = ISPSuccess;
	if (!mInited) {
		return ISPNotInited;
	}
	va_list va;
	int Imgsizey, Imgsizex, strength1, strength2, strength3;
	double GCCweight, bgain, ggain, rgain;
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
		__crt_va_start(va, argNum);
		Imgsizey = static_cast<int>(__crt_va_arg(va, int));
		Imgsizex = static_cast<int>(__crt_va_arg(va, int));
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
		rt = ISPInvalied;
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
