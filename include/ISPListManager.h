// License: GPL-3.0-or-later
/*
 * ISPListManager supports multi ISPList.
 *
 * Copyright (c) 2019 Peng Hao <635945005@qq.com>
 */

#pragma once
#include "ISPList.h"
#include <map>

using namespace std;

enum LIST_CFG_INDEX {
	LIST_CFG_DEFAULT = 0,
	LIST_CFG_NUM
};

class ISPListManager
{
public:
	ISPListManager();
	~ISPListManager();

	int32_t Init(ISPParamManager* pPM, InterfaceWrapper* pIW);
	int32_t CreateList(uint16_t* pRaw, uint16_t* pBGR, uint8_t* pYUV, uint8_t* pPOST, int32_t cfgIndex, int32_t* id);
	int32_t DestoryListbyId(int32_t id);
	int32_t DestoryAllList();
	int32_t StartById(int32_t id);
	int32_t EnableNodebyType(int32_t id, PROCESS_TYPE type);
	int32_t DisableNodebyType(int32_t id, PROCESS_TYPE type);

private:
	ISPList<uint16_t, uint16_t, uint8_t, uint8_t>* FindListById(int32_t id);

	int32_t mListNum;
	ISPParamManager* pParamManager;
	InterfaceWrapper* pItfWrapper;
	ISP_LIST_PROPERTY* pISPListConfigs;
	map<int32_t, int32_t> mISPListConfigMap;
	map<int32_t, ISPList<uint16_t, uint16_t, uint8_t, uint8_t>*> mListMap;
};
