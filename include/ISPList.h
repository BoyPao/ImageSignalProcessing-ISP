//////////////////////////////////////////////////////////////////////////////////////
// Author: Peng Hao <635945005@qq.com>
// License: GPL
//////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////
// @file: ISPList.h
// @brief: ISPList is a serial abstract object helps to construct processing 
// steps more flexabl.
//////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "AlgorithmInterface.h"
#include "ISPListConfig.h"

const PROCESS_TYPE RawNodeConfigure[] = {
	PROCESS_BLC,
	PROCESS_LSC,
};

const PROCESS_TYPE RgbNodeConfigure[] = {
	PROCESS_WB,
	PROCESS_CC,
	PROCESS_GAMMA,
};

const PROCESS_TYPE YuvNodeConfigure[] = {
	PROCESS_WNR,
	PROCESS_EE,
};

template<typename T1, typename T2>
class ISPNode {
public:
	ISPNode();
	virtual ~ISPNode();
	virtual ISPResult GetNodeName(char* name);
	virtual ISPResult Init(ISP_NODE_PROPERTY* cfg, T1* input, T2* output);
	virtual ISPResult Process(ISPParamManager* pPM);
	virtual ISPResult Enable();
	virtual ISPResult Disable();
	virtual bool isOn();
	virtual ISP_NODE_PROPERTY GetProperty();

	ISPNode<T2,T2>* pNext;
	
protected:
	ISP_NODE_PROPERTY mProperty;

private:
	bool mInited;
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

enum LIST_TYPE {
	RAW_LIST,
	RGB_LIST,
	YUV_LIST,
	LIST_TYPE_NUM
};

struct ISP_LIST_PROPERTY {
	int32_t nodeNum;
	ISP_NODE_PROPERTY* pNodeConfig;
};

enum ISP_LIST_STATE {
	ISP_LIST_NEW = 0,
	ISP_LIST_INITED,
	ISP_LIST_CONFIGED,
	ISP_LIST_CONSTRUCTED,
	ISP_LIST_RUNNING
};

enum STATE_TRANS_ORIENTATION {
	STATE_TRANS_FORWARD = 0,
	STATE_TRANS_BACKWARD,
	STATE_TRANS_TO_SELF
};

template<typename T1, typename T2, typename T3>
class ISPList {
public:
	ISPList(int32_t id);
	~ISPList();
	ISPResult Init(T1* pRawBuf, T2* pRgbBuf, T3* pYuvBuf, ISPParamManager* pPM);
	ISPResult ConfigList(ISP_LIST_PROPERTY* pCfg);
	ISPResult FindNodePropertyIndex(PROCESS_TYPE type, int32_t* index);
	ISPResult CreatISPList();
	ISPResult AddNode(PROCESS_TYPE type);
	int32_t GetNodeNum();
	ISPResult Process();
	ISPResult EnableNodebyType(PROCESS_TYPE type);
	ISPResult DisableNodebyType(PROCESS_TYPE type);
	ISPResult EnableNecNodebyType(PROCESS_TYPE type);
	ISPResult DisableNecNodebyType(PROCESS_TYPE type);

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
	ISPResult StateTransform(STATE_TRANS_ORIENTATION orientation);

	int32_t mId;
	ISPNecNode<T1, T1>* mRawHead;
	ISPNecNode<T1, T2>* mRgbHead;
	ISPNecNode<T2, T3>* mYuvHead;
	ISPParamManager* pParamManager;
	int32_t mNodeNum;
	T1* pRawBuffer;
	T2* pRgbBuffer;
	T3* pYuvBuffer;
	ISP_LIST_PROPERTY mProperty;
	ISP_LIST_STATE mState;
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
ISPResult ISPNode<T1, T2>::Init(ISP_NODE_PROPERTY *cfg, T1* input, T2* output)
{
	ISPResult result = ISP_SUCCESS;

	if (!input || !output || !cfg) {
		result = ISP_INVALID_PARAM;
	}

	if (SUCCESS(result)) {
		memcpy(&mProperty, cfg, sizeof(ISP_NODE_PROPERTY));
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
ISP_NODE_PROPERTY ISPNode<T1, T2>::GetProperty()
{
	return mProperty;
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
			result = enable ? BlackLevelCorrection(pInputBuffer, pPM) : ISP_SKIP;
			break;
		case PROCESS_LSC:
			result = enable ? LensShadingCorrection(pInputBuffer, pPM) : ISP_SKIP;
			break;
		case PROCESS_WB:
			result = enable ? WhiteBalance(pInputBuffer, pPM) : ISP_SKIP;
			break;
		case PROCESS_CC:
			result = enable ? ColorCorrection(pInputBuffer, pPM) : ISP_SKIP;
			break;
		case PROCESS_GAMMA:
			result = enable ? GammaCorrection(pInputBuffer, pPM) : ISP_SKIP;
			break;
		case PROCESS_WNR:
			result = enable ? WaveletNR(pInputBuffer, pPM) : ISP_SKIP;
			break;
		case PROCESS_EE:
			result = enable ? EdgeEnhancement(pInputBuffer, pPM) : ISP_SKIP;
			break;
		case PROCESS_HEAD:
			result = ISPLibParamsInit(pPM);
			break;
		case PROCESS_CST_RAW2RGB:
			result = Demosaic(pInputBuffer, pOutputBuffer, pPM, enable);
			break;
		case PROCESS_CST_RGB2YUV:
			result = CST_RGB2YUV(pInputBuffer, pOutputBuffer, pPM, enable);
			break;
		case PROCESS_TYPE_NUM:
			result = ISP_SKIP;
			break;
		default:
			result = ISP_FAILED;
		}
	}

	char name[NODE_NAME_MAX_SZIE];
	GetNodeName(name);
	if (result == ISP_SUCCESS) {
		ISPLogi("%s:Process finished.", name);
	}
	else if (result == ISP_SKIP) {
		ISPLogi("%s:Skiped.", name);
	}

	return result;
}

template<typename T1, typename T2>
ISPResult ISPNecNode<T1, T2>::Disable()
{
	ISPResult result = ISP_SUCCESS;

	char name[NODE_NAME_MAX_SZIE];
	this->GetNodeName(name);
	ISPLogw("Try to disable necessary node:%s", name);
	this->mProperty.enable = NODE_OFF;

	return result;
}

template<typename T1, typename T2, typename T3>
ISPList<T1, T2, T3>::ISPList(int32_t id) :
	mRawHead(nullptr),
	mRgbHead(nullptr),
	mYuvHead(nullptr),
	pParamManager(nullptr),
	mNodeNum(0),
	mState(ISP_LIST_NEW)
{
	mId = id;
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

	ISPLogd("List(%d) Node num:%d", mId, mNodeNum);
}

template<typename T1, typename T2, typename T3>
ISPResult ISPList<T1, T2, T3>::StateTransform(STATE_TRANS_ORIENTATION orientation)
{
	ISPResult result = ISP_SUCCESS;
	ISP_LIST_STATE currentState = mState;

	if (orientation == STATE_TRANS_FORWARD) {
		switch (currentState) {
		case ISP_LIST_NEW:
			mState = ISP_LIST_INITED;
			break;
		case ISP_LIST_INITED:
			mState = ISP_LIST_CONFIGED;
			break;
		case ISP_LIST_CONFIGED:
			mState = ISP_LIST_CONSTRUCTED;
			break;
		case ISP_LIST_CONSTRUCTED:
			mState = ISP_LIST_RUNNING;
			break;
		case ISP_LIST_RUNNING:
			mState = ISP_LIST_CONSTRUCTED;
			break;
		default:
			ISPLogw("Undefined forward state transform! state:%d", currentState);
			break;
		}
	}
	else if (orientation == STATE_TRANS_BACKWARD){
		switch (currentState) {
		case ISP_LIST_INITED:
			mState = ISP_LIST_NEW;
			break;
		case ISP_LIST_CONSTRUCTED:
			mState = ISP_LIST_INITED;
			break;
		default:
			ISPLogw("Undefined backward state transform! state:%d", currentState);
			break;
		}
	}
	else if (orientation != STATE_TRANS_TO_SELF) {
		result = ISP_INVALID_PARAM;
		ISPLoge("Invaled orientation:%d %d", orientation, result);
	}

	if (SUCCESS(result)) {
		ISPLogd("State: %d -> %d", currentState, mState);
	}

	return result;
}

template<typename T1, typename T2, typename T3>
ISPResult ISPList<T1, T2, T3>::Init(T1* pRawBuf, T2* pRgbBuf, T3* pYuvBuf, ISPParamManager* pPM)
{
	ISPResult result = ISP_SUCCESS;

	if (mState != ISP_LIST_NEW) {
		result = ISP_STATE_ERROR;
		ISPLoge("func called in invaled state:%d %d", mState, result);
	}

	if (!pRawBuf || !pRgbBuf || !pYuvBuf || !pPM) {
		result = ISP_INVALID_PARAM;
		ISPLoge("Failed to init list(%d), pBuffer or PM is null! result:%d", mId, result);
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
		RegisterISPLibFuncs();
	}

	if (SUCCESS(result)) {
		StateTransform(STATE_TRANS_FORWARD);
	}

	return result;
}

template<typename T1, typename T2, typename T3>
ISPResult ISPList<T1, T2, T3>::ConfigList(ISP_LIST_PROPERTY* pCfg)
{
	ISPResult result = ISP_SUCCESS;

	if (mState != ISP_LIST_INITED) {
		result = ISP_STATE_ERROR;
		ISPLoge("func called in invaled state:%d %d", mState, result);
	}

	if (SUCCESS(result)) {
		if (pCfg) {
			memcpy(&mProperty, pCfg, sizeof(ISP_LIST_PROPERTY));
		}
		else {
			result = ISP_INVALID_PARAM;
			ISPLoge("pCfg is null! %d", result);
		}
	}

	if (SUCCESS(result)) {
		StateTransform(STATE_TRANS_FORWARD);
	}

	return result;
}

template<typename T1, typename T2, typename T3>
ISPResult ISPList<T1, T2, T3>::FindNodePropertyIndex(PROCESS_TYPE type,int32_t* index)
{
	ISPResult result = ISP_SUCCESS;

	if (index) {
		for (int32_t i = 0; i < mProperty.nodeNum; i++)
		{
			if (type == mProperty.pNodeConfig[i].type)
			{
				*index = i;
				break;
			}
		}
	}
	else {
		result = ISP_INVALID_PARAM;
		ISPLoge("input is null! %d", result);
	}

	return result;
}

template<typename T1, typename T2, typename T3>
ISPResult ISPList<T1, T2, T3>::CreatISPList()
{
	ISPResult result = ISP_SUCCESS;

	if (mState != ISP_LIST_CONFIGED) {
		result = ISP_STATE_ERROR;
		ISPLoge("func called in invaled state:%d %d", mState, result);
	}

	if (SUCCESS(result)) {
		result = CreateNecList();
	}

	if (SUCCESS(result)) {
		result = CreateRawList();
	}

	if (SUCCESS(result)) {
		result = CreateRgbList();
	}

	if (SUCCESS(result)) {
		result = CreateYuvList();
	}

	ISPLogd("List(%d) current node num:%d", mId, mNodeNum);

	if (SUCCESS(result)) {
		StateTransform(STATE_TRANS_FORWARD);
	}

	return result;
}

template<typename T1, typename T2, typename T3>
ISPResult ISPList<T1, T2, T3>::CreateNecList()
{
	ISPResult result = ISP_SUCCESS;
	int32_t nodePropertyIndex = 0;

	if (mState != ISP_LIST_CONFIGED) {
		result = ISP_STATE_ERROR;
		ISPLoge("func called in invaled state:%d %d", mState, result);
	}

	if (mRawHead || mRgbHead || mYuvHead)
	{
		result = ISP_STATE_ERROR;
		ISPLoge("List(%d) create new list failed! Old list exits! result:%d", mId, result);
	}

	//Head node create
	if (SUCCESS(result)) {
		FindNodePropertyIndex(PROCESS_HEAD, &nodePropertyIndex);
		mRawHead = new ISPNecNode<T1, T1>;
		if (mRawHead) {
			mRawHead->Init(&mProperty.pNodeConfig[nodePropertyIndex], pRawBuffer, pRawBuffer);
			mNodeNum++;
		}
		else {
			result = ISP_MEMORY_ERROR;
			ISPLoge("New node failed! %d", result);
		}
	}

	//Raw -> Rgb node create
	if (SUCCESS(result)) {
		FindNodePropertyIndex(PROCESS_CST_RAW2RGB, &nodePropertyIndex);
		mRgbHead = new ISPNecNode<T1, T2>;
		if (mRgbHead) {
			mRgbHead->Init(&mProperty.pNodeConfig[nodePropertyIndex], pRawBuffer, pRgbBuffer);
			mNodeNum++;
		}
		else {
			result = ISP_MEMORY_ERROR;
			ISPLoge("New node failed! %d", result);
		}
	}

	//Rgb -> YUV node create
	if (SUCCESS(result)) {
		FindNodePropertyIndex(PROCESS_CST_RGB2YUV, &nodePropertyIndex);
		mYuvHead = new ISPNecNode<T2, T3>;
		if (mYuvHead) {
			mYuvHead->Init(&mProperty.pNodeConfig[nodePropertyIndex], pRgbBuffer, pYuvBuffer);
			mNodeNum++;
		}
		else {
			result = ISP_MEMORY_ERROR;
			ISPLoge("New node failed! %d", result);
		}
	}

	ISPLogd("List(%d) current node num:%d", mId, mNodeNum);

	return result;
}

template<typename T1, typename T2, typename T3>
ISPResult ISPList<T1, T2, T3>::CreateRawList()
{
	ISPResult result = ISP_SUCCESS;

	int32_t nodePropertyIndex = 0;
	ISPNode<T1, T1>* pNewNode = nullptr;
	PROCESS_TYPE newNodeType = PROCESS_TYPE_NUM;

	if (mState != ISP_LIST_CONFIGED) {
		result = ISP_STATE_ERROR;
		ISPLoge("func called in invaled state:%d %d", mState, result);
	}

	if (SUCCESS(result)) {
		for (int32_t i = 0; i < sizeof(RawNodeConfigure) / sizeof(PROCESS_TYPE); i++) {
			newNodeType = RawNodeConfigure[i];
			pNewNode = new ISPNode<T1, T1>;
			if (pNewNode) {
				FindNodePropertyIndex(newNodeType, &nodePropertyIndex);
				result = pNewNode->Init(&mProperty.pNodeConfig[nodePropertyIndex], pRawBuffer, pRawBuffer);
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
	}

	if (SUCCESS(result)) {
		char name[NODE_NAME_MAX_SZIE] = { '0' };
		ISPNode<T1, T1>* pNode = mRawHead;
		while (pNode) {
			pNode->GetNodeName(name);
			ISPLogd("List(%d) node:%s", mId, name);
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
	
	if (mState != ISP_LIST_CONFIGED) {
		result = ISP_STATE_ERROR;
		ISPLoge("func called in invaled state:%d %d", mState, result);
	}

	if (SUCCESS(result)) {
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
	}

	return result;
}

template<typename T1, typename T2, typename T3>
ISPResult ISPList<T1, T2, T3>::CreateRgbList()
{
	ISPResult result = ISP_SUCCESS;

	int32_t nodePropertyIndex = 0;
	ISPNode<T2, T2>* pNewNode = nullptr;
	PROCESS_TYPE newNodeType = PROCESS_TYPE_NUM;

	if (mState != ISP_LIST_CONFIGED) {
		result = ISP_STATE_ERROR;
		ISPLoge("func called in invaled state:%d %d", mState, result);
	}

	if (SUCCESS(result)) {
		for (int32_t i = 0; i < sizeof(RgbNodeConfigure) / sizeof(PROCESS_TYPE); i++) {
			newNodeType = RgbNodeConfigure[i];
			pNewNode = new ISPNode<T2, T2>;
			if (pNewNode) {
				FindNodePropertyIndex(newNodeType, &nodePropertyIndex);
				result = pNewNode->Init(&mProperty.pNodeConfig[nodePropertyIndex], pRgbBuffer, pRgbBuffer);
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
	}

	if (SUCCESS(result)) {
		char name[NODE_NAME_MAX_SZIE] = { '0' };
		mRgbHead->GetNodeName(name);
		ISPLogd("List(%d) node:%s", mId, name);
		ISPNode<T2, T2>* pNode = mRgbHead->pNext;
		while (pNode) {
			pNode->GetNodeName(name);
			ISPLogd("List(%d) node:%s", mId, name);
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
	
	if (mState != ISP_LIST_CONFIGED) {
		result = ISP_STATE_ERROR;
		ISPLoge("func called in invaled state:%d %d", mState, result);
	}

	if (SUCCESS(result)) {
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
	}

	return result;
}

template<typename T1, typename T2, typename T3>
ISPResult ISPList<T1, T2, T3>::CreateYuvList()
{
	ISPResult result = ISP_SUCCESS;

	int32_t nodePropertyIndex = 0;
	ISPNode<T3, T3>* pNewNode = nullptr;
	PROCESS_TYPE newNodeType = PROCESS_TYPE_NUM;

	if (mState != ISP_LIST_CONFIGED) {
		result = ISP_STATE_ERROR;
		ISPLoge("func called in invaled state:%d %d", mState, result);
	}

	if (SUCCESS(result)) {
		for (int32_t i = 0; i < sizeof(YuvNodeConfigure) / sizeof(PROCESS_TYPE); i++) {
			newNodeType = YuvNodeConfigure[i];
			pNewNode = new ISPNode<T3, T3>;
			if (pNewNode) {
				FindNodePropertyIndex(newNodeType, &nodePropertyIndex);
				result = pNewNode->Init(&mProperty.pNodeConfig[nodePropertyIndex], pYuvBuffer, pYuvBuffer);
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
	}

	if (SUCCESS(result)) {
		char name[NODE_NAME_MAX_SZIE] = { '0' };
		mYuvHead->GetNodeName(name);
		ISPLogd("List(%d) node:%s", mId, name);
		ISPNode<T3, T3>* pNode = mYuvHead->pNext;
		while (pNode) {
			pNode->GetNodeName(name);
			ISPLogd("List(%d) node:%s", mId, name);
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

	if (mState != ISP_LIST_CONFIGED) {
		result = ISP_STATE_ERROR;
		ISPLoge("func called in invaled state:%d %d", mState, result);
	}

	if (SUCCESS(result)) {
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
	}

	return result;
}

template<typename T1, typename T2, typename T3>
ISPResult ISPList<T1, T2, T3>::Process() 
{
	ISPResult result = ISP_SUCCESS;
	ISPNode<T1, T1>* pNode = mRawHead;
	
	if (mState != ISP_LIST_CONSTRUCTED) {
		result = ISP_STATE_ERROR;
		ISPLoge("func called in invaled state:%d %d", mState, result);
	}

	if (SUCCESS(result)) {
		StateTransform(STATE_TRANS_FORWARD);
	}

	if (SUCCESS(result)) {
		while (pNode) {
			result = pNode->Process(pParamManager);
			if (SUCCESS(result)) {
				pNode = pNode->pNext;
			}
			else {
				char name[NODE_NAME_MAX_SZIE] = { '\0' };
				pNode->GetNodeName(name);
				ISPLoge("List(%d) %s node process failed! result:%d", mId, name, result);
				break;
			}
		}
	}

	if (SUCCESS(result)) {
		ISPLogd(">>>>> List(%d) Raw process finished! >>>>>", mId);
		TriggerRgbProcess();
	}

	return result;
}

template<typename T1, typename T2, typename T3>
ISPResult ISPList<T1, T2, T3>::TriggerRgbProcess()
{
	ISPResult result = ISP_SUCCESS;

	if (mState != ISP_LIST_RUNNING) {
		result = ISP_STATE_ERROR;
		ISPLoge("func called in invaled state:%d %d", mState, result);
	}

	if (SUCCESS(result)) {
		if (mState != ISP_LIST_RUNNING) {
			result = ISP_STATE_ERROR;
			ISPLoge("func called in invaled state:%d %d", mState, result);
		}
	}

	if (SUCCESS(result)) {
		result = RgbProcess();
	}

	return result;
}

template<typename T1, typename T2, typename T3>
ISPResult ISPList<T1, T2, T3>::RgbProcess()
{
	ISPResult result = ISP_SUCCESS;

	if (mState != ISP_LIST_RUNNING) {
		result = ISP_STATE_ERROR;
		ISPLoge("func called in invaled state:%d %d", mState, result);
	}

	if (SUCCESS(result)) {
		result = mRgbHead->Process(pParamManager);
	}

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
				ISPLoge("List(%d) %s node process failed! result:%d", mId, name, result);
				break;
			}
		}
	}

	if (SUCCESS(result)) {
		ISPLogd(">>>>> List(%d) Rgb process finished! >>>>>", mId);
		TriggerYuvProcess();
	}

	return result;
}

template<typename T1, typename T2, typename T3>
ISPResult ISPList<T1, T2, T3>::TriggerYuvProcess()
{
	ISPResult result = ISP_SUCCESS;

	if (mState != ISP_LIST_RUNNING) {
		result = ISP_STATE_ERROR;
		ISPLoge("func called in invaled state:%d %d", mState, result);
	}

	if (SUCCESS(result)) {
		if (mState != ISP_LIST_RUNNING) {
			result = ISP_STATE_ERROR;
			ISPLoge("func called in invaled state:%d %d", mState, result);
		}
	}

	if (SUCCESS(result)) {
		result = YuvProcess();
	}

	return result;
}

template<typename T1, typename T2, typename T3>
ISPResult ISPList<T1, T2, T3>::YuvProcess() 
{
	ISPResult result = ISP_SUCCESS;

	if (mState != ISP_LIST_RUNNING) {
		result = ISP_STATE_ERROR;
		ISPLoge("func called in invaled state:%d %d", mState, result);
	}

	if (SUCCESS(result)) {
		result = mYuvHead->Process(pParamManager);
	}

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
				ISPLoge("List(%d) %s node process failed! result:%d", mId, name, result);
				break;
			}
		}
	}

	if (SUCCESS(result)) {
		ISPLogd(">>>>> List(%d) Yuv process finished! >>>>>", mId);
		StateTransform(STATE_TRANS_FORWARD);
	}

	return result;
}

template<typename T1, typename T2, typename T3>
ISPResult ISPList<T1, T2, T3>::EnableNodebyType(PROCESS_TYPE type)
{
	ISPResult result = ISP_SUCCESS;
	LIST_TYPE listType = RAW_LIST;
	bool needFind = true;

	if (mState != ISP_LIST_CONSTRUCTED) {
		result = ISP_STATE_ERROR;
		ISPLoge("func called in invaled state:%d %d", mState, result);
	}

	if (SUCCESS(result)) {
		if (type < PROCESS_CST_RAW2RGB)
		{
			ISPNode<T1, T1>* pRawNode = mRawHead->pNext;
			while (pRawNode) {
				if (type == pRawNode->GetProperty().type)
				{
					pRawNode->Enable();
					needFind = false;
					break;
				}
				pRawNode = pRawNode->pNext;
			}

			if (needFind) {
				ISPNode<T2, T2>* pRgbNode = mRgbHead->pNext;
				while (pRgbNode) {
					if (type == pRgbNode->GetProperty().type)
					{
						pRgbNode->Enable();
						needFind = false;
						break;
					}
					pRgbNode = pRgbNode->pNext;
				}
			}

			if (needFind) {
				ISPNode<T3, T3>* pYuvNode = mYuvHead->pNext;
				while (pYuvNode) {
					if (type == pYuvNode->GetProperty().type)
					{
						pYuvNode->Enable();
						needFind = false;
						break;
					}
					pYuvNode = pYuvNode->pNext;
				}
			}
		}
		else {
			result = EnableNecNodebyType(type);
		}
	}
	
	return result;
}

template<typename T1, typename T2, typename T3>
ISPResult ISPList<T1, T2, T3>::DisableNodebyType(PROCESS_TYPE type)
{
	ISPResult result = ISP_SUCCESS;
	LIST_TYPE listType = RAW_LIST;
	bool needFind = true;

	if (mState != ISP_LIST_CONSTRUCTED) {
		result = ISP_STATE_ERROR;
		ISPLoge("func called in invaled state:%d %d", mState, result);
	}

	if (SUCCESS(result)) {
		if (type < PROCESS_CST_RAW2RGB)
		{
			ISPNode<T1, T1>* pRawNode = mRawHead->pNext;
			while (pRawNode) {
				if (type == pRawNode->GetProperty().type)
				{
					pRawNode->Disable();
					needFind = false;
					break;
				}
				pRawNode = pRawNode->pNext;
			}

			if (needFind) {
				ISPNode<T2, T2>* pRgbNode = mRgbHead->pNext;
				while (pRgbNode) {
					if (type == pRgbNode->GetProperty().type)
					{
						pRgbNode->Disable();
						needFind = false;
						break;
					}
					pRgbNode = pRgbNode->pNext;
				}
			}

			if (needFind) {
				ISPNode<T3, T3>* pYuvNode = mYuvHead->pNext;
				while (pYuvNode) {
					if (type == pYuvNode->GetProperty().type)
					{
						pYuvNode->Disable();
						needFind = false;
						break;
					}
					pYuvNode = pYuvNode->pNext;
				}
			}
		}
		else {
			result = DisableNecNodebyType(type);
		}
	}

	return result;
}

template<typename T1, typename T2, typename T3>
ISPResult ISPList<T1, T2, T3>::EnableNecNodebyType(PROCESS_TYPE type)
{
	ISPResult result = ISP_SUCCESS;

	if (mState != ISP_LIST_CONSTRUCTED) {
		result = ISP_STATE_ERROR;
		ISPLoge("func called in invaled state:%d %d", mState, result);
	}

	if (SUCCESS(result)) {
		if (type == PROCESS_HEAD) {
			result = mRawHead->Enable();
		}
		else if (type == PROCESS_CST_RAW2RGB) {
			result = mRgbHead->Enable();
		}
		else if (type == PROCESS_CST_RGB2YUV) {
			result = mYuvHead->Enable();
		}
	}

	return result;
}

template<typename T1, typename T2, typename T3>
ISPResult ISPList<T1, T2, T3>::DisableNecNodebyType(PROCESS_TYPE type)
{
	ISPResult result = ISP_SUCCESS;

	if (mState != ISP_LIST_CONSTRUCTED) {
		result = ISP_STATE_ERROR;
		ISPLoge("func called in invaled state:%d %d", mState, result);
	}

	if (SUCCESS(result)) {
		if (type == PROCESS_HEAD) {
			result = mRawHead->Disable();
		}
		else if (type == PROCESS_CST_RAW2RGB) {
			result = mRgbHead->Disable();
		}
		else if (type == PROCESS_CST_RGB2YUV) {
			result = mYuvHead->Disable();
		}
	}

	return result;
}