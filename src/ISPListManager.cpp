//////////////////////////////////////////////////////////////////////////////////////
// Author: Peng Hao <635945005@qq.com>
// License: GPL
//////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////
// @file: ISPListManager.cpp
// @brief: Source file of ISPListManager.
//////////////////////////////////////////////////////////////////////////////////////

#include "ISPListManager.h"
#include "ISPListConfig.h"

ISP_LIST_PROPERTY* gISPListConfigs[LIST_CFG_NUM] = {
	&DefaultListConfigure
};

ISPListManager::ISPListManager():
	mListNum(0),
	pParamManager(nullptr),
	pISPListConfigs(nullptr)
{
}

ISPListManager::~ISPListManager()
{
}

ISPResult ISPListManager::Init(ISPParamManager* pPM)
{
	ISPResult result = ISP_SUCCESS;

	if (pPM)
	{
		pParamManager = pPM;
	}
	else {
		result = ISP_INVALID_PARAM;
		ISPLoge("pPM is null. %d", result);
	}

	if (SUCCESS(result)) {
		pISPListConfigs = gISPListConfigs[LIST_CFG_DEFAULT];
	}

	return result;
}

ISPResult ISPListManager::CreateList(uint16_t* pRaw, uint16_t* pBGR, uint8_t* pYUV, uint8_t* pPOST, int32_t cfgIndex, int32_t *id)
{
	ISPResult result = ISP_SUCCESS;
	int32_t listId = 0;
	ISPList<uint16_t, uint16_t, uint8_t, uint8_t>* pIspList = nullptr;

	if (cfgIndex >= LIST_CFG_NUM)
	{
		result = ISP_INVALID_PARAM;
		ISPLoge("Invaled cfgIndex:%d %d", cfgIndex, result);
	}

	if (SUCCESS(result)) {
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

	if (SUCCESS(result)) {
		pIspList = new ISPList<uint16_t, uint16_t, uint8_t, uint8_t>(listId);
		if (pIspList) {
			result = pIspList->Init(pRaw, pBGR, pYUV, pPOST, pParamManager);
		}
		else {
			result = ISP_MEMORY_ERROR;
			ISPLoge("Faile to new ISPList, %d", result);
		}
	}

	if (SUCCESS(result)) {
		result = pIspList->SetListConfig(&pISPListConfigs[LIST_CFG_DEFAULT + cfgIndex]);
	}

	if (SUCCESS(result)) {
		result = pIspList->CreatISPList();
	}

	if (SUCCESS(result)) {
		mListMap.insert(make_pair(listId, pIspList));
		mISPListConfigMap.insert(make_pair(listId, cfgIndex));
		mListNum = mListMap.size();
		if (id) {
			*id = listId;
		}
	}

	return result;
}

ISPList<uint16_t, uint16_t, uint8_t, uint8_t>* ISPListManager::FindListById(int32_t id)
{
	ISPList<uint16_t, uint16_t, uint8_t, uint8_t>* pIspList = nullptr;

	if (mListMap.find(id) != mListMap.end()) {
		pIspList = mListMap.find(id)->second;
	}
	else {
		ISPLoge("Invaled index:%d", id);
	}

	return pIspList;
}

ISPResult ISPListManager::DestoryAllList()
{
	ISPResult result = ISP_SUCCESS;
	ISPList<uint16_t, uint16_t, uint8_t, uint8_t>* pIspList = nullptr;

	while (mListMap.size()) {
		pIspList = mListMap.begin()->second;
		mISPListConfigMap.erase(mISPListConfigMap.begin()->first);
		mListMap.erase(mListMap.begin()->first);
		if (pIspList) {
			delete pIspList;
		}
		else {
			result = ISP_FAILED;
			ISPLoge("key:%d -> list is null!!!", mListMap.begin()->first);
		}
	}

	mListNum = 0;

	return result;
}

ISPResult ISPListManager::DestoryListbyId(int32_t id)
{
	ISPResult result = ISP_SUCCESS;
	ISPList<uint16_t, uint16_t, uint8_t, uint8_t>* pIspList = nullptr;

	pIspList = FindListById(id);
	if (pIspList) {
		mISPListConfigMap.erase(id);
		mListMap.erase(id);
		mListNum = mListMap.size();
		delete pIspList;
	}
	else {
		result = ISP_INVALID_PARAM;
		ISPLoge("Invaled index:%d. %d", id, result);
	}

	return result;
}

ISPResult ISPListManager::StartById(int32_t id)
{
	ISPResult result = ISP_SUCCESS;
	ISPList<uint16_t, uint16_t, uint8_t, uint8_t>* pIspList = nullptr;

	pIspList = FindListById(id);
	if (pIspList) {
		result = pIspList->Process();
	}
	else {
		result = ISP_INVALID_PARAM;
		ISPLoge("Invaled index:%d. %d", id, result);
	}

	return result;
}

ISPResult ISPListManager::EnableNodebyType(int32_t id, PROCESS_TYPE type)
{
	ISPResult result = ISP_SUCCESS;
	ISPList<uint16_t, uint16_t, uint8_t, uint8_t>* pIspList = nullptr;

	pIspList = FindListById(id);
	if (pIspList) {
		pIspList->EnableNodebyType(type);
	}
	else {
		result = ISP_INVALID_PARAM;
		ISPLoge("Invaled index:%d. %d", id, result);
	}

	return result;
}

ISPResult ISPListManager::DisableNodebyType(int32_t id, PROCESS_TYPE type)
{
	ISPResult result = ISP_SUCCESS;
	ISPList<uint16_t, uint16_t, uint8_t, uint8_t>* pIspList = nullptr;

	pIspList = FindListById(id);
	if (pIspList) {
		pIspList->DisableNodebyType(type);
	}
	else {
		result = ISP_INVALID_PARAM;
		ISPLoge("Invaled index:%d. %d", id, result);
	}

	return result;
}
