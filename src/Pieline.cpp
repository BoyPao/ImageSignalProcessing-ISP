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


ISPResult ISPNode::Init(PROCESS_TYPE type, ISPParamManager* pPM)//, Args&&... args) {
{
	ISPResult result = ISP_SUCCESS;

	mName = PROCESSNAME[type];
	mType = type;
	//mEnable = true;
	pProcess = nullptr;
	//isNecessary = true;  //Wait to do. Distinwish necessary process.	
	next = nullptr;

	result = RegisterISPLibFuncs();
	if (pPM) {
		pParamManager = pPM;
	}

	mInited = true;

	return result;
}

ISPResult ISPNode::Process(void* data, int32_t argNum, ...)
{
	ISPResult rt = ISP_SUCCESS;
	if (!mInited) {
		rt = ISP_STATE_ERROR;
	}

	if (SUCCESS(rt)) {
		va_list va;
		bool enable;
		switch (mType) {
		case BLC:
			rt = BlackLevelCorrection(pParamManager, data);
			break;
		case LSC:
			rt = LensShadingCorrection(pParamManager, data);
			break;
		case WB:
			rt = WhiteBalance(pParamManager, data);
			break;
		case CC:
			rt = ColorCorrection(pParamManager, data);
			break;
		case GAMMA:
			rt = GammaCorrection(pParamManager, data);
			break;
		case WNR:
			int32_t Imgsizey, Imgsizex;
			__crt_va_start(va, argNum);
			Imgsizey = static_cast<int32_t>(__crt_va_arg(va, int32_t));
			Imgsizex = static_cast<int32_t>(__crt_va_arg(va, int32_t));
			__crt_va_end(va);
			rt = WaveletNR(pParamManager, data, Imgsizey, Imgsizex);
			break;
		case SHARPNESS:
			rt = Sharpness(pParamManager, data);
			break;
		case NONE:
		case PROCESS_TYPE_MAX:
		default:
			rt = ISP_FAILED;
		}
	}

	return rt;
}

ISPResult ISPNode::getNodeName(string* name)
{ 
	ISPResult rt = ISP_SUCCESS;
	if (mInited) {
		name = &mName;
	} else {
		rt = ISP_STATE_ERROR;
	}
	return rt;
}

/*Pipeline* CreateDefaultPipeline()
{
	Pipeline* pPipeline = new Pipeline;
	pPipeline->AddNode(CST);
}*/