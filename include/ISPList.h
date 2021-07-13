//////////////////////////////////////////////////////////////////////////////////////
// Author: Peng Hao <635945005@qq.com>
// License: GPL
//////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////
// @file: ISPList.h
// @brief: The head of ISPList.
//////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "AlgorithmInterface.h"

#define NODE_NAME_MAX_SZIE 15

typedef enum PROCESS_TYPE {
	PROCESS_BLC = 0,
	PROCESS_LSC,
	PROCESS_WB,
	PROCESS_CC,
	PROCESS_GAMMA,
	PROCESS_WNR,
	PROCESS_EE,
	PROCESS_CST_RAW2RGB,
	PROCESS_CST_RGB2YUV,
	PROCESS_NONE,
	PROCESS_TYPE_NUM
};

const char PROCESSNAME[PROCESS_TYPE_NUM][NODE_NAME_MAX_SZIE] = {
	"BLC",
	"LSC",
	"WB",
	"CC",
	"Gamma",
	"WNR",
	"EE",
	"CST_Demisaic",
	"CST_RGB2YUV",
	"None",
};

typedef enum NODE_SWITCH {
	NODE_OFF = 0,
	NODE_ON
};

typedef enum NODE_PRIORITY {
	PRIORITY_NECESSARY,
	PRIORITY_FREE,
};

struct ISP_NODE_PROPERTY {
	char name[NODE_NAME_MAX_SZIE];
	PROCESS_TYPE type;
	NODE_SWITCH enable;
	NODE_PRIORITY priority;
};

typedef enum LIST_TYPE {
	RAW_LIST,
	RGB_LIST,
	YUV_LIST
};

struct ISP_LIST_PROPERTY {
	int32_t nodeNum;
	ISP_NODE_PROPERTY* pNodeConfig;
};

template<typename T1, typename T2>
class ISPNode {
public:
	ISPNode();
	virtual ~ISPNode();
	virtual ISPResult GetNodeName(char* name);
	virtual ISPResult Init(PROCESS_TYPE type, T1* input, T2* output);
	ISPResult Init(PROCESS_TYPE type, ISPParamManager* pPM);
	ISPResult Process(void* data, int32_t argNum, ...);
	virtual ISPResult Process(ISPParamManager* pPM);
	virtual ISPResult Enable();
	virtual ISPResult Disable();
	virtual bool isOn();

	ISPNode<T2,T2>* pNext;
	
private:
	bool mInited;
	ISP_NODE_PROPERTY mProperty;
	T1* pInputBuffer;
	T2* pOutputBuffer;
	ISPParamManager* pParamManager;
};

template<typename T1, typename T2>
class ISPNecNode : public ISPNode<T1, T2> {   
public:
	ISPResult Disable();
private:
};

template<typename T1, typename T2, typename T3>
class ISPList {
public:
	ISPList();
	~ISPList();
	ISPResult Init(T1* pRawBuf, T2* pRgbBuf, T3* pYuvBuf, ISPParamManager* pPM);
	//Wait to develop NodeList
	ISPResult CreatISPList();
	ISPResult AddNode(PROCESS_TYPE type);
	int32_t GetNodeNum();
	ISPResult Process();

private:
	ISPResult CreateNecList();
	ISPResult CreateRawList();
	ISPResult CreateRgbList();
	ISPResult CreateYuvList();
	ISPResult AddNodeToRawTail(ISPNode<T1, T1>* pNode);
	ISPResult AddNodeToRgbTail(ISPNode<T2, T2>* pNode);
	ISPResult AddNodeToYuvTail(ISPNode<T3, T3>* pNode);
	ISPResult TriggerRgbProcess();
	ISPResult RgbProcess();
	ISPResult TriggerYuvProcess();
	ISPResult YuvProcess();

	ISPNecNode<T1, T1>* mRawHead;
	ISPNecNode<T1, T2>* mRgbHead;
	ISPNecNode<T2, T3>* mYuvHead;
	bool mInited;
	ISPParamManager* pParamManager;
	int32_t mNodeNum;
	T1* pRawBuffer;
	T2* pRgbBuffer;
	T3* pYuvBuffer;

};

PROCESS_TYPE RawNodeConfigure[] = {
	PROCESS_BLC,
	PROCESS_LSC,
};

PROCESS_TYPE RgbNodeConfigure[] = {
	PROCESS_WB,
	PROCESS_CC,
	PROCESS_GAMMA,
};

PROCESS_TYPE YuvNodeConfigure[] = {
	PROCESS_WNR,
	PROCESS_EE,
};

ISP_NODE_PROPERTY gNodesConfigure[] = {
	{"HEAD",			PROCESS_NONE,			NODE_ON,	PRIORITY_NECESSARY},
	{"BLC",				PROCESS_BLC,			NODE_ON,	PRIORITY_FREE},
	{"LSC",				PROCESS_LSC,			NODE_ON,	PRIORITY_FREE},
	{"CST_Demosaic",	PROCESS_CST_RAW2RGB,	NODE_ON,	PRIORITY_NECESSARY},
	{"WB",				PROCESS_WB,				NODE_ON,	PRIORITY_FREE},
	{"CC",				PROCESS_CC,				NODE_ON,	PRIORITY_FREE},
	{"Gamma",			PROCESS_GAMMA,			NODE_ON,	PRIORITY_FREE},
	{"CST_RGB2YUV",		PROCESS_CST_RGB2YUV,	NODE_ON,	PRIORITY_NECESSARY},
	{"WNR",				PROCESS_WNR,			NODE_ON,	PRIORITY_FREE},
	{"EE",				PROCESS_EE,				NODE_ON,	PRIORITY_FREE},
};

ISP_LIST_PROPERTY gISPListConfig = {
	sizeof(gNodesConfigure) / sizeof(ISP_NODE_PROPERTY),
	gNodesConfigure
};



template<typename T1, typename T2>
ISPNode<T1, T2>::ISPNode() :
	pNext(nullptr),
	mInited(false),
	pInputBuffer(nullptr),
	pOutputBuffer(nullptr)
{
	memset(&mProperty, 0, sizeof(ISP_NODE_PROPERTY));
}

template<typename T1, typename T2>
ISPNode<T1, T2>::~ISPNode()
{
	pNext = nullptr;
	mInited = false;
	pInputBuffer = nullptr;
	pOutputBuffer = nullptr;
	memset(&mProperty, 0, sizeof(ISP_NODE_PROPERTY));
}

template<typename T1, typename T2>
ISPResult ISPNode<T1, T2>::Init(PROCESS_TYPE type, T1* input, T2* output)
{
	ISPResult result = ISP_SUCCESS;

	if (!input || !output) {
		result = ISP_INVALID_PARAM;
	}

	if (SUCCESS(result)) {
		for (int32_t index = 0; index < sizeof(gNodesConfigure) / sizeof(ISP_NODE_PROPERTY); index++) {
			if (type == gNodesConfigure[index].type) {
				memcpy(&mProperty, &gNodesConfigure[index], sizeof(ISP_NODE_PROPERTY));
			}
		}
	}

	if (SUCCESS(result)) {
		pInputBuffer = input;
		pOutputBuffer = output;
	}

	if (SUCCESS(result)) {
		mInited = true;
	}

	return result;
}

template<typename T1, typename T2>
ISPResult ISPNode<T1, T2>::Enable()
{
	ISPResult result = ISP_SUCCESS;

	mProperty.enable = NODE_ON;

	return result;
}

template<typename T1, typename T2>
ISPResult ISPNode<T1, T2>::Disable()
{
	ISPResult result = ISP_SUCCESS;

	mProperty.enable = NODE_OFF;

	return result;
}

template<typename T1, typename T2>
bool ISPNode<T1, T2>::isOn()
{
	return (mProperty.enable == NODE_ON) ? true : false;
}

template<typename T1, typename T2>
ISPResult ISPNode<T1, T2>::GetNodeName(char* name)
{
	ISPResult rt = ISP_SUCCESS;
	if (mInited) {
		memcpy(name, mProperty.name, sizeof(char) * NODE_NAME_MAX_SZIE);
	}
	else {
		rt = ISP_STATE_ERROR;
	}
	return rt;
}

template<typename T1, typename T2>
ISPResult ISPNode<T1, T2>::Process(ISPParamManager* pPM)
{
	ISPResult result = ISP_SUCCESS;

	if (!mInited) {
		result = ISP_STATE_ERROR;
		ISPLoge("Node is not inited!");
	}

	if (!pPM) {
		result = ISP_INVALID_PARAM;
	}

	if (SUCCESS(result)) {
		bool enable = mProperty.enable;
		switch (mProperty.type) {
		case PROCESS_BLC:
			result = enable ? BlackLevelCorrection(pPM, pInputBuffer) : ISP_SKIP;
			break;
		case PROCESS_LSC:
			result = enable ? LensShadingCorrection(pPM, pInputBuffer) : ISP_SKIP;
			break;
		case PROCESS_WB:
			result = enable ? WhiteBalance(pPM, pInputBuffer) : ISP_SKIP;
			break;
		case PROCESS_CC:
			result = enable ? ColorCorrection(pPM, pInputBuffer) : ISP_SKIP;
			break;
		case PROCESS_GAMMA:
			result = enable ? GammaCorrection(pPM, pInputBuffer) : ISP_SKIP;
			break;
		case PROCESS_WNR:
			result = enable ? WaveletNR(pPM, pInputBuffer) : ISP_SKIP;
			break;
		case PROCESS_EE:
			result = enable ? Sharpness(pPM, pInputBuffer) : ISP_SKIP;
			break;
		case PROCESS_NONE:
			result = ISP_SKIP;
			break;
		case PROCESS_CST_RAW2RGB:
			result = Demosaic(pPM, pInputBuffer, pOutputBuffer);
			break;
		case PROCESS_CST_RGB2YUV:
			result = CST_RGB2YUV(pPM, pInputBuffer, pOutputBuffer);
			break;
		case PROCESS_TYPE_NUM:
		default:
			result = ISP_FAILED;
		}
	}

	char name[NODE_NAME_MAX_SZIE];
	GetNodeName(name);
	ISPLogi("%s:Process finished.", name);

	return result;
}

template<typename T1, typename T2>
ISPResult ISPNecNode<T1, T2>::Disable()
{
	ISPResult result = ISP_SUCCESS;

	char name[NODE_NAME_MAX_SZIE];
	this->GetNodeName(name);
	ISPLogw("Try to disable necessary node:%s", name);

	return result;
}


template<typename T1, typename T2, typename T3>
ISPList<T1, T2, T3>::ISPList() :
	mRawHead(nullptr),
	mRgbHead(nullptr),
	mYuvHead(nullptr),
	mInited(false),
	pParamManager(nullptr),
	mNodeNum(0)
{


}

template<typename T1, typename T2, typename T3>
ISPList<T1, T2, T3>::~ISPList()
{
	if (mRawHead) {
		ISPNode<T1, T1>* pRawNext = mRawHead->pNext;
		while (pRawNext != nullptr) {
			mRawHead->pNext = pRawNext->pNext;
			delete pRawNext;
			mNodeNum--;
			pRawNext = mRawHead->pNext;
		}
		delete mRawHead;
		mRawHead = nullptr;
		mNodeNum--;
	}

	if (mRgbHead) {
		ISPNode<T2, T2>* pRgbNext = mRgbHead->pNext;
		while (pRgbNext != nullptr) {
			mRgbHead->pNext = pRgbNext->pNext;
			delete pRgbNext;
			mNodeNum--;
			pRgbNext = mRgbHead->pNext;
		}
		delete mRgbHead;
		mRgbHead = nullptr;
		mNodeNum--;

	}

	if (mYuvHead) {
		ISPNode<T3, T3>* pYuvNext = mYuvHead->pNext;
		while (pYuvNext != nullptr) {
			mYuvHead->pNext = pYuvNext->pNext;
			delete pYuvNext;
			mNodeNum--;
			pYuvNext = mYuvHead->pNext;
		}
		delete mYuvHead;
		mYuvHead = nullptr;
		mNodeNum--;
	}

	pParamManager = nullptr;

	ISPLogd("Node num:%d", mNodeNum);
}

template<typename T1, typename T2, typename T3>
ISPResult ISPList<T1, T2, T3>::Init(T1* pRawBuf, T2* pRgbBuf, T3* pYuvBuf, ISPParamManager* pPM)
{
	ISPResult result = ISP_SUCCESS;

	if (!pRawBuf || !pRgbBuf || !pYuvBuf || !pPM) {
		result = ISP_INVALID_PARAM;
		ISPLoge("Failed to init list, pBuffer or PM is null! result:%d", result);
	}

	if (SUCCESS(result)) {
		pRawBuffer = pRawBuf;
		pRgbBuffer = pRgbBuf;
		pYuvBuffer = pYuvBuf;
	}

	if(SUCCESS(result)){
		pParamManager = pPM;
	}

	if (SUCCESS(result)) {
		result = RegisterISPLibFuncs();
	}

	if (SUCCESS(result)) {
		mInited = true;
	}

	return result;
}

template<typename T1, typename T2, typename T3>
ISPResult ISPList<T1, T2, T3>::CreatISPList()
{
	ISPResult result = ISP_SUCCESS;

	result = CreateNecList();

	if (SUCCESS(result)) {
		result = CreateRawList();
	}

	if (SUCCESS(result)) {
		result = CreateRgbList();
	}

	if (SUCCESS(result)) {
		result = CreateYuvList();
	}

	ISPLogd("Current node num:%d", mNodeNum);

	return result;
}

template<typename T1, typename T2, typename T3>
ISPResult ISPList<T1, T2, T3>::CreateNecList()
{
	ISPResult result = ISP_SUCCESS;

	if (!mInited) {
		result = ISP_STATE_ERROR;
		ISPLoge("List not inited! result:%d", result);
	}

	if (mRawHead || mRgbHead || mYuvHead)
	{
		result = ISP_STATE_ERROR;
		ISPLoge("Create new list failed! Old list exits! result:%d", result);
	}

	//Head node create
	if (SUCCESS(result)) {
		mRawHead = new ISPNecNode<T1, T1>;
		if (mRawHead) {
			mRawHead->Init(PROCESS_NONE, pRawBuffer, pRawBuffer);
			mNodeNum++;
		}
		else {
			result = ISP_MEMORY_ERROR;
			ISPLoge("New node failed! %d", result);
		}
	}

	//Raw -> Rgb node create
	if (SUCCESS(result)) {
		mRgbHead = new ISPNecNode<T1, T2>;
		if (mRgbHead) {
			mRgbHead->Init(PROCESS_CST_RAW2RGB, pRawBuffer, pRgbBuffer);
			mNodeNum++;
		}
		else {
			result = ISP_MEMORY_ERROR;
			ISPLoge("New node failed! %d", result);
		}
	}

	//Rgb -> YUV node create
	if (SUCCESS(result)) {
		mYuvHead = new ISPNecNode<T2, T3>;
		if (mYuvHead) {
			mYuvHead->Init(PROCESS_CST_RGB2YUV, pRgbBuffer, pYuvBuffer);
			mNodeNum++;
		}
		else {
			result = ISP_MEMORY_ERROR;
			ISPLoge("New node failed! %d", result);
		}
	}

	ISPLogd("Current node num:%d", mNodeNum);

	return result;
}

template<typename T1, typename T2, typename T3>
ISPResult ISPList<T1, T2, T3>::CreateRawList()
{
	ISPResult result = ISP_SUCCESS;

	ISPNode<T1, T1>* pNewNode = nullptr;
	PROCESS_TYPE newNodeType = PROCESS_NONE;
	for (int32_t i = 0; i < sizeof(RawNodeConfigure) / sizeof(PROCESS_TYPE); i++) {
		newNodeType = RawNodeConfigure[i];
		pNewNode = new ISPNode<T1, T1>;
		if (pNewNode) {
			result = pNewNode->Init(newNodeType, pRawBuffer, pRawBuffer);
			if (SUCCESS(result)) {
				result = AddNodeToRawTail(pNewNode);
				if (!SUCCESS(result)) {
					break;
				}
			}
			else {
				break;
			}
		}
		else {
			result = ISP_MEMORY_ERROR;
			ISPLoge("Failed to new node! result:%d", result);
			break;
		}
	}

	if (SUCCESS(result)) {
		char name[NODE_NAME_MAX_SZIE] = { '0' };
		ISPNode<T1, T1>* pNode = mRawHead;
		while (pNode) {
			pNode->GetNodeName(name);
			ISPLogd("List node:%s", name);
			pNode = pNode->pNext;
		}
	}

	return result;
}

template<typename T1, typename T2, typename T3>
ISPResult ISPList<T1, T2, T3>::AddNodeToRawTail(ISPNode<T1, T1>* pNode)
{
	ISPResult result = ISP_SUCCESS;

	ISPNode<T1, T1>* pTmp = mRawHead;

	while (pTmp) {
		if (pTmp->pNext) {
			pTmp = pTmp->pNext;
		}
		else {
			break;
		}
	}

	pTmp->pNext = pNode;
	mNodeNum++;

	return result;
}

template<typename T1, typename T2, typename T3>
ISPResult ISPList<T1, T2, T3>::CreateRgbList()
{
	ISPResult result = ISP_SUCCESS;

	ISPNode<T2, T2>* pNewNode = nullptr;
	PROCESS_TYPE newNodeType = PROCESS_NONE;
	for (int32_t i = 0; i < sizeof(RgbNodeConfigure) / sizeof(PROCESS_TYPE); i++) {
		newNodeType = RgbNodeConfigure[i];
		pNewNode = new ISPNode<T2, T2>;
		if (pNewNode) {
			result = pNewNode->Init(newNodeType, pRgbBuffer, pRgbBuffer);
			if (SUCCESS(result)) {
				result = AddNodeToRgbTail(pNewNode);
				if (!SUCCESS(result)) {
					break;
				}
			}
			else {
				break;
			}
		}
		else {
			result = ISP_MEMORY_ERROR;
			ISPLoge("Failed to new node! result:%d", result);
			break;
		}
	}

	if (SUCCESS(result)) {
		char name[NODE_NAME_MAX_SZIE] = { '0' };
		mRgbHead->GetNodeName(name);
		ISPLogd("List node:%s", name);
		ISPNode<T2, T2>* pNode = mRgbHead->pNext;
		while (pNode) {
			pNode->GetNodeName(name);
			ISPLogd("List node:%s", name);
			pNode = pNode->pNext;
		}
	}

	return result;
}

template<typename T1, typename T2, typename T3>
ISPResult ISPList<T1, T2, T3>::AddNodeToRgbTail(ISPNode<T2, T2>* pNode)
{
	ISPResult result = ISP_SUCCESS;

	ISPNode<T2, T2>* pTmp = mRgbHead->pNext;
	if (pTmp) {
		while (pTmp) {
			if (pTmp->pNext) {
				pTmp = pTmp->pNext;
			}
			else {
				break;
			}
		}
		pTmp->pNext = pNode;
	}
	else {
		mRgbHead->pNext = pNode;
		
	}

	mNodeNum++;
	return result;
}

template<typename T1, typename T2, typename T3>
ISPResult ISPList<T1, T2, T3>::CreateYuvList()
{
	ISPResult result = ISP_SUCCESS;

	ISPNode<T3, T3>* pNewNode = nullptr;
	PROCESS_TYPE newNodeType = PROCESS_NONE;
	for (int32_t i = 0; i < sizeof(YuvNodeConfigure) / sizeof(PROCESS_TYPE); i++) {
		newNodeType = YuvNodeConfigure[i];
		pNewNode = new ISPNode<T3, T3>;
		if (pNewNode) {
			result = pNewNode->Init(newNodeType, pYuvBuffer, pYuvBuffer);
			if (SUCCESS(result)) {
				result = AddNodeToYuvTail(pNewNode);
				if (!SUCCESS(result)) {
					break;
				}
			}
			else {
				break;
			}
		}
		else {
			result = ISP_MEMORY_ERROR;
			ISPLoge("Failed to new node! result:%d", result);
			break;
		}
	}

	if (SUCCESS(result)) {
		char name[NODE_NAME_MAX_SZIE] = { '0' };
		mYuvHead->GetNodeName(name);
		ISPLogd("List node:%s", name);
		ISPNode<T3, T3>* pNode = mYuvHead->pNext;
		while (pNode) {
			pNode->GetNodeName(name);
			ISPLogd("List node:%s", name);
			pNode = pNode->pNext;
		}
	}

	return result;
}

template<typename T1, typename T2, typename T3>
ISPResult ISPList<T1, T2, T3>::AddNodeToYuvTail(ISPNode<T3, T3>* pNode)
{
	ISPResult result = ISP_SUCCESS;

	ISPNode<T3, T3>* pTmp = mYuvHead->pNext;
	if (pTmp) {
		while (pTmp) {
			if (pTmp->pNext) {
				pTmp = pTmp->pNext;
			}
			else {
				break;
			}
		}
		pTmp->pNext = pNode;
	}
	else {
		mYuvHead->pNext = pNode;

	}

	mNodeNum++;
	return result;
}

template<typename T1, typename T2, typename T3>
ISPResult ISPList<T1, T2, T3>::Process() 
{
	ISPResult result = ISP_SUCCESS;

	ISPNode<T1, T1>* pNode = mRawHead;
	while (pNode) {
		result = pNode->Process(pParamManager);
		if (SUCCESS(result)) {
			pNode = pNode->pNext;
		} 
		else {
			char name[NODE_NAME_MAX_SZIE] = { '\0' };
			pNode->GetNodeName(name);
			ISPLoge("%s node process failed! result:%d", name, result);
			break;
		}
	}

	if (SUCCESS(result)) {
		ISPLogd(">>>>> Raw process finished! >>>>>");
		TriggerRgbProcess();
	}

	return result;
}

template<typename T1, typename T2, typename T3>
ISPResult ISPList<T1, T2, T3>::TriggerRgbProcess()
{
	ISPResult result = ISP_SUCCESS;

	result = RgbProcess();

	return result;
}

template<typename T1, typename T2, typename T3>
ISPResult ISPList<T1, T2, T3>::RgbProcess()
{
	ISPResult result = ISP_SUCCESS;

	result = mRgbHead->Process(pParamManager);

	if (SUCCESS(result)) {
		ISPNode<T2, T2>* pNode = mRgbHead->pNext;
		while (pNode) {
			result = pNode->Process(pParamManager);
			if (SUCCESS(result)) {
				pNode = pNode->pNext;
			}
			else {
				char name[NODE_NAME_MAX_SZIE] = { '\0' };
				pNode->GetNodeName(name);
				ISPLoge("%s node process failed! result:%d", name, result);
				break;
			}
		}
	}

	if (SUCCESS(result)) {
		ISPLogd(">>>>> Rgb process finished! >>>>>");
		TriggerYuvProcess();
	}

	return result;
}

template<typename T1, typename T2, typename T3>
ISPResult ISPList<T1, T2, T3>::TriggerYuvProcess()
{
	ISPResult result = ISP_SUCCESS;

	result = YuvProcess();

	return result;
}

template<typename T1, typename T2, typename T3>
ISPResult ISPList<T1, T2, T3>::YuvProcess() 
{
	ISPResult result = ISP_SUCCESS;

	result = mYuvHead->Process(pParamManager);

	if (SUCCESS(result)) {
		ISPNode<T3, T3>* pNode = mYuvHead->pNext;
		while (pNode) {
			result = pNode->Process(pParamManager);
			if (SUCCESS(result)) {
				pNode = pNode->pNext;
			}
			else {
				char name[NODE_NAME_MAX_SZIE] = { '\0' };
				pNode->GetNodeName(name);
				ISPLoge("%s node process failed! result:%d", name, result);
				break;
			}
		}
	}

	if (SUCCESS(result)) {
		ISPLogd(">>>>> Yuv process finished! >>>>>");
	}

	return result;
}

template<typename T1, typename T2>
ISPResult ISPNode<T1, T2>::Init(PROCESS_TYPE type, ISPParamManager* pPM)//, Args&&... args) {
{
	ISPResult result = ISP_SUCCESS;

	if (pPM) {
		pParamManager = pPM;
	}
	mProperty.type = type;

	pNext = nullptr;

	result = RegisterISPLibFuncs();


	mInited = true;

	return result;
}


template<typename T1, typename T2>
ISPResult ISPNode<T1, T2>::Process(void* data, int32_t argNum, ...)
{
	ISPResult rt = ISP_SUCCESS;
	if (!mInited) {
		rt = ISP_STATE_ERROR;
	}

	if (SUCCESS(rt)) {
		va_list va;
		bool enable;
		switch (mProperty.type) {
		case PROCESS_BLC:
			rt = BlackLevelCorrection(pParamManager, data);
			break;
		case PROCESS_LSC:
			rt = LensShadingCorrection(pParamManager, data);
			break;
		case PROCESS_WB:
			rt = WhiteBalance(pParamManager, data);
			break;
		case PROCESS_CC:
			rt = ColorCorrection(pParamManager, data);
			break;
		case PROCESS_GAMMA:
			rt = GammaCorrection(pParamManager, data);
			break;
		case PROCESS_WNR:
			rt = WaveletNR(pParamManager, data);
			break;
		case PROCESS_EE:
			rt = Sharpness(pParamManager, data);
			break;
		case PROCESS_NONE:
		case PROCESS_TYPE_NUM:
		default:
			rt = ISP_FAILED;
		}
	}

	return rt;
}