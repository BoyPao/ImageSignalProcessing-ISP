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

enum ListCfgIndex {
	LIST_CFG_DEFAULT = 0,
	LIST_CFG_NUM
};

class ISPListManagerItf
{
public:
	virtual ~ISPListManagerItf() {};

	virtual int32_t CreateList(uint16_t* pRaw, uint16_t* pBGR, uint8_t* pYUV, uint8_t* pPOST, int32_t cfgIndex, int32_t* id) = 0;
	virtual int32_t DestoryListbyId(int32_t id) = 0;
	virtual int32_t DestoryAllList() = 0;
	virtual int32_t StartById(int32_t id) = 0;
	virtual int32_t EnableNodebyType(int32_t id, ProcessType type) = 0;
	virtual int32_t DisableNodebyType(int32_t id, ProcessType type) = 0;

};

class ISPListManager : public ISPListManagerItf
{
public:
	static ISPListManager* GetInstance();
	virtual int32_t CreateList(uint16_t* pRaw, uint16_t* pBGR, uint8_t* pYUV, uint8_t* pPOST, int32_t cfgIndex, int32_t* id);
	virtual int32_t DestoryListbyId(int32_t id);
	virtual int32_t DestoryAllList();
	virtual int32_t StartById(int32_t id);
	virtual int32_t EnableNodebyType(int32_t id, ProcessType type);
	virtual int32_t DisableNodebyType(int32_t id, ProcessType type);

private:
	ISPListManager();
	virtual ~ISPListManager();
	int32_t Init();
	ISPList<uint16_t, uint16_t, uint8_t, uint8_t>* FindListById(int32_t id);

	int32_t mListNum;
	ISPParamManager* pParamManager;
	InterfaceWrapper* pItfWrapper;
	ISPListProperty* pISPListConfigs;
	map<int32_t, int32_t> mISPListConfigMap;
	map<int32_t, ISPList<uint16_t, uint16_t, uint8_t, uint8_t>*> mListMap;
};
