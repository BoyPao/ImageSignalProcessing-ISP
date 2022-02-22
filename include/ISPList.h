// License: GPL-3.0-or-later
/*
 * ISPList is a serial abstract object helps to construct processing
 * steps more flexabl.
 *
 * Copyright (c) 2019 Peng Hao <635945005@qq.com>
 */

#pragma once

#include "InterfaceWrapper.h"
#include "ParamManager.h"

#define NODE_NAME_MAX_SZIE 15

/* NODE */
enum PROCESS_TYPE {
	PROCESS_BLC = 0,
	PROCESS_LSC,
	PROCESS_Demosaic,
	PROCESS_WB,
	PROCESS_CC,
	PROCESS_GAMMA,
	PROCESS_WNR,
	PROCESS_EE,
	PROCESS_TYPE_NUM
};

enum NODE_SWITCH {
	NODE_OFF = 0,
	NODE_ON
};

struct ISP_NODE_PROPERTY {
	char name[NODE_NAME_MAX_SZIE];
	PROCESS_TYPE type;
	NODE_SWITCH enable;
};

template<typename T1, typename T2>
class ISPNode {
public:
	ISPNode();
	virtual ~ISPNode();
	virtual ISPResult GetNodeName(char* name);
	virtual ISPResult Init(ISP_NODE_PROPERTY* cfg, T1* input, T2* output);
	virtual ISPResult Process(void* pItf);
	virtual ISPResult Enable();
	virtual ISPResult Disable();
	virtual bool isOn();
	virtual ISP_NODE_PROPERTY GetProperty();

	ISPNode<T2,T2>* pNext;

protected:
	bool mInited;
	T1* pInputBuffer;
	T2* pOutputBuffer;

private:
	ISP_NODE_PROPERTY mProperty;
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
	ISPResult rt = ISP_SUCCESS;

	if (!input || !output || !cfg) {
		rt = ISP_INVALID_PARAM;
	}

	if (SUCCESS(rt)) {
		memcpy(&mProperty, cfg, sizeof(ISP_NODE_PROPERTY));
	}

	if (SUCCESS(rt)) {
		pInputBuffer = input;
		pOutputBuffer = output;
	}

	if (SUCCESS(rt)) {
		mInited = true;
	}

	return rt;
}

template<typename T1, typename T2>
ISPResult ISPNode<T1, T2>::Enable()
{
	ISPResult rt = ISP_SUCCESS;

	mProperty.enable = NODE_ON;

	return rt;
}

template<typename T1, typename T2>
ISPResult ISPNode<T1, T2>::Disable()
{
	ISPResult rt = ISP_SUCCESS;

	mProperty.enable = NODE_OFF;

	return rt;
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
ISPResult ISPNode<T1, T2>::Process(void* pItf)
{
	ISPResult rt = ISP_SUCCESS;
	InterfaceWrapper* pIW = nullptr;
	char name[NODE_NAME_MAX_SZIE];
	GetNodeName(name);

	if (!mInited) {
		rt = ISP_STATE_ERROR;
		ILOGE("Node is not inited!");
	}

	if (SUCCESS(rt)) {
		pIW = static_cast<InterfaceWrapper*>(pItf);
		if (!pIW) {
			rt = ISP_INVALID_PARAM;
			ILOGE("Invalid input!");
		}
	}

	if (SUCCESS(rt)) {
		ILOGDL("%s:Buffer(in:%p out:%p)", name, pInputBuffer, pOutputBuffer);
		switch (mProperty.type) {
		case PROCESS_BLC:
			rt = pIW->AlgProcess(ALG_CMD_BLC ,pInputBuffer, mProperty.enable);
			break;
		case PROCESS_LSC:
			rt = pIW->AlgProcess(ALG_CMD_LSC ,pInputBuffer, mProperty.enable);
			break;
		case PROCESS_Demosaic:
			rt = pIW->AlgProcess(ALG_CMD_DEMOSAIC ,pInputBuffer, mProperty.enable);
			break;
		case PROCESS_WB:
			rt = pIW->AlgProcess(ALG_CMD_WB ,pInputBuffer, mProperty.enable);
			break;
		case PROCESS_CC:
			rt = pIW->AlgProcess(ALG_CMD_CC ,pInputBuffer, mProperty.enable);
			break;
		case PROCESS_GAMMA:
			rt = pIW->AlgProcess(ALG_CMD_GAMMA ,pInputBuffer, mProperty.enable);
			break;
		case PROCESS_WNR:
			rt = pIW->AlgProcess(ALG_CMD_WNR ,pInputBuffer, mProperty.enable);
			break;
		case PROCESS_EE:
			rt = pIW->AlgProcess(ALG_CMD_EE ,pInputBuffer, mProperty.enable);
			break;
		case PROCESS_TYPE_NUM:
		default:
			rt = ISP_FAILED;
			break;
		}
	}

	if (SUCCESS(rt)) {
		if (mProperty.enable) {
			ILOGI("%s:Process finished.", name);
		} else {
			ILOGI("%s:Skiped.", name);
		}
	}

	return rt;
}

/* NEC NODE */
enum NEC_PROCESS_TYPE {
	NEC_PROCESS_HEAD = PROCESS_TYPE_NUM + 1,
	NEC_PROCESS_CST_RAW2RGB,
	NEC_PROCESS_CST_RGB2YUV,
	NEC_PROCESS_CST_YUV2RGB,
	NEC_PROCESS_TYPE_NUM
};

struct ISP_NECNODE_PROPERTY {
	char name[NODE_NAME_MAX_SZIE];
	NEC_PROCESS_TYPE type;
	NODE_SWITCH enable;
};

template<typename T1, typename T2>
class ISPNecNode : public ISPNode<T1, T2> {
public:
	ISPNecNode();
	~ISPNecNode();
	ISPResult Init(ISP_NECNODE_PROPERTY* cfg, T1* input, T2* output);
	ISPResult GetNodeName(char* name);
	ISPResult Process(void* pItf);
	ISPResult Disable();
private:
	ISP_NECNODE_PROPERTY mProperty;
};

template<typename T1, typename T2>
ISPNecNode<T1, T2>::ISPNecNode()
{
	memset(&mProperty, 0, sizeof(ISP_NODE_PROPERTY));
}

template<typename T1, typename T2>
ISPNecNode<T1, T2>::~ISPNecNode()
{
	memset(&mProperty, 0, sizeof(ISP_NODE_PROPERTY));
}

template<typename T1, typename T2>
ISPResult ISPNecNode<T1, T2>::Init(ISP_NECNODE_PROPERTY *cfg, T1* input, T2* output)
{
	ISPResult rt = ISP_SUCCESS;

	if (!input || !output || !cfg) {
		rt = ISP_INVALID_PARAM;
	}

	if (SUCCESS(rt)) {
		memcpy(&mProperty, cfg, sizeof(ISP_NODE_PROPERTY));
	}

	if (SUCCESS(rt)) {
		this->pInputBuffer = input;
		this->pOutputBuffer = output;
	}

	if (SUCCESS(rt)) {
		this->mInited = true;
	}

	return rt;
}

template<typename T1, typename T2>
ISPResult ISPNecNode<T1, T2>::GetNodeName(char* name)
{
	ISPResult rt = ISP_SUCCESS;
	if (this->mInited) {
		memcpy(name, mProperty.name, sizeof(char) * NODE_NAME_MAX_SZIE);
	}
	else {
		rt = ISP_STATE_ERROR;
	}
	return rt;
}

template<typename T1, typename T2>
ISPResult ISPNecNode<T1, T2>::Process(void* pItf)
{
	ISPResult rt = ISP_SUCCESS;
	InterfaceWrapper* pIW = nullptr;
	char name[NODE_NAME_MAX_SZIE];
	GetNodeName(name);

	if (!this->mInited) {
		rt = ISP_STATE_ERROR;
		ILOGE("Node is not inited!");
	}

	if (SUCCESS(rt)) {
		pIW = static_cast<InterfaceWrapper*>(pItf);
		if (!pIW) {
			rt = ISP_INVALID_PARAM;
			ILOGE("Invalid input!");
		}
	}

	if (SUCCESS(rt)) {
		ILOGDL("%s:Buffer(in:%p out:%p)", name, this->pInputBuffer, this->pOutputBuffer);
		switch (mProperty.type) {
		case NEC_PROCESS_HEAD:
			// currentlly nothing to be done here.
			break;
		case NEC_PROCESS_CST_RAW2RGB:
			rt = pIW->AlgProcess(ALG_CMD_CTS_RAW2RGB, this->pInputBuffer, this->pOutputBuffer, mProperty.enable);
			break;
		case NEC_PROCESS_CST_RGB2YUV:
			rt = pIW->AlgProcess(ALG_CMD_CTS_RGB2YUV, this->pInputBuffer, this->pOutputBuffer, mProperty.enable);
			break;
		case NEC_PROCESS_CST_YUV2RGB:
			rt = pIW->AlgProcess(ALG_CMD_CTS_YUV2RGB, this->pInputBuffer, this->pOutputBuffer, mProperty.enable);
			break;
		case NEC_PROCESS_TYPE_NUM:
		default:
			rt = ISP_FAILED;
			break;
		}
	}

	if (SUCCESS(rt)) {
		if (mProperty.enable) {
			ILOGI("%s:Process finished.", name);
		} else {
			ILOGI("%s:Skiped.", name);
		}
	}

	return rt;
}

template<typename T1, typename T2>
ISPResult ISPNecNode<T1, T2>::Disable()
{
	ISPResult rt = ISP_SUCCESS;

	char name[NODE_NAME_MAX_SZIE];
	this->GetNodeName(name);
	ILOGW("Try to disable necessary node:%s", name);
	mProperty.enable = NODE_OFF;

	return rt;
}

/* Node List */
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

struct ISP_LIST_PROPERTY {
	ISP_NODE_PROPERTY NodeProperty[PROCESS_TYPE_NUM];
};

struct ISP_LISTHEAD_PROPERTY {
	ISP_NECNODE_PROPERTY NecNodeProperty[NEC_PROCESS_TYPE_NUM - NEC_PROCESS_HEAD];
};

static ISP_LISTHEAD_PROPERTY gListHeadsConfigure = {
	{{"HEAD",			NEC_PROCESS_HEAD,			NODE_ON},
	 {"CST_RAW2RGB",	NEC_PROCESS_CST_RAW2RGB,	NODE_ON},
	 {"CST_RGB2YUV",	NEC_PROCESS_CST_RGB2YUV,	NODE_ON}, /* support off this node for test*/
	 {"CST_YUV2RGB",	NEC_PROCESS_CST_YUV2RGB,	NODE_ON}}
};

const PROCESS_TYPE RawListConfigure[] = {
	PROCESS_BLC,
	PROCESS_LSC,
};

const PROCESS_TYPE RgbListConfigure[] = {
	PROCESS_Demosaic,
	PROCESS_WB,
	PROCESS_CC,
	PROCESS_GAMMA,
};

const PROCESS_TYPE YuvListConfigure[] = {
	PROCESS_WNR,
	PROCESS_EE,
};

const PROCESS_TYPE PostListConfigure[] = {
	PROCESS_TYPE_NUM
};


template<typename T1, typename T2, typename T3, typename T4>
class ISPList {
public:
	ISPList(int32_t id);
	~ISPList();
	ISPResult Init(T1* pRawBuf, T2* pRgbBuf, T3* pYuvBuf, T4* pPostBuf, void* pPM);
	ISPResult SetListConfig(ISP_LIST_PROPERTY* pCfg);
	ISPResult FindNodePropertyIndex(PROCESS_TYPE type, int32_t* index);
	ISPResult FindNecNodePropertyIndex(NEC_PROCESS_TYPE type, int32_t* index);
	ISPResult CreatISPList();
	ISPResult AddNode(PROCESS_TYPE type);
	int32_t GetNodeNum();
	ISPResult Process();
	ISPResult EnableNodebyType(int32_t type);
	ISPResult DisableNodebyType(int32_t type);
	ISPResult EnableNecNodebyType(int32_t type);
	ISPResult DisableNecNodebyType(int32_t type);

private:
	ISPResult CreateNecList();
	ISPResult CreateRawList();
	ISPResult CreateRgbList();
	ISPResult CreateYuvList();
	ISPResult CreatePostList();
	ISPResult AddNodeToRawTail(ISPNode<T1, T1>* pNode);
	ISPResult AddNodeToRgbTail(ISPNode<T2, T2>* pNode);
	ISPResult AddNodeToYuvTail(ISPNode<T3, T3>* pNode);
	ISPResult AddNodeToPostTail(ISPNode<T4, T4>* pNode);
	ISPResult TriggerRgbProcess();
	ISPResult RgbProcess();
	ISPResult TriggerYuvProcess();
	ISPResult YuvProcess();
	ISPResult TriggerPostProcess();
	ISPResult PostProcess();
	ISPResult StateTransform(STATE_TRANS_ORIENTATION orientation);

	int32_t mId;
	ISPNecNode<T1, T1>* mRawHead;
	ISPNecNode<T1, T2>* mRgbHead;
	ISPNecNode<T2, T3>* mYuvHead;
	ISPNecNode<T3, T4>* mPostHead;
	void* pItfWrapper;
	int32_t mNodeNum;
	T1* pRawBuffer;
	T2* pRgbBuffer;
	T3* pYuvBuffer;
	T4* pPostBuffer;
	ISP_LIST_PROPERTY mProperty;
	ISP_LIST_STATE mState;
};

template<typename T1, typename T2, typename T3, typename T4>
ISPList<T1, T2, T3, T4>::ISPList(int32_t id) :
	mRawHead(nullptr),
	mRgbHead(nullptr),
	mYuvHead(nullptr),
	mPostHead(nullptr),
	pItfWrapper(nullptr),
	mNodeNum(0),
	mState(ISP_LIST_NEW)
{
	mId = id;
}

template<typename T1, typename T2, typename T3, typename T4>
ISPList<T1, T2, T3, T4>::~ISPList()
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

	if (mPostHead) {
		ISPNode<T4, T4>* pPostNext = mPostHead->pNext;
		while (pPostNext != nullptr) {
			mPostHead->pNext = pPostNext->pNext;
			delete pPostNext;
			mNodeNum--;
			pPostNext = mPostHead->pNext;
		}
		delete mPostHead;
		mPostHead = nullptr;
		mNodeNum--;
	}

	pItfWrapper = nullptr;

	ILOGDL("List(%d) Node num:%d", mId, mNodeNum);
}

template<typename T1, typename T2, typename T3, typename T4>
ISPResult ISPList<T1, T2, T3, T4>::StateTransform(STATE_TRANS_ORIENTATION orientation)
{
	ISPResult rt = ISP_SUCCESS;
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
			ILOGW("Undefined forward state transform! state:%d", currentState);
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
			ILOGW("Undefined backward state transform! state:%d", currentState);
			break;
		}
	}
	else if (orientation != STATE_TRANS_TO_SELF) {
		rt = ISP_INVALID_PARAM;
		ILOGE("Invaled orientation:%d %d", orientation, rt);
	}

	if (SUCCESS(rt)) {
		ILOGDL("State: %d -> %d", currentState, mState);
	}

	return rt;
}

template<typename T1, typename T2, typename T3, typename T4>
ISPResult ISPList<T1, T2, T3, T4>::Init(T1* pRawBuf, T2* pRgbBuf, T3* pYuvBuf, T4* pPostBuf, void* pIW)
{
	ISPResult rt = ISP_SUCCESS;

	if (mState != ISP_LIST_NEW) {
		rt = ISP_STATE_ERROR;
		ILOGE("func called in invaled state:%d %d", mState, rt);
	}

	if (!pRawBuf || !pRgbBuf || !pYuvBuf || !pPostBuf || !pIW) {
		rt = ISP_INVALID_PARAM;
		ILOGE("Failed to init list(%d), pBuffer or PM is null! rt:%d", mId, rt);
	}

	if (SUCCESS(rt)) {
		pRawBuffer = pRawBuf;
		pRgbBuffer = pRgbBuf;
		pYuvBuffer = pYuvBuf;
		pPostBuffer = pPostBuf;
	}

	if(SUCCESS(rt)){
		pItfWrapper = pIW;
	}

	if (SUCCESS(rt)) {
		rt = StateTransform(STATE_TRANS_FORWARD);
	}

	return rt;
}

template<typename T1, typename T2, typename T3, typename T4>
ISPResult ISPList<T1, T2, T3, T4>::SetListConfig(ISP_LIST_PROPERTY* pCfg)
{
	ISPResult rt = ISP_SUCCESS;

	if (mState != ISP_LIST_INITED) {
		rt = ISP_STATE_ERROR;
		ILOGE("func called in invaled state:%d %d", mState, rt);
	}

	if (SUCCESS(rt)) {
		if (pCfg) {
			memcpy(&mProperty, pCfg, sizeof(ISP_LIST_PROPERTY));
		}
		else {
			rt = ISP_INVALID_PARAM;
			ILOGE("pCfg is null! %d", rt);
		}
	}

	if (SUCCESS(rt)) {
		rt = StateTransform(STATE_TRANS_FORWARD);
	}

	return rt;
}

template<typename T1, typename T2, typename T3, typename T4>
ISPResult ISPList<T1, T2, T3, T4>::FindNodePropertyIndex(PROCESS_TYPE type, int32_t* index)
{
	ISPResult rt = ISP_SUCCESS;

	if (index) {
		for (int32_t i = 0; i < PROCESS_TYPE_NUM; i++)
		{
			if (type == mProperty.NodeProperty[i].type)
			{
				*index = i;
				break;
			}
		}
	}
	else {
		rt = ISP_INVALID_PARAM;
		ILOGE("input is null! %d", rt);
	}

	return rt;
}

template<typename T1, typename T2, typename T3, typename T4>
ISPResult ISPList<T1, T2, T3, T4>::FindNecNodePropertyIndex(NEC_PROCESS_TYPE type, int32_t* index)
{
	ISPResult rt = ISP_SUCCESS;

	if (index) {
		for (int32_t i = 0; i < NEC_PROCESS_TYPE_NUM - NEC_PROCESS_HEAD; i++)
		{
			if (type == gListHeadsConfigure.NecNodeProperty[i].type)
			{
				*index = i;
				break;
			}
		}
	}
	else {
		rt = ISP_INVALID_PARAM;
		ILOGE("input is null! %d", rt);
	}

	return rt;
}

template<typename T1, typename T2, typename T3, typename T4>
ISPResult ISPList<T1, T2, T3, T4>::CreatISPList()
{
	ISPResult rt = ISP_SUCCESS;

	if (mState != ISP_LIST_CONFIGED) {
		rt = ISP_STATE_ERROR;
		ILOGE("func called in invaled state:%d %d", mState, rt);
	}

	if (SUCCESS(rt)) {
		rt = CreateNecList();
	}

	if (SUCCESS(rt)) {
		rt = CreateRawList();
	}

	if (SUCCESS(rt)) {
		rt = CreateRgbList();
	}

	if (SUCCESS(rt)) {
		rt = CreateYuvList();
	}

	if (SUCCESS(rt)) {
		rt = CreatePostList();
	}

	ILOGDL("List(%d) current node num:%d", mId, mNodeNum);

	if (SUCCESS(rt)) {
		rt = StateTransform(STATE_TRANS_FORWARD);
	}

	return rt;
}

template<typename T1, typename T2, typename T3, typename T4>
ISPResult ISPList<T1, T2, T3, T4>::CreateNecList()
{
	ISPResult rt = ISP_SUCCESS;
	int32_t nodePropertyIndex = 0;

	if (mState != ISP_LIST_CONFIGED) {
		rt = ISP_STATE_ERROR;
		ILOGE("func called in invaled state:%d %d", mState, rt);
	}

	if (mRawHead || mRgbHead || mYuvHead || mPostHead)
	{
		rt = ISP_STATE_ERROR;
		ILOGE("List(%d) create new list failed! Old head exits! rt:%d", mId, rt);
	}

	//Head node create
	if (SUCCESS(rt)) {
		FindNecNodePropertyIndex(NEC_PROCESS_HEAD, &nodePropertyIndex);
		mRawHead = new ISPNecNode<T1, T1>;
		if (mRawHead) {
			mRawHead->Init(&gListHeadsConfigure.NecNodeProperty[nodePropertyIndex], pRawBuffer, pRawBuffer);
			mNodeNum++;
		}
		else {
			rt = ISP_MEMORY_ERROR;
			ILOGE("New node failed! %d", rt);
		}
	}

	//Raw -> Rgb node create
	if (SUCCESS(rt)) {
		FindNecNodePropertyIndex(NEC_PROCESS_CST_RAW2RGB, &nodePropertyIndex);
		mRgbHead = new ISPNecNode<T1, T2>;
		if (mRgbHead) {
			mRgbHead->Init(&gListHeadsConfigure.NecNodeProperty[nodePropertyIndex], pRawBuffer, pRgbBuffer);
			mNodeNum++;
		}
		else {
			rt = ISP_MEMORY_ERROR;
			ILOGE("New node failed! %d", rt);
		}
	}

	//Rgb -> YUV node create
	if (SUCCESS(rt)) {
		FindNecNodePropertyIndex(NEC_PROCESS_CST_RGB2YUV, &nodePropertyIndex);
		mYuvHead = new ISPNecNode<T2, T3>;
		if (mYuvHead) {
			mYuvHead->Init(&gListHeadsConfigure.NecNodeProperty[nodePropertyIndex], pRgbBuffer, pYuvBuffer);
			mNodeNum++;
		}
		else {
			rt = ISP_MEMORY_ERROR;
			ILOGE("New node failed! %d", rt);
		}
	}

	//YUV -> Post node create
	if (SUCCESS(rt)) {
		FindNecNodePropertyIndex(NEC_PROCESS_CST_YUV2RGB, &nodePropertyIndex);
		mPostHead = new ISPNecNode<T3, T4>;
		if (mPostHead) {
			mPostHead->Init(&gListHeadsConfigure.NecNodeProperty[nodePropertyIndex], pYuvBuffer, pPostBuffer);
			mNodeNum++;
		}
		else {
			rt = ISP_MEMORY_ERROR;
			ILOGE("New node failed! %d", rt);
		}
	}

	ILOGDL("List(%d) current node num:%d", mId, mNodeNum);

	return rt;
}

template<typename T1, typename T2, typename T3, typename T4>
ISPResult ISPList<T1, T2, T3, T4>::CreateRawList()
{
	ISPResult rt = ISP_SUCCESS;

	int32_t nodePropertyIndex = -1;
	ISPNode<T1, T1>* pNewNode = nullptr;
	PROCESS_TYPE newNodeType = PROCESS_TYPE_NUM;

	if (mState != ISP_LIST_CONFIGED) {
		rt = ISP_STATE_ERROR;
		ILOGE("func called in invaled state:%d %d", mState, rt);
	}

	if (SUCCESS(rt)) {
		for (int32_t i = 0; i < (int32_t)(sizeof(RawListConfigure) / sizeof(PROCESS_TYPE)); i++) {
			newNodeType = RawListConfigure[i];
			FindNodePropertyIndex(newNodeType, &nodePropertyIndex);
			if (nodePropertyIndex >= 0) {
				pNewNode = new ISPNode<T1, T1>;
				if (pNewNode) {
					rt = pNewNode->Init(&mProperty.NodeProperty[nodePropertyIndex], pRawBuffer, pRawBuffer);
					if (SUCCESS(rt)) {
						rt = AddNodeToRawTail(pNewNode);
						if (!SUCCESS(rt)) {
							break;
						}
					}
					else {
						break;
					}
				}
				else {
					rt = ISP_MEMORY_ERROR;
					ILOGE("Failed to new node! rt:%d", rt);
					break;
				}
			}
			else {
				ILOGW("Not find node type:%d", newNodeType);
			}
		}
	}

	if (SUCCESS(rt)) {
		char name[NODE_NAME_MAX_SZIE] = { '0' };
		ISPNode<T1, T1>* pNode = mRawHead;
		while (pNode) {
			pNode->GetNodeName(name);
			ILOGDL("List(%d) node:%s", mId, name);
			pNode = pNode->pNext;
		}
	}

	return rt;
}

template<typename T1, typename T2, typename T3, typename T4>
ISPResult ISPList<T1, T2, T3, T4>::AddNodeToRawTail(ISPNode<T1, T1>* pNode)
{
	ISPResult rt = ISP_SUCCESS;
	ISPNode<T1, T1>* pTmp = mRawHead;

	if (mState != ISP_LIST_CONFIGED) {
		rt = ISP_STATE_ERROR;
		ILOGE("func called in invaled state:%d %d", mState, rt);
	}

	if (SUCCESS(rt)) {
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

	return rt;
}

template<typename T1, typename T2, typename T3, typename T4>
ISPResult ISPList<T1, T2, T3, T4>::CreateRgbList()
{
	ISPResult rt = ISP_SUCCESS;

	int32_t nodePropertyIndex = -1;
	ISPNode<T2, T2>* pNewNode = nullptr;
	PROCESS_TYPE newNodeType = PROCESS_TYPE_NUM;

	if (mState != ISP_LIST_CONFIGED) {
		rt = ISP_STATE_ERROR;
		ILOGE("func called in invaled state:%d %d", mState, rt);
	}

	if (SUCCESS(rt)) {
		for (int32_t i = 0; i < (int32_t)(sizeof(RgbListConfigure) / sizeof(PROCESS_TYPE)); i++) {
			newNodeType = RgbListConfigure[i];
			FindNodePropertyIndex(newNodeType, &nodePropertyIndex);
			if (nodePropertyIndex >= 0) {
				pNewNode = new ISPNode<T2, T2>;
				if (pNewNode) {
					rt = pNewNode->Init(&mProperty.NodeProperty[nodePropertyIndex], pRgbBuffer, pRgbBuffer);
					if (SUCCESS(rt)) {
						rt = AddNodeToRgbTail(pNewNode);
						if (!SUCCESS(rt)) {
							break;
						}
					}
					else {
						break;
					}
				}
				else {
					rt = ISP_MEMORY_ERROR;
					ILOGE("Failed to new node! rt:%d", rt);
					break;
				}
			}
			else {
				ILOGW("Not find node type:%d", newNodeType);
			}
		}
	}

	if (SUCCESS(rt)) {
		char name[NODE_NAME_MAX_SZIE] = { '0' };
		mRgbHead->GetNodeName(name);
		ILOGDL("List(%d) node:%s", mId, name);
		ISPNode<T2, T2>* pNode = mRgbHead->pNext;
		while (pNode) {
			pNode->GetNodeName(name);
			ILOGDL("List(%d) node:%s", mId, name);
			pNode = pNode->pNext;
		}
	}

	return rt;
}

template<typename T1, typename T2, typename T3, typename T4>
ISPResult ISPList<T1, T2, T3, T4>::AddNodeToRgbTail(ISPNode<T2, T2>* pNode)
{
	ISPResult rt = ISP_SUCCESS;
	ISPNode<T2, T2>* pTmp = mRgbHead->pNext;

	if (mState != ISP_LIST_CONFIGED) {
		rt = ISP_STATE_ERROR;
		ILOGE("func called in invaled state:%d %d", mState, rt);
	}

	if (SUCCESS(rt)) {
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

	return rt;
}

template<typename T1, typename T2, typename T3, typename T4>
ISPResult ISPList<T1, T2, T3, T4>::CreateYuvList()
{
	ISPResult rt = ISP_SUCCESS;

	int32_t nodePropertyIndex = -1;
	ISPNode<T3, T3>* pNewNode = nullptr;
	PROCESS_TYPE newNodeType = PROCESS_TYPE_NUM;

	if (mState != ISP_LIST_CONFIGED) {
		rt = ISP_STATE_ERROR;
		ILOGE("func called in invaled state:%d %d", mState, rt);
	}

	if (SUCCESS(rt)) {
		for (int32_t i = 0; i < (int32_t)(sizeof(YuvListConfigure) / sizeof(PROCESS_TYPE)); i++) {
			newNodeType = YuvListConfigure[i];
			FindNodePropertyIndex(newNodeType, &nodePropertyIndex);
			if (nodePropertyIndex >= 0) {
				pNewNode = new ISPNode<T3, T3>;
				if (pNewNode) {

					rt = pNewNode->Init(&mProperty.NodeProperty[nodePropertyIndex], pYuvBuffer, pYuvBuffer);
					if (SUCCESS(rt)) {
						rt = AddNodeToYuvTail(pNewNode);
						if (!SUCCESS(rt)) {
							break;
						}
					}
					else {
						break;
					}
				}
				else {
					rt = ISP_MEMORY_ERROR;
					ILOGE("Failed to new node! rt:%d", rt);
					break;
				}
			}
			else {
				ILOGW("Not find node type:%d", newNodeType);
			}
		}
	}

	if (SUCCESS(rt)) {
		char name[NODE_NAME_MAX_SZIE] = { '0' };
		mYuvHead->GetNodeName(name);
		ILOGDL("List(%d) node:%s", mId, name);
		ISPNode<T3, T3>* pNode = mYuvHead->pNext;
		while (pNode) {
			pNode->GetNodeName(name);
			ILOGDL("List(%d) node:%s", mId, name);
			pNode = pNode->pNext;
		}
	}

	return rt;
}

template<typename T1, typename T2, typename T3, typename T4>
ISPResult ISPList<T1, T2, T3, T4>::AddNodeToYuvTail(ISPNode<T3, T3>* pNode)
{
	ISPResult rt = ISP_SUCCESS;
	ISPNode<T3, T3>* pTmp = mYuvHead->pNext;

	if (mState != ISP_LIST_CONFIGED) {
		rt = ISP_STATE_ERROR;
		ILOGE("func called in invaled state:%d %d", mState, rt);
	}

	if (SUCCESS(rt)) {
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

	return rt;
}

template<typename T1, typename T2, typename T3, typename T4>
ISPResult ISPList<T1, T2, T3, T4>::CreatePostList()
{
	ISPResult rt = ISP_SUCCESS;

	int32_t nodePropertyIndex = -1;
	ISPNode<T4, T4>* pNewNode = nullptr;
	PROCESS_TYPE newNodeType = PROCESS_TYPE_NUM;

	if (mState != ISP_LIST_CONFIGED) {
		rt = ISP_STATE_ERROR;
		ILOGE("func called in invaled state:%d %d", mState, rt);
	}

	if (SUCCESS(rt)) {
		for (int32_t i = 0; i < (int32_t)(sizeof(PostListConfigure) / sizeof(PROCESS_TYPE)); i++) {
			newNodeType = PostListConfigure[i];
			FindNodePropertyIndex(newNodeType, &nodePropertyIndex);
			if (nodePropertyIndex >= 0) {
				pNewNode = new ISPNode<T4, T4>;
				if (pNewNode) {
					rt = pNewNode->Init(&mProperty.NodeProperty[nodePropertyIndex], pPostBuffer, pPostBuffer);
					if (SUCCESS(rt)) {
						rt = AddNodeToPostTail(pNewNode);
						if (!SUCCESS(rt)) {
							break;
						}
					}
					else {
						break;
					}
				}
				else {
					rt = ISP_MEMORY_ERROR;
					ILOGE("Failed to new node! rt:%d", rt);
					break;
				}
			}
			else if (sizeof(PostListConfigure)/sizeof(PROCESS_TYPE) != 1) { //Special logic for post list
				ILOGW("Not find node type:%d", newNodeType);
			}
		}
	}

	if (SUCCESS(rt)) {
		char name[NODE_NAME_MAX_SZIE] = { '0' };
		mPostHead->GetNodeName(name);
		ILOGDL("List(%d) node:%s", mId, name);
		ISPNode<T4, T4>* pNode = mPostHead->pNext;
		while (pNode) {
			pNode->GetNodeName(name);
			ILOGDL("List(%d) node:%s", mId, name);
			pNode = pNode->pNext;
		}
	}

	return rt;
}

template<typename T1, typename T2, typename T3, typename T4>
ISPResult ISPList<T1, T2, T3, T4>::AddNodeToPostTail(ISPNode<T4, T4>* pNode)
{
	ISPResult rt = ISP_SUCCESS;
	ISPNode<T4, T4>* pTmp = mPostHead->pNext;

	if (mState != ISP_LIST_CONFIGED) {
		rt = ISP_STATE_ERROR;
		ILOGE("func called in invaled state:%d %d", mState, rt);
	}

	if (SUCCESS(rt)) {
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
			mPostHead->pNext = pNode;
		}
		mNodeNum++;
	}

	return rt;
}

template<typename T1, typename T2, typename T3, typename T4>
ISPResult ISPList<T1, T2, T3, T4>::Process()
{
	ISPResult rt = ISP_SUCCESS;
	ISPNode<T1, T1>* pNode = mRawHead;

	if (mState != ISP_LIST_CONSTRUCTED) {
		rt = ISP_STATE_ERROR;
		ILOGE("func called in invaled state:%d %d", mState, rt);
	}

	if (SUCCESS(rt)) {
		rt = StateTransform(STATE_TRANS_FORWARD);
	}

	if (SUCCESS(rt)) {
		while (pNode) {
			rt = pNode->Process(pItfWrapper);
			if (SUCCESS(rt)) {
				pNode = pNode->pNext;
			}
			else {
				char name[NODE_NAME_MAX_SZIE] = { '\0' };
				pNode->GetNodeName(name);
				ILOGE("List(%d) %s node process failed! rt:%d", mId, name, rt);
				break;
			}
		}
	}

	if (SUCCESS(rt)) {
		ILOGDL(">>>>> List(%d) Raw process finished! >>>>>", mId);
		rt = TriggerRgbProcess();
	}

	return rt;
}

template<typename T1, typename T2, typename T3, typename T4>
ISPResult ISPList<T1, T2, T3, T4>::TriggerRgbProcess()
{
	ISPResult rt = ISP_SUCCESS;

	if (mState != ISP_LIST_RUNNING) {
		rt = ISP_STATE_ERROR;
		ILOGE("func called in invaled state:%d %d", mState, rt);
	}

	if (SUCCESS(rt)) {
		rt = RgbProcess();
	}

	return rt;
}

template<typename T1, typename T2, typename T3, typename T4>
ISPResult ISPList<T1, T2, T3, T4>::RgbProcess()
{
	ISPResult rt = ISP_SUCCESS;

	if (mState != ISP_LIST_RUNNING) {
		rt = ISP_STATE_ERROR;
		ILOGE("func called in invaled state:%d %d", mState, rt);
	}

	if (SUCCESS(rt)) {
		rt = mRgbHead->Process(pItfWrapper);
	}

	if (SUCCESS(rt)) {
		ISPNode<T2, T2>* pNode = mRgbHead->pNext;
		while (pNode) {
			rt = pNode->Process(pItfWrapper);
			if (SUCCESS(rt)) {
				pNode = pNode->pNext;
			}
			else {
				char name[NODE_NAME_MAX_SZIE] = { '\0' };
				pNode->GetNodeName(name);
				ILOGE("List(%d) %s node process failed! rt:%d", mId, name, rt);
				break;
			}
		}
	}

	if (SUCCESS(rt)) {
		ILOGDL(">>>>> List(%d) Rgb process finished! >>>>>", mId);
		rt = TriggerYuvProcess();
	}

	return rt;
}

template<typename T1, typename T2, typename T3, typename T4>
ISPResult ISPList<T1, T2, T3, T4>::TriggerYuvProcess()
{
	ISPResult rt = ISP_SUCCESS;

	if (mState != ISP_LIST_RUNNING) {
		rt = ISP_STATE_ERROR;
		ILOGE("func called in invaled state:%d %d", mState, rt);
	}

	if (SUCCESS(rt)) {
		rt = YuvProcess();
	}

	return rt;
}

template<typename T1, typename T2, typename T3, typename T4>
ISPResult ISPList<T1, T2, T3, T4>::YuvProcess()
{
	ISPResult rt = ISP_SUCCESS;

	if (mState != ISP_LIST_RUNNING) {
		rt = ISP_STATE_ERROR;
		ILOGE("func called in invaled state:%d %d", mState, rt);
	}

	if (SUCCESS(rt)) {
		rt = mYuvHead->Process(pItfWrapper);
	}

	if (SUCCESS(rt)) {
		ISPNode<T3, T3>* pNode = mYuvHead->pNext;
		while (pNode) {
			rt = pNode->Process(pItfWrapper);
			if (SUCCESS(rt)) {
				pNode = pNode->pNext;
			}
			else {
				char name[NODE_NAME_MAX_SZIE] = { '\0' };
				pNode->GetNodeName(name);
				ILOGE("List(%d) %s node process failed! rt:%d", mId, name, rt);
				break;
			}
		}
	}

	if (SUCCESS(rt)) {
		ILOGDL(">>>>> List(%d) Yuv process finished! >>>>>", mId);
		rt = TriggerPostProcess();
	}

	return rt;
}

template<typename T1, typename T2, typename T3, typename T4>
ISPResult ISPList<T1, T2, T3, T4>::TriggerPostProcess()
{
	ISPResult rt = ISP_SUCCESS;

	if (mState != ISP_LIST_RUNNING) {
		rt = ISP_STATE_ERROR;
		ILOGE("func called in invaled state:%d %d", mState, rt);
	}

	if (SUCCESS(rt)) {
		rt = PostProcess();
	}

	return rt;
}

template<typename T1, typename T2, typename T3, typename T4>
ISPResult ISPList<T1, T2, T3, T4>::PostProcess()
{
	ISPResult rt = ISP_SUCCESS;

	if (mState != ISP_LIST_RUNNING) {
		rt = ISP_STATE_ERROR;
		ILOGE("func called in invaled state:%d %d", mState, rt);
	}

	if (SUCCESS(rt)) {
		rt = mPostHead->Process(pItfWrapper);
	}

	if (SUCCESS(rt)) {
		ISPNode<T4, T4>* pNode = mPostHead->pNext;
		while (pNode) {
			rt = pNode->Process(pItfWrapper);
			if (SUCCESS(rt)) {
				pNode = pNode->pNext;
			}
			else {
				char name[NODE_NAME_MAX_SZIE] = { '\0' };
				pNode->GetNodeName(name);
				ILOGE("List(%d) %s node process failed! rt:%d", mId, name, rt);
				break;
			}
		}
	}

	if (SUCCESS(rt)) {
		ILOGDL(">>>>> List(%d) Post process finished! >>>>>", mId);
		rt = StateTransform(STATE_TRANS_FORWARD);
	}

	return rt;
}

template<typename T1, typename T2, typename T3, typename T4>
ISPResult ISPList<T1, T2, T3, T4>::EnableNodebyType(int32_t type)
{
	ISPResult rt = ISP_SUCCESS;
	bool needFind = true;

	if (mState != ISP_LIST_CONSTRUCTED) {
		rt = ISP_STATE_ERROR;
		ILOGE("func called in invaled state:%d %d", mState, rt);
	}

	if (SUCCESS(rt)) {
		if (type < PROCESS_TYPE_NUM)
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

			if (needFind) {
				ISPNode<T4, T4>* pPostNode = mPostHead->pNext;
				while (pPostNode) {
					if (type == pPostNode->GetProperty().type)
					{
						pPostNode->Enable();
						needFind = false;
						break;
					}
					pPostNode = pPostNode->pNext;
				}
			}
		}
		else {
			rt = EnableNecNodebyType(type);
		}
	}

	return rt;
}

template<typename T1, typename T2, typename T3, typename T4>
ISPResult ISPList<T1, T2, T3, T4>::DisableNodebyType(int32_t type)
{
	ISPResult rt = ISP_SUCCESS;
	bool needFind = true;

	if (mState != ISP_LIST_CONSTRUCTED) {
		rt = ISP_STATE_ERROR;
		ILOGE("func called in invaled state:%d %d", mState, rt);
	}

	if (SUCCESS(rt)) {
		if (type < PROCESS_TYPE_NUM)
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

			if (needFind) {
				ISPNode<T4, T4>* pPostNode = mPostHead->pNext;
				while (pPostNode) {
					if (type == pPostNode->GetProperty().type)
					{
						pPostNode->Disable();
						needFind = false;
						break;
					}
					pPostNode = pPostNode->pNext;
				}
			}
		}
		else {
			rt = DisableNecNodebyType(type);
		}
	}

	return rt;
}

template<typename T1, typename T2, typename T3, typename T4>
ISPResult ISPList<T1, T2, T3, T4>::EnableNecNodebyType(int32_t type)
{
	ISPResult rt = ISP_SUCCESS;

	if (mState != ISP_LIST_CONSTRUCTED) {
		rt = ISP_STATE_ERROR;
		ILOGE("func called in invaled state:%d %d", mState, rt);
	}

	if (SUCCESS(rt)) {
		if (type == NEC_PROCESS_HEAD) {
			rt = mRawHead->Enable();
		}
		else if (type == NEC_PROCESS_CST_RAW2RGB) {
			rt = mRgbHead->Enable();
		}
		else if (type == NEC_PROCESS_CST_RGB2YUV) {
			rt = mYuvHead->Enable();
		}
		else if (type == NEC_PROCESS_CST_YUV2RGB) {
			rt = mPostHead->Enable();
		}
	}

	return rt;
}

template<typename T1, typename T2, typename T3, typename T4>
ISPResult ISPList<T1, T2, T3, T4>::DisableNecNodebyType(int32_t type)
{
	ISPResult rt = ISP_SUCCESS;

	if (mState != ISP_LIST_CONSTRUCTED) {
		rt = ISP_STATE_ERROR;
		ILOGE("func called in invaled state:%d %d", mState, rt);
	}

	if (SUCCESS(rt)) {
		if (type == NEC_PROCESS_HEAD) {
			rt = mRawHead->Disable();
		}
		else if (type == NEC_PROCESS_CST_RAW2RGB) {
			rt = mRgbHead->Disable();
		}
		else if (type == NEC_PROCESS_CST_RGB2YUV) {
			rt = mYuvHead->Disable();
		}
		else if (type == NEC_PROCESS_CST_YUV2RGB) {
			rt = mPostHead->Disable();
		}
	}

	return rt;
}
