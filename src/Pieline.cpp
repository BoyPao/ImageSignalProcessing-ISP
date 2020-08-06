//////////////////////////////////////////////////////////////////////////////////////
// Author: Peng Hao
// License: GPL
//////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////
// @file: Pipeline.cpp
// @brief: Pipeline is a structure of process steps. Aims to let process construction
//         more flexable.
//////////////////////////////////////////////////////////////////////////////////////
#include "Pipeline.h"

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

ISPResult ISPNode::Process(void* data,uint32_t argNum, ...) {
	ISPResult rt = ISPSuccess;
	if (!mInited) {
		return ISPNotInited;
	}
	va_list va;
	int BLValue, Imgsizey, Imgsizex, strength1, strength2, strength3;
	double GCCweight, bgain, ggain, rgain;
	bool enable;
	switch (mType) {
	case BLC:
		pProcess = &BlackLevelCorrection;
		__crt_va_start(va, argNum);
		BLValue = static_cast<int>(__crt_va_arg(va, int));
		enable = static_cast<bool>(__crt_va_arg(va, bool));
		__crt_va_end(va);
		rt = pProcess(data, argNum, BLValue, enable);
		break;
	case LSC:
		pProcess = &LensShadingCorrection;
		__crt_va_start(va, argNum);
		enable = static_cast<bool>(__crt_va_arg(va, bool));
		__crt_va_end(va);
		rt = pProcess(data, argNum, enable);
		break;
	case GCC:
		pProcess = &GreenImbalanceCorrection;
		__crt_va_start(va, argNum);
		GCCweight = static_cast<double>(__crt_va_arg(va, double));
		enable = static_cast<bool>(__crt_va_arg(va, bool));
		__crt_va_end(va);
		rt = pProcess(data, argNum, GCCweight, enable);
		break;
	case WB:
		pProcess = &WhiteBalance;
		__crt_va_start(va, argNum);
		bgain = static_cast<double>(__crt_va_arg(va, double));
		ggain = static_cast<double>(__crt_va_arg(va, double));
		rgain = static_cast<double>(__crt_va_arg(va, double));
		enable = static_cast<bool>(__crt_va_arg(va, bool));
		__crt_va_end(va);
		rt = pProcess(data, argNum, bgain, ggain, rgain, enable);
		break;
	case CC:
		pProcess = &ColorCorrection;
		__crt_va_start(va, argNum);
		enable = static_cast<bool>(__crt_va_arg(va, bool));
		__crt_va_end(va);
		rt = pProcess(data, argNum, enable);
		break;
	case GAMMA:
		pProcess = &GammaCorrection;
		__crt_va_start(va, argNum);
		enable = static_cast<bool>(__crt_va_arg(va, bool));
		__crt_va_end(va);
		rt = pProcess(data, argNum, enable);
		break;
	case SWNR:
		pProcess = &SmallWaveNR;
		__crt_va_start(va, argNum);
		Imgsizey = static_cast<int>(__crt_va_arg(va, int));
		Imgsizex = static_cast<int>(__crt_va_arg(va, int));
		strength1 = static_cast<int>(__crt_va_arg(va, int));
		strength2 = static_cast<int>(__crt_va_arg(va, int));
		strength3 = static_cast<int>(__crt_va_arg(va, int));
		enable = static_cast<bool>(__crt_va_arg(va, bool));
		__crt_va_end(va);
		rt = pProcess(data, argNum, Imgsizey, Imgsizex, strength1, strength2, strength3, enable);
		break;
	case SHARPNESS:
		pProcess = &Sharpness;
		__crt_va_start(va, argNum);
		enable = static_cast<bool>(__crt_va_arg(va, bool));
		__crt_va_end(va);
		rt = pProcess(data, argNum, enable);
		break;
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
