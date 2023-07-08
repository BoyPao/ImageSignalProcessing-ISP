// License: GPL-3.0-or-later
/*
 * ISPList is a serial abstract object helps to construct processing
 * steps more flexabl.
 *
 * Copyright (c) 2019 Peng Hao <635945005@qq.com>
 */

#include "ISPList.h"
#include "FileManager.h"

using namespace asteroidaxis::isp::resource;

const int32_t gNodeParamTypeMap[NEC_PROCESS_TYPE_NUM] =
{
	BZ_PARAM_TYPE_BLC,
	BZ_PARAM_TYPE_LSC,
	BZ_PARAM_TYPE_DMC,
	BZ_PARAM_TYPE_WB,
	BZ_PARAM_TYPE_CC,
	BZ_PARAM_TYPE_Gamma,
	BZ_PARAM_TYPE_WNR,
	BZ_PARAM_TYPE_EE,
	BZ_PARAM_TYPE_NUM,
	BZ_PARAM_TYPE_NUM,
	BZ_PARAM_TYPE_NUM,
	BZ_PARAM_TYPE_RAW2RGB,
	BZ_PARAM_TYPE_RGB2YUV,
	BZ_PARAM_TYPE_YUV2RGB,
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
	PROCESS_CODER,
};

template<typename T1, typename T2>
ISPNode<T1, T2>::ISPNode(int32_t id) :
	pNext(NULL),
	mInited(false),
	pCtrl(NULL)
{
	mHostId = id;
	memset(&mProperty, 0, sizeof(ISPNodeProperty));
}

template<typename T1, typename T2>
ISPNode<T1, T2>::~ISPNode()
{
	pNext = NULL;
	mInited = false;
	memset(&mProperty, 0, sizeof(ISPNodeProperty));
	if (pCtrl) {
		Buffer::Free(&pCtrl);
	}
}

template<typename T1, typename T2>
int32_t ISPNode<T1, T2>::Init(ISPNodeProperty *cfg, T1* input, T2* output)
{
	int32_t rt = ISP_SUCCESS;

	if (!input || !output || !cfg) {
		rt = ISP_INVALID_PARAM;
		ILOGE("Invalid input");
		return rt;
	}

	pCtrl = Buffer::Alloc(SIZEOF_T(BZCtrl));
	if (!pCtrl) {
		rt = ISP_INVALID_PARAM;
		ILOGE("Faild to alloc ctrl");
		return rt;
	}

	memcpy(&mProperty, cfg, sizeof(ISPNodeProperty));
	BZCtrl *ctl = static_cast<BZCtrl*>(pCtrl->Addr());
	ctl->en = mProperty.enable;
	ctl->pInfo = ISPParamManager::GetInstance()->GetParam(mHostId, BZ_PARAM_TYPE_IMAGE_INFO);
	ctl->pSrc = input;
	ctl->pDst = output;
	ctl->pParam = ISPParamManager::GetInstance()->GetParam(mHostId, gNodeParamTypeMap[mProperty.type]);

	mInited = true;

	return rt;
}

template<typename T1, typename T2>
int32_t ISPNode<T1, T2>::Enable()
{
	int32_t rt = ISP_SUCCESS;

	mProperty.enable = NODE_ON;
	static_cast<BZCtrl*>(pCtrl->Addr())->en = mProperty.enable;

	return rt;
}

template<typename T1, typename T2>
int32_t ISPNode<T1, T2>::Disable()
{
	int32_t rt = ISP_SUCCESS;

	mProperty.enable = NODE_OFF;
	static_cast<BZCtrl*>(pCtrl->Addr())->en = mProperty.enable;

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

	if (!mInited) {
		rt = ISP_STATE_ERROR;
		ILOGE("Node is not inited!");
		return rt;
	}

	BZCtrl *ctl = static_cast<BZCtrl*>(pCtrl->Addr());
	ILOGDL("%s:Buffer(in:%p out:%p)", mProperty.name, ctl->pSrc, ctl->pDst);
	if (mProperty.type == PROCESS_CODER) {
		return FileManager::GetInstance()->SaveImgData(static_cast<uint8_t*>(ctl->pDst));
	}
	rt = InterfaceWrapper::GetInstance()->AlgProcess(mHostId,
			gNodeParamTypeMap[mProperty.type],
			pCtrl->Addr());
	if (!SUCCESS(rt)) {
		ILOGE("Failed to set ctrl at %s", mProperty.name);
		return rt;
	}

	rt = WaitResult();

	return rt;
}

template<typename T1, typename T2>
int32_t ISPNode<T1, T2>::WaitResult()
{
	int32_t rt = ISP_SUCCESS;
	int32_t cnt = 0;

	rtTrigger = ISP_SKIP + 1;
	while(rtTrigger == ISP_SKIP + 1) {
		if (cnt * NODE_CHECK_GAP_US > NODE_WAIT_US_MAX) {
			rtTrigger = 0;
			rt = ISP_TIMEOUT;
			ILOGE("%s: process timeout(%dus) cnt:%d", mProperty.name, NODE_WAIT_US_MAX, cnt);
			return rt;
		}
		cnt++;
		usleep(NODE_CHECK_GAP_US);
	}

	rt = rtTrigger;
	if (!SUCCESS(rt)) {
		ILOGE("%s: Failed to process. %d", mProperty.name, rt);
	} else {
		ILOGI("%s:%s (%d)", mProperty.name, isOn() ? "processed" : "skiped", cnt);
	}

	return rt;
}

template<typename T1, typename T2>
int32_t ISPNode<T1, T2>::Notify(NotifyData data)
{
	int32_t rt = ISP_SUCCESS;

	rtTrigger = data.rt;

	return rt;
}

template<typename T1, typename T2>
ISPNecNode<T1, T2>::ISPNecNode(int32_t id):ISPNode<T1, T2>(id)
{
}

template<typename T1, typename T2>
ISPNecNode<T1, T2>::~ISPNecNode()
{
	this->pNext = NULL;
}

template<typename T1, typename T2>
int32_t ISPNecNode<T1, T2>::Init(ISPNodeProperty *cfg, T1* input, T2* output)
{
	int32_t rt = ISP_SUCCESS;

	if (!input || !output || !cfg) {
		rt = ISP_INVALID_PARAM;
	}

	this->pCtrl = Buffer::Alloc(SIZEOF_T(BZCtrl));
	if (!this->pCtrl) {
		rt = ISP_INVALID_PARAM;
		ILOGE("Faild to alloc ctrl");
		return rt;
	}

	BZCtrl *ctl = static_cast<BZCtrl*>(this->pCtrl->Addr());
	memcpy(&this->mProperty, cfg, sizeof(ISPNodeProperty));
	ctl->en = this->mProperty.enable;
	ctl->pInfo = ISPParamManager::GetInstance()->GetParam(this->mHostId, BZ_PARAM_TYPE_IMAGE_INFO);
	ctl->pSrc = input;
	ctl->pDst = output;
	ctl->pParam = ISPParamManager::GetInstance()->GetParam(this->mHostId, gNodeParamTypeMap[this->mProperty.type]);

	this->mInited = true;

	return rt;
}

template<typename T1, typename T2>
int32_t ISPNecNode<T1, T2>::Process()
{
	int32_t rt = ISP_SUCCESS;

	if (!this->mInited) {
		rt = ISP_STATE_ERROR;
		ILOGE("Node is not inited!");
		return rt;
	}

	if (this->mProperty.type == NEC_PROCESS_HEAD) {
		return rt; /* Now nothing todo with head */
	}

	BZCtrl *ctl = static_cast<BZCtrl*>(this->pCtrl->Addr());
	ILOGDL("%s:Buffer(in:%p out:%p)", this->mProperty.name, ctl->pSrc, ctl->pDst);
	rt = InterfaceWrapper::GetInstance()->AlgProcess(this->mHostId,
			gNodeParamTypeMap[this->mProperty.type],
			this->pCtrl->Addr());
	if (!SUCCESS(rt)) {
		ILOGE("Failed to set ctrl at %s", this->mProperty.name);
		return rt;
	}

	rt = this->WaitResult();

	return rt;
}

template<typename T1, typename T2, typename T3, typename T4>
ISPList<T1, T2, T3, T4>::ISPList(int32_t id) :
	mRawHead(NULL),
	mRgbHead(NULL),
	mYuvHead(NULL),
	mPostHead(NULL),
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
		while (pRawNext != NULL) {
			mRawHead->pNext = pRawNext->pNext;
			delete pRawNext;
			mNodeNum--;
			pRawNext = mRawHead->pNext;
		}
		delete mRawHead;
		mRawHead = NULL;
		mNodeNum--;
	}

	if (mRgbHead) {
		ISPNode<T2, T2>* pRgbNext = mRgbHead->pNext;
		while (pRgbNext != NULL) {
			mRgbHead->pNext = pRgbNext->pNext;
			delete pRgbNext;
			mNodeNum--;
			pRgbNext = mRgbHead->pNext;
		}
		delete mRgbHead;
		mRgbHead = NULL;
		mNodeNum--;

	}

	if (mYuvHead) {
		ISPNode<T3, T3>* pYuvNext = mYuvHead->pNext;
		while (pYuvNext != NULL) {
			mYuvHead->pNext = pYuvNext->pNext;
			delete pYuvNext;
			mNodeNum--;
			pYuvNext = mYuvHead->pNext;
		}
		delete mYuvHead;
		mYuvHead = NULL;
		mNodeNum--;
	}

	if (mPostHead) {
		ISPNode<T4, T4>* pPostNext = mPostHead->pNext;
		while (pPostNext != NULL) {
			mPostHead->pNext = pPostNext->pNext;
			delete pPostNext;
			mNodeNum--;
			pPostNext = mPostHead->pNext;
		}
		delete mPostHead;
		mPostHead = NULL;
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
	} else if (orientation == STATE_TRANS_BACKWARD) {
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
	} else if (orientation != STATE_TRANS_TO_SELF) {
		rt = ISP_INVALID_PARAM;
		ILOGE("Invaled orientation:%d %d", orientation, rt);
		return rt;
	}
	ILOGDL("State: %d -> %d", currentState, mState);

	return rt;
}

template<typename T1, typename T2, typename T3, typename T4>
int32_t ISPList<T1, T2, T3, T4>::Init(T1* pRawBuf, T2* pRgbBuf, T3* pYuvBuf, T4* pPostBuf)
{
	int32_t rt = ISP_SUCCESS;

	if (mState != ISP_LIST_NEW) {
		rt = ISP_STATE_ERROR;
		ILOGE("func called in invaled state:%d %d", mState, rt);
		return rt;
	}

	if (!pRawBuf || !pRgbBuf || !pYuvBuf || !pPostBuf) {
		rt = ISP_INVALID_PARAM;
		ILOGE("Failed to init list(%d), pBuffer or PM is null! rt:%d", mId, rt);
		return rt;
	}

	pRawBuffer = pRawBuf;
	pRgbBuffer = pRgbBuf;
	pYuvBuffer = pYuvBuf;
	pPostBuffer = pPostBuf;

	rt = StateTransform(STATE_TRANS_FORWARD);

	return rt;
}

template<typename T1, typename T2, typename T3, typename T4>
int32_t ISPList<T1, T2, T3, T4>::SetListConfig(ISPListProperty* pCfg)
{
	int32_t rt = ISP_SUCCESS;

	if (mState != ISP_LIST_INITED) {
		rt = ISP_STATE_ERROR;
		ILOGE("func called in invaled state:%d %d", mState, rt);
		return rt;
	}

	if (!pCfg) {
		rt = ISP_INVALID_PARAM;
		ILOGE("pCfg is null! %d", rt);
		return rt;
	}
	memcpy(&mProperty, pCfg, sizeof(ISPListProperty));

	rt = StateTransform(STATE_TRANS_FORWARD);

	return rt;
}

template<typename T1, typename T2, typename T3, typename T4>
int32_t ISPList<T1, T2, T3, T4>::FindNodePropertyIndex(int32_t type, int32_t* index)
{
	int32_t rt = ISP_SUCCESS;

	if (!index) {
		rt = ISP_INVALID_PARAM;
		ILOGE("input is null! %d", rt);
		return rt;
	}

	for (int32_t i = 0; i < PROCESS_TYPE_NUM; i++)
	{
		if (type == mProperty.NodeProperty[i].type)
		{
			*index = i;
			break;
		}
	}

	return rt;
}

template<typename T1, typename T2, typename T3, typename T4>
int32_t ISPList<T1, T2, T3, T4>::FindNecNodePropertyIndex(int32_t type, int32_t* index)
{
	int32_t rt = ISP_SUCCESS;

	if (!index) {
		rt = ISP_INVALID_PARAM;
		ILOGE("input is null! %d", rt);
		return rt;
	}

	for (int32_t i = 0; i < NEC_PROCESS_TYPE_NUM - NEC_PROCESS_HEAD; i++)
	{
		if (type == gListHeadsConfigure.NecNodeProperty[i].type)
		{
			*index = i;
			break;
		}
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
		return rt;
	}

	rt = CreateNecList();
	if (!SUCCESS(rt)) {
		return rt;
	}

	rt = CreateRawList();
	if (!SUCCESS(rt)) {
		return rt;
	}

	rt = CreateRgbList();
	if (!SUCCESS(rt)) {
		return rt;
	}

	rt = CreateYuvList();
	if (!SUCCESS(rt)) {
		return rt;
	}

	rt = CreatePostList();
	if (!SUCCESS(rt)) {
		return rt;
	}

	rt = InterfaceWrapper::GetInstance()->AlgISPListCreate(mId);
	if (!SUCCESS(rt)) {
		return rt;
	}

	ILOGDL("List(%d) current node num:%d", mId, mNodeNum);
	rt = StateTransform(STATE_TRANS_FORWARD);

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
		return rt;
	}

	if (mRawHead || mRgbHead || mYuvHead || mPostHead) {
		rt = ISP_STATE_ERROR;
		ILOGE("List(%d) create new list failed! Old head exits! rt:%d", mId, rt);
		return rt;
	}

	//Head node create
	FindNecNodePropertyIndex(NEC_PROCESS_HEAD, &nodePropertyIndex);
	mRawHead = new ISPNecNode<T1, T1>(mId);
	if (!mRawHead) {
		rt = ISP_MEMORY_ERROR;
		ILOGE("New node failed! %d", rt);
		return rt;
	}
	mRawHead->Init(&gListHeadsConfigure.NecNodeProperty[nodePropertyIndex], pRawBuffer, pRawBuffer);
	mNodeNum++;

	//Raw -> Rgb node create
	FindNecNodePropertyIndex(NEC_PROCESS_CST_RAW2RGB, &nodePropertyIndex);
	mRgbHead = new ISPNecNode<T1, T2>(mId);
	if (!mRgbHead) {
		rt = ISP_MEMORY_ERROR;
		ILOGE("New node failed! %d", rt);
		return rt;
	}
	mRgbHead->Init(&gListHeadsConfigure.NecNodeProperty[nodePropertyIndex], pRawBuffer, pRgbBuffer);
	mNodeNum++;

	//Rgb -> YUV node create
	FindNecNodePropertyIndex(NEC_PROCESS_CST_RGB2YUV, &nodePropertyIndex);
	mYuvHead = new ISPNecNode<T2, T3>(mId);
	if (!mYuvHead) {
		rt = ISP_MEMORY_ERROR;
		ILOGE("New node failed! %d", rt);
		return rt;
	}
	mYuvHead->Init(&gListHeadsConfigure.NecNodeProperty[nodePropertyIndex], pRgbBuffer, pYuvBuffer);
	mNodeNum++;

	//YUV -> Post node create
	FindNecNodePropertyIndex(NEC_PROCESS_CST_YUV2RGB, &nodePropertyIndex);
	mPostHead = new ISPNecNode<T3, T4>(mId);
	if (!mPostHead) {
		rt = ISP_MEMORY_ERROR;
		ILOGE("New node failed! %d", rt);
		return rt;
	}
	mPostHead->Init(&gListHeadsConfigure.NecNodeProperty[nodePropertyIndex], pYuvBuffer, pPostBuffer);
	mNodeNum++;

	ILOGDL("List(%d) nec num:%d Raw(%p) Rgb(%p) Yuv(%p) Post(%p)", mId, mNodeNum,
			mRawHead, mRgbHead, mYuvHead, mPostHead);

	return rt;
}

template<typename T1, typename T2, typename T3, typename T4>
int32_t ISPList<T1, T2, T3, T4>::CreateRawList()
{
	int32_t rt = ISP_SUCCESS;

	int32_t nodePropertyIndex = -1;
	ISPNode<T1, T1>* pNewNode = NULL;
	int32_t newNodeType = PROCESS_TYPE_NUM;

	if (mState != ISP_LIST_CONFIGED) {
		rt = ISP_STATE_ERROR;
		ILOGE("func called in invaled state:%d %d", mState, rt);
		return rt;
	}

	for (uint32_t i = 0; i < (sizeof(RawListConfigure) / sizeof(int32_t)); i++) {
		newNodeType = RawListConfigure[i];
		FindNodePropertyIndex(newNodeType, &nodePropertyIndex);
		if (nodePropertyIndex < 0) {
			rt = ISP_FAILED;
			ILOGE("Not find node type:%d", newNodeType);
			return rt;
		}
		pNewNode = new ISPNode<T1, T1>(mId);
		if (!pNewNode) {
			rt = ISP_MEMORY_ERROR;
			ILOGE("Failed to new node! rt:%d", rt);
			return rt;
		}
		rt = pNewNode->Init(&mProperty.NodeProperty[nodePropertyIndex], pRawBuffer, pRawBuffer);
		if (!SUCCESS(rt)) {
			return rt;
		}
		rt = AddNodeToRawTail(pNewNode);
		if (!SUCCESS(rt)) {
			return rt;
		}
	}

	char name[NODE_NAME_MAX_SZIE] = { '0' };
	ISPNode<T1, T1>* pNode = mRawHead;
	while (pNode) {
		pNode->GetNodeName(name);
		ILOGDL("List(%d) node:%s", mId, name);
		pNode = pNode->pNext;
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
		return rt;
	}

	while (pTmp->pNext) {
		pTmp = pTmp->pNext;
	}

	pTmp->pNext = pNode;
	mNodeNum++;

	return rt;
}

template<typename T1, typename T2, typename T3, typename T4>
int32_t ISPList<T1, T2, T3, T4>::CreateRgbList()
{
	int32_t rt = ISP_SUCCESS;

	int32_t nodePropertyIndex = -1;
	ISPNode<T2, T2>* pNewNode = NULL;
	int32_t newNodeType = PROCESS_TYPE_NUM;

	if (mState != ISP_LIST_CONFIGED) {
		rt = ISP_STATE_ERROR;
		ILOGE("func called in invaled state:%d %d", mState, rt);
		return rt;
	}

	for (uint32_t i = 0; i < (sizeof(RgbListConfigure) / sizeof(int32_t)); i++) {
		newNodeType = RgbListConfigure[i];
		FindNodePropertyIndex(newNodeType, &nodePropertyIndex);
		if (nodePropertyIndex < 0) {
			rt = ISP_FAILED;
			ILOGE("Not find node type:%d", newNodeType);
			return rt;
		}
		pNewNode = new ISPNode<T2, T2>(mId);
		if (!pNewNode) {
			rt = ISP_MEMORY_ERROR;
			ILOGE("Failed to new node! rt:%d", rt);
			return rt;
		}
		rt = pNewNode->Init(&mProperty.NodeProperty[nodePropertyIndex], pRgbBuffer, pRgbBuffer);
		if (!SUCCESS(rt)) {
			return rt;
		}
		rt = AddNodeToRgbTail(pNewNode);
		if (!SUCCESS(rt)) {
			return rt;
		}
	}

	char name[NODE_NAME_MAX_SZIE] = { '0' };
	mRgbHead->GetNodeName(name);
	ILOGDL("List(%d) node:%s", mId, name);
	ISPNode<T2, T2>* pNode = mRgbHead->pNext;
	while (pNode) {
		pNode->GetNodeName(name);
		ILOGDL("List(%d) node:%s", mId, name);
		pNode = pNode->pNext;
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
		return rt;
	}

	if (pTmp) {
		while (pTmp->pNext) {
			pTmp = pTmp->pNext;
		}
		pTmp->pNext = pNode;
	} else {
		mRgbHead->pNext = pNode;
	}
	mNodeNum++;

	return rt;
}

template<typename T1, typename T2, typename T3, typename T4>
int32_t ISPList<T1, T2, T3, T4>::CreateYuvList()
{
	int32_t rt = ISP_SUCCESS;

	int32_t nodePropertyIndex = -1;
	ISPNode<T3, T3>* pNewNode = NULL;
	int32_t newNodeType = PROCESS_TYPE_NUM;

	if (mState != ISP_LIST_CONFIGED) {
		rt = ISP_STATE_ERROR;
		ILOGE("func called in invaled state:%d %d", mState, rt);
		return rt;
	}

	for (uint32_t i = 0; i < (sizeof(YuvListConfigure) / sizeof(int32_t)); i++) {
		newNodeType = YuvListConfigure[i];
		FindNodePropertyIndex(newNodeType, &nodePropertyIndex);
		if (nodePropertyIndex < 0) {
			rt = ISP_FAILED;
			ILOGE("Not find node type:%d", newNodeType);
			return rt;
		}
		pNewNode = new ISPNode<T3, T3>(mId);
		if (!pNewNode) {
			rt = ISP_MEMORY_ERROR;
			ILOGE("Failed to new node! rt:%d", rt);
			return rt;
		}
		rt = pNewNode->Init(&mProperty.NodeProperty[nodePropertyIndex], pYuvBuffer, pYuvBuffer);
		if (!SUCCESS(rt)) {
			return rt;
		}
		rt = AddNodeToYuvTail(pNewNode);
		if (!SUCCESS(rt)) {
			return rt;
		}
	}

	char name[NODE_NAME_MAX_SZIE] = { '0' };
	mYuvHead->GetNodeName(name);
	ILOGDL("List(%d) node:%s", mId, name);
	ISPNode<T3, T3>* pNode = mYuvHead->pNext;
	while (pNode) {
		pNode->GetNodeName(name);
		ILOGDL("List(%d) node:%s", mId, name);
		pNode = pNode->pNext;
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
		return rt;
	}

	if (pTmp) {
		while (pTmp->pNext) {
			pTmp = pTmp->pNext;
		}
		pTmp->pNext = pNode;
	} else {
		mYuvHead->pNext = pNode;
	}
	mNodeNum++;

	return rt;
}

template<typename T1, typename T2, typename T3, typename T4>
int32_t ISPList<T1, T2, T3, T4>::CreatePostList()
{
	int32_t rt = ISP_SUCCESS;

	int32_t nodePropertyIndex = -1;
	ISPNode<T4, T4>* pNewNode = NULL;
	int32_t newNodeType = PROCESS_TYPE_NUM;

	if (mState != ISP_LIST_CONFIGED) {
		rt = ISP_STATE_ERROR;
		ILOGE("func called in invaled state:%d %d", mState, rt);
		return rt;
	}

	for (uint32_t i = 0; i < (sizeof(PostListConfigure) / sizeof(int32_t)); i++) {
		newNodeType = PostListConfigure[i];
		FindNodePropertyIndex(newNodeType, &nodePropertyIndex);
		if (nodePropertyIndex < 0) {
			rt = ISP_FAILED;
			ILOGE("Not find node type:%d i:%d", newNodeType, i);
			return rt;
		}
		pNewNode = new ISPNode<T4, T4>(mId);
		if (!pNewNode) {
			rt = ISP_MEMORY_ERROR;
			ILOGE("Failed to new node! rt:%d", rt);
			return rt;
		}
		rt = pNewNode->Init(&mProperty.NodeProperty[nodePropertyIndex], pPostBuffer, pPostBuffer);
		if (!SUCCESS(rt)) {
			return rt;
		}
		rt = AddNodeToPostTail(pNewNode);
		if (!SUCCESS(rt)) {
			return rt;
		}
	}

	char name[NODE_NAME_MAX_SZIE] = { '0' };
	mPostHead->GetNodeName(name);
	ILOGDL("List(%d) node:%s", mId, name);
	ISPNode<T4, T4>* pNode = mPostHead->pNext;
	while (pNode) {
		pNode->GetNodeName(name);
		ILOGDL("List(%d) node:%s", mId, name);
		pNode = pNode->pNext;
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
		return rt;
	}

	if (pTmp) {
		while (pTmp->pNext) {
			pTmp = pTmp->pNext;
		}
		pTmp->pNext = pNode;
	} else {
		mPostHead->pNext = pNode;
	}
	mNodeNum++;

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
		return rt;
	}

	rt = StateTransform(STATE_TRANS_FORWARD);
	if (!SUCCESS(rt)) {
		return rt;
	}

	while (pNode) {
		rt = pNode->Process();
		if (!SUCCESS(rt)) {
			char name[NODE_NAME_MAX_SZIE] = { '\0' };
			pNode->GetNodeName(name);
			ILOGE("List(%d) %s node process failed! rt:%d", mId, name, rt);
			return rt;
		}
		pNode = pNode->pNext;
	}

	ILOGDL(">>>>> List(%d) Raw process finished! >>>>>", mId);
	rt = TriggerRgbProcess();

	return rt;
}

template<typename T1, typename T2, typename T3, typename T4>
int32_t ISPList<T1, T2, T3, T4>::TriggerRgbProcess()
{
	int32_t rt = ISP_SUCCESS;

	if (mState != ISP_LIST_RUNNING) {
		rt = ISP_STATE_ERROR;
		ILOGE("func called in invaled state:%d %d", mState, rt);
		return rt;
	}

	rt = RgbProcess();

	return rt;
}

template<typename T1, typename T2, typename T3, typename T4>
int32_t ISPList<T1, T2, T3, T4>::RgbProcess()
{
	int32_t rt = ISP_SUCCESS;

	if (mState != ISP_LIST_RUNNING) {
		rt = ISP_STATE_ERROR;
		ILOGE("func called in invaled state:%d %d", mState, rt);
		return rt;
	}

	rt = mRgbHead->Process();
	if (!SUCCESS(rt)) {
		return rt;
	}

	ISPNode<T2, T2>* pNode = mRgbHead->pNext;
	while (pNode) {
		rt = pNode->Process();
		if (!SUCCESS(rt)) {
			char name[NODE_NAME_MAX_SZIE] = { '\0' };
			pNode->GetNodeName(name);
			ILOGE("List(%d) %s node process failed! rt:%d", mId, name, rt);
			return rt;
		}
		pNode = pNode->pNext;
	}

	ILOGDL(">>>>> List(%d) Rgb process finished! >>>>>", mId);
	rt = TriggerYuvProcess();

	return rt;
}

template<typename T1, typename T2, typename T3, typename T4>
int32_t ISPList<T1, T2, T3, T4>::TriggerYuvProcess()
{
	int32_t rt = ISP_SUCCESS;

	if (mState != ISP_LIST_RUNNING) {
		rt = ISP_STATE_ERROR;
		ILOGE("func called in invaled state:%d %d", mState, rt);
		return rt;
	}

	rt = YuvProcess();

	return rt;
}

template<typename T1, typename T2, typename T3, typename T4>
int32_t ISPList<T1, T2, T3, T4>::YuvProcess()
{
	int32_t rt = ISP_SUCCESS;

	if (mState != ISP_LIST_RUNNING) {
		rt = ISP_STATE_ERROR;
		ILOGE("func called in invaled state:%d %d", mState, rt);
		return rt;
	}

	rt = mYuvHead->Process();
	if (!SUCCESS(rt)) {
		return rt;
	}

	ISPNode<T3, T3>* pNode = mYuvHead->pNext;
	while (pNode) {
		rt = pNode->Process();
		if (!SUCCESS(rt)) {
			char name[NODE_NAME_MAX_SZIE] = { '\0' };
			pNode->GetNodeName(name);
			ILOGE("List(%d) %s node process failed! rt:%d", mId, name, rt);
			return rt;
		}
		pNode = pNode->pNext;
	}

	ILOGDL(">>>>> List(%d) Yuv process finished! >>>>>", mId);
	rt = TriggerPostProcess();

	return rt;
}

template<typename T1, typename T2, typename T3, typename T4>
int32_t ISPList<T1, T2, T3, T4>::TriggerPostProcess()
{
	int32_t rt = ISP_SUCCESS;

	if (mState != ISP_LIST_RUNNING) {
		rt = ISP_STATE_ERROR;
		ILOGE("func called in invaled state:%d %d", mState, rt);
		return rt;
	}

	rt = PostProcess();

	return rt;
}

template<typename T1, typename T2, typename T3, typename T4>
int32_t ISPList<T1, T2, T3, T4>::PostProcess()
{
	int32_t rt = ISP_SUCCESS;

	if (mState != ISP_LIST_RUNNING) {
		rt = ISP_STATE_ERROR;
		ILOGE("func called in invaled state:%d %d", mState, rt);
		return rt;
	}

	rt = mPostHead->Process();
	if (!SUCCESS(rt)) {
		return rt;
	}

	ISPNode<T4, T4>* pNode = mPostHead->pNext;
	while (pNode) {
		rt = pNode->Process();
		if (!SUCCESS(rt)) {
			char name[NODE_NAME_MAX_SZIE] = { '\0' };
			pNode->GetNodeName(name);
			ILOGE("List(%d) %s node process failed! rt:%d", mId, name, rt);
			return rt;
		}
		pNode = pNode->pNext;
	}

	ILOGDL(">>>>> List(%d) Post process finished! >>>>>", mId);
	rt = StateTransform(STATE_TRANS_FORWARD);

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
		return rt;
	}

	if (type < PROCESS_TYPE_NUM) {
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
	} else {
		rt = EnableNecNodebyType(type);
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
		return rt;
	}

	if (type < PROCESS_TYPE_NUM) {
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
	} else {
		rt = DisableNecNodebyType(type);
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
		return rt;
	}

	if (type == NEC_PROCESS_HEAD) {
		rt = mRawHead->Enable();
	} else if (type == NEC_PROCESS_CST_RAW2RGB) {
		rt = mRgbHead->Enable();
	} else if (type == NEC_PROCESS_CST_RGB2YUV) {
		rt = mYuvHead->Enable();
	} else if (type == NEC_PROCESS_CST_YUV2RGB) {
		rt = mPostHead->Enable();
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
		return rt;
	}

	if (type == NEC_PROCESS_HEAD) {
		rt = mRawHead->Disable();
	} else if (type == NEC_PROCESS_CST_RAW2RGB) {
		rt = mRgbHead->Disable();
	} else if (type == NEC_PROCESS_CST_RGB2YUV) {
		rt = mYuvHead->Disable();
	} else if (type == NEC_PROCESS_CST_YUV2RGB) {
		rt = mPostHead->Disable();
	}

	return rt;
}

template<typename T1, typename T2, typename T3, typename T4>
int32_t ISPList<T1, T2, T3, T4>::NotifyNodeByType(int32_t type, NotifyData data)
{
	int32_t rt = ISP_SUCCESS;
	bool needFind = true;

	if (type < BZ_PARAM_TYPE_RAW2RGB) {
		ISPNode<T1, T1>* pRawNode = mRawHead->pNext;
		while (pRawNode) {
			if (type == gNodeParamTypeMap[pRawNode->GetProperty().type])
			{
				pRawNode->Notify(data);
				needFind = false;
				break;
			}
			pRawNode = pRawNode->pNext;
		}
		if (needFind) {
			ISPNode<T2, T2>* pRgbNode = mRgbHead->pNext;
			while (pRgbNode) {
				if (type == gNodeParamTypeMap[pRgbNode->GetProperty().type])
				{
					pRgbNode->Notify(data);
					needFind = false;
					break;
				}
				pRgbNode = pRgbNode->pNext;
			}
		}
		if (needFind) {
			ISPNode<T3, T3>* pYuvNode = mYuvHead->pNext;
			while (pYuvNode) {
				if (type == gNodeParamTypeMap[pYuvNode->GetProperty().type])
				{
					pYuvNode->Notify(data);
					needFind = false;
					break;
				}
				pYuvNode = pYuvNode->pNext;
			}
		}
		if (needFind) {
			ISPNode<T4, T4>* pPostNode = mPostHead->pNext;
			while (pPostNode) {
				if (type == gNodeParamTypeMap[pPostNode->GetProperty().type])
				{
					pPostNode->Notify(data);
					needFind = false;
					break;
				}
				pPostNode = pPostNode->pNext;
			}
		}
	} else {
		if (type == gNodeParamTypeMap[NEC_PROCESS_HEAD]) {
			rt = mRawHead->Notify(data);
		} else if (type == gNodeParamTypeMap[NEC_PROCESS_CST_RAW2RGB]) {
			rt = mRgbHead->Notify(data);
		} else if (type == gNodeParamTypeMap[NEC_PROCESS_CST_RGB2YUV]) {
			rt = mYuvHead->Notify(data);
		} else if (type == gNodeParamTypeMap[NEC_PROCESS_CST_YUV2RGB]) {
			rt = mPostHead->Notify(data);
		}
	}

	return rt;
}
