//////////////////////////////////////////////////////////////////////////////////////
// Author: Peng Hao
// License: GPL
//////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////
// @file: Pipeline.h
// @brief: The head of Pipeline
//////////////////////////////////////////////////////////////////////////////////////

#pragma once
#include "ImageSignalProcess.h"

const string PROCESSNAME[] = {
	"Black Level Correction",
	"Lens Shading Correction",
	"Green Channels Correction",
	"White Balance",
	"Color Correction",
	"Gamma Correction",
	"Small Wave Niose Reduction",
	"Sharpness"
};

class ISPNode {
	typedef ISPResult(*FUNCTPRT)(void* data, uint32_t argNum, ...);
public:
	ISPResult getNodeName(string* name);
	ISPResult Init(PROCESS_TYPE type);
	ISPResult Process(void* data, uint32_t argNum, ...);
	
private:
	
	FUNCTPRT pProcess;
	bool mEnable;
	bool mInited;
	bool isNecessary;
	string mName;
	PROCESS_TYPE mType;
	ISPNode* next;
};

class Pipeline {
public:
	Pipeline() {};
	~Pipeline() {};
	//Wait to develop NodeList
private:
	ISPNode mHead;
};