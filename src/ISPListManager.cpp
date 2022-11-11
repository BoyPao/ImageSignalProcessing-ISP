// License: GPL-3.0-or-later
/*
 * Implementation of ISPListManager functions.
 *
 * Copyright (c) 2019 Peng Hao <635945005@qq.com>
 */

#include "ISPListManager.h"
#include "ISPListConfig.h"
#include "ISPList.hpp"

ISP_LIST_PROPERTY* gISPListConfigs[LIST_CFG_NUM] = {
	&DefaultListConfig
};

ISPListManager::ISPListManager():
	mListNum(0),
	pParamManager(nullptr),
	pItfWrapper(nullptr),
	pISPListConfigs(nullptr)
{
}

ISPListManager::~ISPListManager()
{
	DestoryAllList();
}

int32_t ISPListManager::Init(ISPParamManager* pPM, InterfaceWrapper* pIW)
{
	int32_t rt = ISP_SUCCESS;

	if (!pPM || !pIW)
	{
		rt = ISP_INVALID_PARAM;
		ILOGE("pPM is null. %d", rt);
	}

	if (SUCCESS(rt)) {
		pParamManager = pPM;
		pItfWrapper = pIW;
	}

	if (SUCCESS(rt)) {
		pISPListConfigs = gISPListConfigs[LIST_CFG_DEFAULT];
	}

	return rt;
}

int32_t ISPListManager::CreateList(uint16_t* pRaw, uint16_t* pBGR, uint8_t* pYUV, uint8_t* pPOST, int32_t cfgIndex, int32_t *id)
{
	int32_t rt = ISP_SUCCESS;
	int32_t listId = 0;
	ISPList<uint16_t, uint16_t, uint8_t, uint8_t>* pIspList = nullptr;

	if (cfgIndex >= LIST_CFG_NUM)
	{
		rt = ISP_INVALID_PARAM;
		ILOGE("Invaled cfgIndex:%d %d", cfgIndex, rt);
	}

	if (SUCCESS(rt)) {
		if (mListMap.find(mListNum) == mListMap.end()) {
			listId = mListNum;
		}
		else {
			/* It means some lists have been destoried, ids can be reused */
			for (int32_t i = 0; i < mListNum; i++)
			{
				if (mListMap.find(i) != mListMap.end()) {
					listId = i;
					break;
				}
			}
		}
	}

	if (SUCCESS(rt)) {
		pIspList = new ISPList<uint16_t, uint16_t, uint8_t, uint8_t>(listId);
		if (pIspList) {
			rt = pIspList->Init(pRaw, pBGR, pYUV, pPOST, pItfWrapper);
		}
		else {
			rt = ISP_MEMORY_ERROR;
			ILOGE("Faile to new ISPList, %d", rt);
		}
	}

	if (SUCCESS(rt)) {
		rt = pIspList->SetListConfig(&pISPListConfigs[LIST_CFG_DEFAULT + cfgIndex]);
	}

	if (SUCCESS(rt)) {
		rt = pIspList->CreatISPList();
	}

	if (SUCCESS(rt)) {
		mListMap.insert(make_pair(listId, pIspList));
		mISPListConfigMap.insert(make_pair(listId, cfgIndex));
		mListNum = mListMap.size();
		if (id) {
			*id = listId;
		}
	}

	return rt;
}

ISPList<uint16_t, uint16_t, uint8_t, uint8_t>* ISPListManager::FindListById(int32_t id)
{
	ISPList<uint16_t, uint16_t, uint8_t, uint8_t>* pIspList = nullptr;

	if (mListMap.find(id) != mListMap.end()) {
		pIspList = mListMap.find(id)->second;
	}
	else {
		ILOGE("Invaled index:%d", id);
	}

	return pIspList;
}

int32_t ISPListManager::DestoryAllList()
{
	int32_t rt = ISP_SUCCESS;
	ISPList<uint16_t, uint16_t, uint8_t, uint8_t>* pIspList = nullptr;

	while (mListMap.size()) {
		pIspList = mListMap.begin()->second;
		mISPListConfigMap.erase(mISPListConfigMap.begin()->first);
		mListMap.erase(mListMap.begin()->first);
		if (pIspList) {
			delete pIspList;
		}
		else {
			rt = ISP_FAILED;
			ILOGE("key:%d -> list is null!!!", mListMap.begin()->first);
		}
	}

	mListNum = 0;

	return rt;
}

int32_t ISPListManager::DestoryListbyId(int32_t id)
{
	int32_t rt = ISP_SUCCESS;
	ISPList<uint16_t, uint16_t, uint8_t, uint8_t>* pIspList = nullptr;

	pIspList = FindListById(id);
	if (pIspList) {
		mISPListConfigMap.erase(id);
		mListMap.erase(id);
		mListNum = mListMap.size();
		delete pIspList;
	}
	else {
		rt = ISP_INVALID_PARAM;
		ILOGE("Invaled index:%d. %d", id, rt);
	}

	return rt;
}

int32_t ISPListManager::StartById(int32_t id)
{
	int32_t rt = ISP_SUCCESS;
	ISPList<uint16_t, uint16_t, uint8_t, uint8_t>* pIspList = nullptr;

	if (!pItfWrapper) {
		rt = ISP_FAILED;
		ILOGE("itf not init!");
	}

	if (SUCCESS(rt)) {
		pIspList = FindListById(id);
		if (pIspList) {
			rt = pItfWrapper->ISPLibConfig((void*)pParamManager);
			if (SUCCESS(rt)) {
				rt = pIspList->Process();
			}
		}
		else {
			rt = ISP_INVALID_PARAM;
			ILOGE("Invaled index:%d. %d", id, rt);
		}
	}

	return rt;
}

int32_t ISPListManager::EnableNodebyType(int32_t id, PROCESS_TYPE type)
{
	int32_t rt = ISP_SUCCESS;
	ISPList<uint16_t, uint16_t, uint8_t, uint8_t>* pIspList = nullptr;

	pIspList = FindListById(id);
	if (pIspList) {
		pIspList->EnableNodebyType(type);
	}
	else {
		rt = ISP_INVALID_PARAM;
		ILOGE("Invaled index:%d. %d", id, rt);
	}

	return rt;
}

int32_t ISPListManager::DisableNodebyType(int32_t id, PROCESS_TYPE type)
{
	int32_t rt = ISP_SUCCESS;
	ISPList<uint16_t, uint16_t, uint8_t, uint8_t>* pIspList = nullptr;

	pIspList = FindListById(id);
	if (pIspList) {
		pIspList->DisableNodebyType(type);
	}
	else {
		rt = ISP_INVALID_PARAM;
		ILOGE("Invaled index:%d. %d", id, rt);
	}

	return rt;
}
