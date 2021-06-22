//////////////////////////////////////////////////////////////////////////////////////
// Author: Peng Hao <635945005@qq.com>
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
	"Sharpness",
	"Color Space Transfromation",
	"None",
};

const string COLOR_SPACE_NAME[] = {
	"Bayer",
	"RGB",
	"YUV",
	"None",
};

class ISPNode {
	typedef ISPResult(*FUNCTPRT)(void* data, int32_t argNum, ...);
public:
	ISPResult getNodeName(string* name);
	ISPResult Init(PROCESS_TYPE type);
	ISPResult Process(void* data, int32_t argNum, ...);
	
private:
	
	FUNCTPRT pProcess;
	//bool mEnable;
	bool mInited;
	//bool isNecessary;
	string mName;
	PROCESS_TYPE mType;
	ISPNode* next;
};

//Color Space Change Node
class ISPCSTNode : public ISPNode {   
public:
	ISPResult Process(void* data, int32_t argNum, ...);
private:
	string mSrcColorSpace;
	string mDstColorSpace;
	bool isNecessary;
};

class Pipeline {
public:
	Pipeline() {};
	~Pipeline() {};
	//Wait to develop NodeList
	Pipeline* CreateDefaultPipeline();
	ISPResult AddNode(PROCESS_TYPE type);
	int32_t GetNodeNum();

private:
	ISPNode mHead;
	bool mInited;
	int32_t mNodeNum;
};

