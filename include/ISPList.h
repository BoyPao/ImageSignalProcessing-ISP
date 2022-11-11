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

enum NEC_PROCESS_TYPE {
	NEC_PROCESS_HEAD = PROCESS_TYPE_NUM + 1,
	NEC_PROCESS_CST_RAW2RGB,
	NEC_PROCESS_CST_RGB2YUV,
	NEC_PROCESS_CST_YUV2RGB,
	NEC_PROCESS_TYPE_NUM
};

enum NODE_SWITCH {
	NODE_OFF = 0,
	NODE_ON
};

/* NODE */
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
	virtual int32_t GetNodeName(char* name);
	virtual int32_t Init(ISP_NODE_PROPERTY* cfg, T1* input, T2* output);
	virtual int32_t Process(void* pItf);
	virtual int32_t Enable();
	virtual int32_t Disable();
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

/* NEC NODE */
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
	int32_t Init(ISP_NECNODE_PROPERTY* cfg, T1* input, T2* output);
	int32_t GetNodeName(char* name);
	int32_t Process(void* pItf);
	int32_t Disable();
private:
	ISP_NECNODE_PROPERTY mProperty;
};

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

template<typename T1, typename T2, typename T3, typename T4>
class ISPList {
public:
	ISPList(int32_t id);
	~ISPList();
	int32_t Init(T1* pRawBuf, T2* pRgbBuf, T3* pYuvBuf, T4* pPostBuf, void* pIW);
	int32_t SetListConfig(ISP_LIST_PROPERTY* pCfg);
	int32_t FindNodePropertyIndex(PROCESS_TYPE type, int32_t* index);
	int32_t FindNecNodePropertyIndex(NEC_PROCESS_TYPE type, int32_t* index);
	int32_t CreatISPList();
	int32_t AddNode(PROCESS_TYPE type);
	int32_t GetNodeNum();
	int32_t Process();
	int32_t EnableNodebyType(int32_t type);
	int32_t DisableNodebyType(int32_t type);
	int32_t EnableNecNodebyType(int32_t type);
	int32_t DisableNecNodebyType(int32_t type);

private:
	int32_t CreateNecList();
	int32_t CreateRawList();
	int32_t CreateRgbList();
	int32_t CreateYuvList();
	int32_t CreatePostList();
	int32_t AddNodeToRawTail(ISPNode<T1, T1>* pNode);
	int32_t AddNodeToRgbTail(ISPNode<T2, T2>* pNode);
	int32_t AddNodeToYuvTail(ISPNode<T3, T3>* pNode);
	int32_t AddNodeToPostTail(ISPNode<T4, T4>* pNode);
	int32_t TriggerRgbProcess();
	int32_t RgbProcess();
	int32_t TriggerYuvProcess();
	int32_t YuvProcess();
	int32_t TriggerPostProcess();
	int32_t PostProcess();
	int32_t StateTransform(STATE_TRANS_ORIENTATION orientation);

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

