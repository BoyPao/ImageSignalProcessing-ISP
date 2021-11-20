//////////////////////////////////////////////////////////////////////////////////////
// Author: Peng Hao <635945005@qq.com>
// License: GPL
//////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////
// @file: ISPListManager.h
// @brief: Head file of ISPListManager. ISPListManager supports multi ISPList.
//////////////////////////////////////////////////////////////////////////////////////

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

	ISPResult Init(ISPParamManager* pPM);
	ISPResult CreateList(uint16_t* pRaw, uint16_t* pBGR, uint8_t* pYUV, uint8_t* pPOST, int32_t cfgIndex, int32_t* id);
	ISPResult DestoryListbyId(int32_t id);
	ISPResult DestoryAllList();
	ISPResult StartById(int32_t id);
	ISPResult EnableNodebyType(int32_t id, PROCESS_TYPE type);
	ISPResult DisableNodebyType(int32_t id, PROCESS_TYPE type);

private:
	ISPList<uint16_t, uint16_t, uint8_t, uint8_t>* FindListById(int32_t id);

	int32_t mListNum;
	ISPParamManager* pParamManager;
	ISP_LIST_PROPERTY* pISPListConfigs;
	map<int32_t, int32_t> mISPListConfigMap;
	map<int32_t, ISPList<uint16_t, uint16_t, uint8_t, uint8_t>*> mListMap;
};
