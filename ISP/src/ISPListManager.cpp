// License: GPL-3.0-or-later
/*
 * Implementation of ISPListManager functions.
 *
 * Copyright (c) 2019 Peng Hao <635945005@qq.com>
 */

#include "ISPListManager.h"
#include "ISPListConfig.h"
#include "ISPList.hpp"
#include "InterfaceWrapper.h"
#include "ParamManager.h"

void *gISPListConfigs[LIST_CFG_NUM] = {
	(void *)&defaultListConfig
};

ISPListManager::ISPListManager():
	pISPListConfigs(NULL)
{
	Init();
}

ISPListManager::~ISPListManager()
{
	DestroyAllList();
}

ISPListManager* ISPListManager::GetInstance()
{
	static ISPListManager gListMgr;
	return &gListMgr;
}

int32_t ISPListManager::Init()
{
	int32_t rt = ISP_SUCCESS;

	pISPListConfigs = static_cast<ISPListProperty*>(gISPListConfigs[LIST_CFG_DEFAULT]);

	return rt;
}

int32_t ISPListManager::CreateList(uint16_t* pRaw, uint16_t* pBGR, uint8_t* pYUV, uint8_t* pPOST, int32_t cfgIndex, int32_t *id)
{
	int32_t rt = ISP_SUCCESS;
	int32_t listId = 0;
	ISPList<uint16_t, uint16_t, uint8_t, uint8_t>* pIspList = NULL;

	if (cfgIndex >= LIST_CFG_NUM)
	{
		rt = ISP_INVALID_PARAM;
		ILOGE("Invaled cfgIndex:%d %d", cfgIndex, rt);
		return rt;
	}

	{
		unique_lock <mutex> lock(mListMapLock);

		if (!mListMap.empty()) {
			for (auto iter = mListMap.begin(); iter != mListMap.end(); iter++)
			{
				if (iter->first > listId) { /* Id can be reused */
					break;
				} else if (iter->first < listId) {
					rt = ISP_FAILED;
					ILOGE("Fatal: impossible condition");
				}
				listId++;
			}
		}
	}

	pIspList = new ISPList<uint16_t, uint16_t, uint8_t, uint8_t>(listId);
	if (!pIspList) {
		rt = ISP_MEMORY_ERROR;
		ILOGE("Faile to new ISPList, %d", rt);
		return rt;
	}

	rt = pIspList->Init(pRaw, pBGR, pYUV, pPOST);
	if (!SUCCESS(rt)) {
		delete pIspList;
		return rt;
	}

	rt = pIspList->SetListConfig(&pISPListConfigs[LIST_CFG_DEFAULT + cfgIndex]);
	if (!SUCCESS(rt)) {
		delete pIspList;
		return rt;
	}

	rt = ISPParamManager::GetInstance()->CreateParam(listId, SETTING_1920x1080_D65_1000Lux);
	if (!SUCCESS(rt)) {
		delete pIspList;
		return rt;
	}

	rt = pIspList->CreatISPList();
	if (!SUCCESS(rt)) {
		ISPParamManager::GetInstance()->DeleteParam(listId);
		delete pIspList;
		return rt;
	}

	{
		unique_lock <mutex> lock(mListMapLock);
		mListMap.insert(make_pair(listId, pIspList));
	}

	return rt;
}

ISPList<uint16_t, uint16_t, uint8_t, uint8_t>* ISPListManager::FindListById(int32_t id)
{
	ISPList<uint16_t, uint16_t, uint8_t, uint8_t>* pIspList = NULL;
	map<int32_t, ISPList<uint16_t, uint16_t, uint8_t, uint8_t>*>::iterator iter;

	{
		unique_lock <mutex> lock(mListMapLock);

		iter = mListMap.find(id);
		if (iter == mListMap.end()) {
			ILOGE("Invaled id:%d", id);
		} else {
			pIspList = iter->second;
		}
	}

	return pIspList;
}

int32_t ISPListManager::DestroyAllList()
{
	int32_t rt = ISP_SUCCESS;

	while (mListMap.size()) {
		rt |= DestroyListbyId(mListMap.begin()->first);
		if (!SUCCESS(rt)) {
			ILOGE("Faild to destroy list(%d)", mListMap.begin()->first);
		}
	}

	return rt;
}

int32_t ISPListManager::DestroyListbyId(int32_t id)
{
	int32_t rt = ISP_SUCCESS;
	ISPList<uint16_t, uint16_t, uint8_t, uint8_t>* pIspList = NULL;

	pIspList = FindListById(id);
	if (!pIspList) {
		rt = ISP_INVALID_PARAM;
		ILOGE("Invaled index:%d.", id);
		return rt;
	}

	rt = ISPParamManager::GetInstance()->DeleteParam(id);
	if (!SUCCESS(rt)) {
		return rt;
	}

	{
		unique_lock <mutex> lock(mListMapLock);
		mListMap.erase(id);
		delete pIspList;
	}

	return rt;
}

int32_t ISPListManager::StartById(int32_t id)
{
	int32_t rt = ISP_SUCCESS;
	ISPList<uint16_t, uint16_t, uint8_t, uint8_t>* pIspList = NULL;

	pIspList = FindListById(id);
	if (!pIspList) {
		rt = ISP_INVALID_PARAM;
		ILOGE("Invaled index:%d.", id);
		return rt;
	}

	rt = pIspList->Process();

	return rt;
}

int32_t ISPListManager::EnableNodebyType(int32_t id, int32_t type)
{
	int32_t rt = ISP_SUCCESS;
	ISPList<uint16_t, uint16_t, uint8_t, uint8_t>* pIspList = NULL;

	pIspList = FindListById(id);
	if (!pIspList) {
		rt = ISP_INVALID_PARAM;
		ILOGE("Invaled index:%d.", id);
		return rt;
	}

	rt = pIspList->EnableNodebyType(type);

	return rt;
}

int32_t ISPListManager::DisableNodebyType(int32_t id, int32_t type)
{
	int32_t rt = ISP_SUCCESS;
	ISPList<uint16_t, uint16_t, uint8_t, uint8_t>* pIspList = NULL;

	pIspList = FindListById(id);
	if (!pIspList) {
		rt = ISP_INVALID_PARAM;
		ILOGE("Invaled index:%d.", id);
		return rt;
	}

	pIspList->DisableNodebyType(type);

	return rt;
}

int32_t ISPListManager::NotifyList(int32_t id, int32_t type, NotifyData data)
{
	int32_t rt = ISP_SUCCESS;
	ISPList<uint16_t, uint16_t, uint8_t, uint8_t>* pIspList = NULL;

	pIspList = FindListById(id);
	if (!pIspList) {
		rt = ISP_INVALID_PARAM;
		ILOGE("Invaled index:%d.", id);
		return rt;
	}

	rt = pIspList->NotifyNodeByType(type, data);

	return rt;
}
