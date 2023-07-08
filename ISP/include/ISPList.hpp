// License: GPL-3.0-or-later
/*
 * ISPList is a serial abstract object helps to construct processing
 * steps more flexabl.
 *
 * Copyright (c) 2019 Peng Hao <635945005@qq.com>
 */

#include "ISPList.h"

const int32_t gProcessCmd[NEC_PROCESS_TYPE_NUM] =
{
	ALG_CMD_BLC,
	ALG_CMD_LSC,
	ALG_CMD_DEMOSAIC,
	ALG_CMD_WB,
	ALG_CMD_CC,
	ALG_CMD_GAMMA,
	ALG_CMD_WNR,
	ALG_CMD_EE,
	ALG_CMD_NUM,
	ALG_CMD_NUM,
	ALG_CMD_CTS_RAW2RGB,
	ALG_CMD_CTS_RGB2YUV,
	ALG_CMD_CTS_YUV2RGB,
};

static ISPListHeadProperty gListHeadsConfigure = {
	{{"HEAD",			NEC_PROCESS_HEAD,			NODE_ON},
	 {"CST_RAW2RGB",	NEC_PROCESS_CST_RAW2RGB,	NODE_ON},
	 {"CST_RGB2YUV",	NEC_PROCESS_CST_RGB2YUV,	NODE_ON}, /* support off this node for debug */
	 {"CST_YUV2RGB",	NEC_PROCESS_CST_YUV2RGB,	NODE_ON}}
};

const int32_t RawListConfigure[] = {
	PROCESS_BLC,
	PROCESS_LSC,
};

const int32_t RgbListConfigure[] = {
	PROCESS_Demosaic,
	PROCESS_WB,
	PROCESS_CC,
	PROCESS_GAMMA,
};

const int32_t YuvListConfigure[] = {
	PROCESS_WNR,
	PROCESS_EE,
};

const int32_t PostListConfigure[] = {
	PROCESS_TYPE_NUM
};

template<typename T1, typename T2>
ISPNode<T1, T2>::ISPNode() :
	pNext(nullptr),
	mInited(false),
	pInputBuffer(nullptr),
	pOutputBuffer(nullptr)
{
	memset(&mProperty, 0, sizeof(ISPNodeProperty));
}

template<typename T1, typename T2>
ISPNode<T1, T2>::~ISPNode()
{
	pNext = nullptr;
	mInited = false;
	pInputBuffer = nullptr;
	pOutputBuffer = nullptr;
	memset(&mProperty, 0, sizeof(ISPNodeProperty));
}

template<typename T1, typename T2>
int32_t ISPNode<T1, T2>::Init(ISPNodeProperty *cfg, T1* input, T2* output)
{
	int32_t rt = ISP_SUCCESS;

	if (!input || !output || !cfg) {
		rt = ISP_INVALID_PARAM;
	}

	if (SUCCESS(rt)) {
		memcpy(&mProperty, cfg, sizeof(ISPNodeProperty));
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
int32_t ISPNode<T1, T2>::Enable()
{
	int32_t rt = ISP_SUCCESS;

	mProperty.enable = NODE_ON;

	return rt;
}

template<typename T1, typename T2>
int32_t ISPNode<T1, T2>::Disable()
{
	int32_t rt = ISP_SUCCESS;

	mProperty.enable = NODE_OFF;

	return rt;
}

template<typename T1, typename T2>
bool ISPNode<T1, T2>::isOn()
{
	return (mProperty.enable == NODE_ON) ? true : false;
}

template<typename T1, typename T2>
int32_t ISPNode<T1, T2>::GetNodeName(char* name)
{
	int32_t rt = ISP_SUCCESS;

	if (mInited) {
		memcpy(name, mProperty.name, sizeof(char) * NODE_NAME_MAX_SZIE);
	}
	else {
		rt = ISP_STATE_ERROR;
	}

	return rt;
}

template<typename T1, typename T2>
ISPNodeProperty ISPNode<T1, T2>::GetProperty()
{
	return mProperty;
}

template<typename T1, typename T2>
int32_t ISPNode<T1, T2>::Process()
{
	int32_t rt = ISP_SUCCESS;
	InterfaceWrapperBase* pItf = nullptr;
	char name[NODE_NAME_MAX_SZIE];
	GetNodeName(name);

	if (!mInited) {
		rt = ISP_STATE_ERROR;
		ILOGE("Node is not inited!");
	}

	if (SUCCESS(rt)) {
		pItf = InterfaceWrapper::GetInstance();
		if (!pItf) {
			rt = ISP_INVALID_PARAM;
			ILOGE("Cannot get interface");
		}
	}

	if (SUCCESS(rt)) {
		ILOGDL("%s:Buffer(in:%p out:%p)", name, pInputBuffer, pOutputBuffer);
		rt = pItf->AlgProcess(gProcessCmd[mProperty.type] ,pInputBuffer, mProperty.enable);
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
ISPNecNode<T1, T2>::ISPNecNode()
{
	memset(&mProperty, 0, sizeof(ISPNodeProperty));
}

template<typename T1, typename T2>
ISPNecNode<T1, T2>::~ISPNecNode()
{
	memset(&mProperty, 0, sizeof(ISPNodeProperty));
}

template<typename T1, typename T2>
int32_t ISPNecNode<T1, T2>::Init(ISPNecNodeProperty *cfg, T1* input, T2* output)
{
	int32_t rt = ISP_SUCCESS;

	if (!input || !output || !cfg) {
		rt = ISP_INVALID_PARAM;
	}

	if (SUCCESS(rt)) {
		memcpy(&mProperty, cfg, sizeof(ISPNodeProperty));
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
int32_t ISPNecNode<T1, T2>::GetNodeName(char* name)
{
	int32_t rt = ISP_SUCCESS;
	if (this->mInited) {
		memcpy(name, mProperty.name, sizeof(char) * NODE_NAME_MAX_SZIE);
	}
	else {
		rt = ISP_STATE_ERROR;
	}
	return rt;
}

template<typename T1, typename T2>
int32_t ISPNecNode<T1, T2>::Process()
{
	int32_t rt = ISP_SUCCESS;
	InterfaceWrapperBase *pItf = nullptr;
	char name[NODE_NAME_MAX_SZIE];
	GetNodeName(name);

	if (!this->mInited) {
		rt = ISP_STATE_ERROR;
		ILOGE("Node is not inited!");
	}

	if (SUCCESS(rt)) {
		pItf = InterfaceWrapper::GetInstance();
		if (!pItf) {
			rt = ISP_INVALID_PARAM;
			ILOGE("Cannot get interface");
		}
	}

	if (SUCCESS(rt)) {
		if (mProperty.type != NEC_PROCESS_HEAD) { /* Now nothing todo with head */
			ILOGDL("%s:Buffer(in:%p out:%p)", name, this->pInputBuffer, this->pOutputBuffer);
			rt = pItf->AlgProcess(gProcessCmd[mProperty.type], this->pInputBuffer, this->pOutputBuffer, mProperty.enable);
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
int32_t ISPNecNode<T1, T2>::Disable()
{
	int32_t rt = ISP_SUCCESS;

	char name[NODE_NAME_MAX_SZIE];
	this->GetNodeName(name);
	ILOGW("Try to disable necessary node:%s", name);
	mProperty.enable = NODE_OFF;

	return rt;
}

template<typename T1, typename T2, typename T3, typename T4>
ISPList<T1, T2, T3, T4>::ISPList(int32_t id) :
	mRawHead(nullptr),
	mRgbHead(nullptr),
	mYuvHead(nullptr),
	mPostHead(nullptr),
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

	ILOGDL("List(%d) Node num:%d", mId, mNodeNum);
}

template<typename T1, typename T2, typename T3, typename T4>
int32_t ISPList<T1, T2, T3, T4>::StateTransform(int32_t orientation)
{
	int32_t rt = ISP_SUCCESS;
	int32_t currentState = mState;

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
int32_t ISPList<T1, T2, T3, T4>::Init(T1* pRawBuf, T2* pRgbBuf, T3* pYuvBuf, T4* pPostBuf)
{
	int32_t rt = ISP_SUCCESS;

	if (mState != ISP_LIST_NEW) {
		rt = ISP_STATE_ERROR;
		ILOGE("func called in invaled state:%d %d", mState, rt);
	}

	if (!pRawBuf || !pRgbBuf || !pYuvBuf || !pPostBuf) {
		rt = ISP_INVALID_PARAM;
		ILOGE("Failed to init list(%d), pBuffer or PM is null! rt:%d", mId, rt);
	}

	if (SUCCESS(rt)) {
		pRawBuffer = pRawBuf;
		pRgbBuffer = pRgbBuf;
		pYuvBuffer = pYuvBuf;
		pPostBuffer = pPostBuf;
	}

	if (SUCCESS(rt)) {
		rt = StateTransform(STATE_TRANS_FORWARD);
	}

	return rt;
}

template<typename T1, typename T2, typename T3, typename T4>
int32_t ISPList<T1, T2, T3, T4>::SetListConfig(ISPListProperty* pCfg)
{
	int32_t rt = ISP_SUCCESS;

	if (mState != ISP_LIST_INITED) {
		rt = ISP_STATE_ERROR;
		ILOGE("func called in invaled state:%d %d", mState, rt);
	}

	if (SUCCESS(rt)) {
		if (pCfg) {
			memcpy(&mProperty, pCfg, sizeof(ISPListProperty));
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
int32_t ISPList<T1, T2, T3, T4>::FindNodePropertyIndex(int32_t type, int32_t* index)
{
	int32_t rt = ISP_SUCCESS;

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
int32_t ISPList<T1, T2, T3, T4>::FindNecNodePropertyIndex(int32_t type, int32_t* index)
{
	int32_t rt = ISP_SUCCESS;

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
int32_t ISPList<T1, T2, T3, T4>::CreatISPList()
{
	int32_t rt = ISP_SUCCESS;

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
int32_t ISPList<T1, T2, T3, T4>::CreateNecList()
{
	int32_t rt = ISP_SUCCESS;
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
int32_t ISPList<T1, T2, T3, T4>::CreateRawList()
{
	int32_t rt = ISP_SUCCESS;

	int32_t nodePropertyIndex = -1;
	ISPNode<T1, T1>* pNewNode = nullptr;
	int32_t newNodeType = PROCESS_TYPE_NUM;

	if (mState != ISP_LIST_CONFIGED) {
		rt = ISP_STATE_ERROR;
		ILOGE("func called in invaled state:%d %d", mState, rt);
	}

	if (SUCCESS(rt)) {
		for (uint32_t i = 0; i < (sizeof(RawListConfigure) / sizeof(int32_t)); i++) {
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
int32_t ISPList<T1, T2, T3, T4>::AddNodeToRawTail(ISPNode<T1, T1>* pNode)
{
	int32_t rt = ISP_SUCCESS;
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
int32_t ISPList<T1, T2, T3, T4>::CreateRgbList()
{
	int32_t rt = ISP_SUCCESS;

	int32_t nodePropertyIndex = -1;
	ISPNode<T2, T2>* pNewNode = nullptr;
	int32_t newNodeType = PROCESS_TYPE_NUM;

	if (mState != ISP_LIST_CONFIGED) {
		rt = ISP_STATE_ERROR;
		ILOGE("func called in invaled state:%d %d", mState, rt);
	}

	if (SUCCESS(rt)) {
		for (uint32_t i = 0; i < (sizeof(RgbListConfigure) / sizeof(int32_t)); i++) {
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
int32_t ISPList<T1, T2, T3, T4>::AddNodeToRgbTail(ISPNode<T2, T2>* pNode)
{
	int32_t rt = ISP_SUCCESS;
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
int32_t ISPList<T1, T2, T3, T4>::CreateYuvList()
{
	int32_t rt = ISP_SUCCESS;

	int32_t nodePropertyIndex = -1;
	ISPNode<T3, T3>* pNewNode = nullptr;
	int32_t newNodeType = PROCESS_TYPE_NUM;

	if (mState != ISP_LIST_CONFIGED) {
		rt = ISP_STATE_ERROR;
		ILOGE("func called in invaled state:%d %d", mState, rt);
	}

	if (SUCCESS(rt)) {
		for (uint32_t i = 0; i < (sizeof(YuvListConfigure) / sizeof(int32_t)); i++) {
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
int32_t ISPList<T1, T2, T3, T4>::AddNodeToYuvTail(ISPNode<T3, T3>* pNode)
{
	int32_t rt = ISP_SUCCESS;
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
int32_t ISPList<T1, T2, T3, T4>::CreatePostList()
{
	int32_t rt = ISP_SUCCESS;

	int32_t nodePropertyIndex = -1;
	ISPNode<T4, T4>* pNewNode = nullptr;
	int32_t newNodeType = PROCESS_TYPE_NUM;

	if (mState != ISP_LIST_CONFIGED) {
		rt = ISP_STATE_ERROR;
		ILOGE("func called in invaled state:%d %d", mState, rt);
	}

	if (SUCCESS(rt)) {
		for (uint32_t i = 0; i < (sizeof(PostListConfigure) / sizeof(int32_t)); i++) {
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
			else if (sizeof(PostListConfigure)/sizeof(int32_t) != 1) { //Special logic for post list
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
int32_t ISPList<T1, T2, T3, T4>::AddNodeToPostTail(ISPNode<T4, T4>* pNode)
{
	int32_t rt = ISP_SUCCESS;
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
int32_t ISPList<T1, T2, T3, T4>::Process()
{
	int32_t rt = ISP_SUCCESS;
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
			rt = pNode->Process();
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
int32_t ISPList<T1, T2, T3, T4>::TriggerRgbProcess()
{
	int32_t rt = ISP_SUCCESS;

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
int32_t ISPList<T1, T2, T3, T4>::RgbProcess()
{
	int32_t rt = ISP_SUCCESS;

	if (mState != ISP_LIST_RUNNING) {
		rt = ISP_STATE_ERROR;
		ILOGE("func called in invaled state:%d %d", mState, rt);
	}

	if (SUCCESS(rt)) {
		rt = mRgbHead->Process();
	}

	if (SUCCESS(rt)) {
		ISPNode<T2, T2>* pNode = mRgbHead->pNext;
		while (pNode) {
			rt = pNode->Process();
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
int32_t ISPList<T1, T2, T3, T4>::TriggerYuvProcess()
{
	int32_t rt = ISP_SUCCESS;

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
int32_t ISPList<T1, T2, T3, T4>::YuvProcess()
{
	int32_t rt = ISP_SUCCESS;

	if (mState != ISP_LIST_RUNNING) {
		rt = ISP_STATE_ERROR;
		ILOGE("func called in invaled state:%d %d", mState, rt);
	}

	if (SUCCESS(rt)) {
		rt = mYuvHead->Process();
	}

	if (SUCCESS(rt)) {
		ISPNode<T3, T3>* pNode = mYuvHead->pNext;
		while (pNode) {
			rt = pNode->Process();
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
int32_t ISPList<T1, T2, T3, T4>::TriggerPostProcess()
{
	int32_t rt = ISP_SUCCESS;

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
int32_t ISPList<T1, T2, T3, T4>::PostProcess()
{
	int32_t rt = ISP_SUCCESS;

	if (mState != ISP_LIST_RUNNING) {
		rt = ISP_STATE_ERROR;
		ILOGE("func called in invaled state:%d %d", mState, rt);
	}

	if (SUCCESS(rt)) {
		rt = mPostHead->Process();
	}

	if (SUCCESS(rt)) {
		ISPNode<T4, T4>* pNode = mPostHead->pNext;
		while (pNode) {
			rt = pNode->Process();
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
int32_t ISPList<T1, T2, T3, T4>::EnableNodebyType(int32_t type)
{
	int32_t rt = ISP_SUCCESS;
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
int32_t ISPList<T1, T2, T3, T4>::DisableNodebyType(int32_t type)
{
	int32_t rt = ISP_SUCCESS;
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
int32_t ISPList<T1, T2, T3, T4>::EnableNecNodebyType(int32_t type)
{
	int32_t rt = ISP_SUCCESS;

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
int32_t ISPList<T1, T2, T3, T4>::DisableNecNodebyType(int32_t type)
{
	int32_t rt = ISP_SUCCESS;

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
