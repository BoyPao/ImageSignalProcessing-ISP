// License: GPL-3.0-or-later
/*
 * ISP interface wrapper, supports ISP interact with other interfaces.
 *
 * Copyright (c) 2019 Peng Hao <635945005@qq.com>
 */

#include "InterfaceWrapper.h"
#include "FileManager.h"
#include "ISPCore.h"
#include "ISPConfig.h"
#ifdef LINUX_SYSTEM
#include <dlfcn.h>
#elif defined WIN32_SYSTEM
#include <WINDOWS.H>
#endif

#define ALG_DYNAMIC_LIB_PATH (WORK_PATH LIB_PATH PATH_FMT ALG_LIB_NAME DYNAMIC_LIB_FMT)

#define CALL_OPS(ops, op, params...) \
	(((ops).op) ? (ops).op(params) : ISP_FAILED)

//static ISPCallbacks gISPCallbacks;
void ISPWrapNotify(int32_t argNum, ...)
{
	NotifyData data = { 0 };
	int32_t id = 0;
	int32_t type = 0;
	va_list va;

	va_start(va, argNum);
	id = va_arg(va, int32_t);
	type = va_arg(va, int32_t);
	data.rt = va_arg(va, int32_t);
	va_end(va);

	ISPListManager::GetInstance()->NotifyList(id, type, data);
}

const char LIB_SYMBLE[LIB_FUNCS_NUM][SYMBLE_SIZE_MAX] = {
	"LibInit",
	"LibDeInit",
	"RegistCallbacks"
};

InterfaceWrapper *InterfaceWrapper::pItfW = NULL;
static mutex gInstanceLock;

InterfaceWrapper* InterfaceWrapper::GetInstance()
{
	{
		unique_lock <mutex> lock(gInstanceLock);

		if (!pItfW) {
			pItfW = new InterfaceWrapper();
			if (!pItfW) {
				ILOGE("Faild to new ITF");
				return NULL;
			}
		}

		return pItfW->IsReady() ? pItfW : NULL;
	}
}

int32_t InterfaceWrapper::RemoveInstance()
{
	int32_t rt = ISP_SUCCESS;
	static int32_t rCnt = 0;

	{
		unique_lock <mutex> lock(gInstanceLock);
		if (pItfW) {
			delete pItfW;
			pItfW = NULL;
		} else if (rCnt > 0) {
			ILOGDI("Remove ITF cnt:%d", rCnt);
		}
		rCnt++;
	}

	return rt;
}

int32_t InterfaceWrapper::IsReady()
{
	return mState;
}

InterfaceWrapper::InterfaceWrapper()
{
	mLibs = { NULL };
	mLibsOPS = { 0 };
	mISPLibParams = { 0 };
	mState = ITFWRAPPER_STATE_NOT_READY;

	if (SUCCESS(Init())) {
		mState = ITFWRAPPER_STATE_READY;
	}
}

InterfaceWrapper::~InterfaceWrapper()
{
	if (SUCCESS(DeInit())) {
		memset((void*)&mISPLibParams, 0, sizeof(BZParam));
	} else {
		ILOGE("Fail to deinit!");
	}

	mState = ITFWRAPPER_STATE_NOT_READY;
}

int32_t InterfaceWrapper::Init()
{
	int32_t rt = ISP_SUCCESS;

	rt = LoadLib(ISP_ALG_LIB, ALG_DYNAMIC_LIB_PATH);
	if (!SUCCESS(rt)) {
		ILOGE("Failed to load lib");
		return rt;
	}

	rt = InterfaceInit(ISP_ALG_LIB);

	return rt;
	/* TODO[L]: load libs if need */
}

int32_t InterfaceWrapper::DeInit()
{
	int32_t rt = ISP_SUCCESS;

	if (mState != ITFWRAPPER_STATE_READY) {
		rt = ISP_STATE_ERROR;
		ILOGE("Invalid state:%d", mState);
		return rt;
	}

	for (int32_t index = ISP_ALG_LIB; index < ISP_LIBS_NUM; index++) {
		rt = InterfaceDeInit(index);
		if (!SUCCESS(rt)) {
			ILOGE("Failed to deinit interface:%d", index);
			return rt;
		}
	}

	for (int32_t index = ISP_ALG_LIB; index < ISP_LIBS_NUM; index++) {
		rt = ReleaseLib(index);
		if (!SUCCESS(rt)) {
			ILOGE("Failed to release lib:%d", index);
			return rt;
		}
	}

	return rt;
}

int32_t InterfaceWrapper::LoadLib(int32_t libId, const char* path)
{
	int32_t rt = ISP_SUCCESS;

	void** pLib = NULL;

	switch(libId) {
		case ISP_ALG_LIB:
			pLib = &mLibs.pAlgLib;
			break;
		case ISP_LIBS_NUM:
		default:
			break;
	}

	if (!pLib) {
		rt = ISP_INVALID_PARAM;
		ILOGE("Invalid lib id:%d", libId);
		return rt;
	}

#ifdef LINUX_SYSTEM
	*pLib = dlopen(path, RTLD_LAZY);
#elif defined WIN32_SYSTEM
	int32_t length = 0;
	for (length = 0; length < FILE_PATH_MAX_SIZE; length++) {
		if (path[length] == '\0') {
			break;
		}
	}
	if (length >= FILE_PATH_MAX_SIZE - 1) {
		rt = ISP_INVALID_PARAM;
		ILOGE("Invalid lib path length:%d", length);
		return rt;
	}
	WCHAR wPath[FILE_PATH_MAX_SIZE] = { L'\0' };
	MultiByteToWideChar(CP_ACP, 0, path, length, wPath,
			length);
	*pLib = LoadLibrary(wPath);
#endif
	if (!*pLib) {
		rt = ISP_FAILED;
		ILOGE("Faild to open lib:%s", path);
		return rt;
	}
	ILOGDI("Load lib:%d %s", libId, path);

	return rt;
}

int32_t InterfaceWrapper::ReleaseLib(int32_t libId)
{
	int32_t rt = ISP_SUCCESS;

	void** pLib = NULL;

	switch(libId) {
		case ISP_ALG_LIB:
			pLib = &mLibs.pAlgLib;
			break;
		case ISP_LIBS_NUM:
		default:
			break;
	}

	if (!pLib) {
		rt = ISP_INVALID_PARAM;
		ILOGE("Invalid lib id:%d", libId);
		return rt;
	}

	if (!(*pLib)) {
		rt = ISP_FAILED;
		ILOGE("Lib not load:%d", libId);
		return rt;
	}

#ifdef LINUX_SYSTEM
	dlclose(*pLib);
#elif defined WIN32_SYSTEM
	FreeLibrary((HMODULE)*pLib);
#endif
	*pLib = NULL;
	ILOGDI("Release lib:%d", libId);

	return rt;
}

int32_t InterfaceWrapper::InterfaceInit(int32_t libId)
{
	int32_t rt = ISP_SUCCESS;

	switch(libId) {
		case ISP_ALG_LIB:
			rt = AlgInterfaceInit();
			break;
		case ISP_LIBS_NUM:
		default:
			rt = ISP_INVALID_PARAM;
			ILOGE("Invalid lib id:%d", libId);
			break;
	}

	return rt;
}

int32_t InterfaceWrapper::InterfaceDeInit(int32_t libId)
{
	int32_t rt = ISP_SUCCESS;

	switch(libId) {
		case ISP_ALG_LIB:
			rt = AlgInterfaceDeInit();
			break;
		case ISP_LIBS_NUM:
		default:
			rt = ISP_INVALID_PARAM;
			ILOGE("Invalid lib id:%d", libId);
			break;
	}

	return rt;
}

int32_t InterfaceWrapper::AlgInterfaceInit()
{
	int32_t rt = ISP_SUCCESS;
	LIB_VOID_FUNC_ADDR funcs[LIB_FUNCS_NUM] = {NULL};

	if (!mLibs.pAlgLib) {
		rt = ISP_FAILED;
		ILOGE("Wrap not init!");
		return rt;
	}

	for (int32_t i = 0; i < LIB_FUNCS_NUM; i++) {
#ifdef LINUX_SYSTEM
		funcs[i] = (LIB_VOID_FUNC_ADDR)dlsym(mLibs.pAlgLib, LIB_SYMBLE[i]);
#elif defined WIN32_SYSTEM
		funcs[i] = (LIB_VOID_FUNC_ADDR)GetProcAddress((HMODULE)mLibs.pAlgLib, LIB_SYMBLE[i]);
#endif
		ILOGDI("Lib Func[%d]:%p", i, funcs[i]);
	}

	if (!funcs[0]) {
		rt = ISP_FAILED;
		ILOGE("Lib Func[0]:%p", funcs[0]);
		return rt;
	}

	funcs[0]((void*)&mLibsOPS.algOPS);

	if (!funcs[2]) {
		rt = ISP_FAILED;
		ILOGE("Lib Func[2]:%p", funcs[2]);
		return rt;
	}

	/* TODO[L]: add callbacks if need */
	ISPCallbacks CBs = { 0 };
	CBs.ISPNotify = ISPWrapNotify;
	CBs.UtilsFuncs.Log = LogBase;
	CBs.UtilsFuncs.DumpDataInt = DumpDataInt;
	CBs.UtilsFuncs.Alloc = ISPAlloc;
	CBs.UtilsFuncs.Free = ISPFree;
	funcs[2]((void*)&CBs);

	return rt;
}

int32_t InterfaceWrapper::AlgInterfaceDeInit()
{
	int32_t rt = ISP_SUCCESS;
	LIB_VOID_FUNC_ADDR funcs[LIB_FUNCS_NUM] = {NULL};

	if (!mLibs.pAlgLib) {
		rt = ISP_FAILED;
		ILOGE("Wrap not init!");
		return rt;
	}

	memset(&mLibsOPS.algOPS, 0, sizeof(BZOps));

	for (int32_t i = 0; i < LIB_FUNCS_NUM; i++) {
#ifdef LINUX_SYSTEM
		funcs[i] = (LIB_VOID_FUNC_ADDR)dlsym(mLibs.pAlgLib, LIB_SYMBLE[i]);
#elif defined WIN32_SYSTEM
		funcs[i] = (LIB_VOID_FUNC_ADDR)GetProcAddress((HMODULE)mLibs.pAlgLib, LIB_SYMBLE[i]);
#endif
		ILOGDI("Lib Func[%d]:%p", i, funcs[i]);
	}

	if (!funcs[1]) {
		rt = ISP_FAILED;
		ILOGE("Lib Func[1]:%p", funcs[1]);
		return rt;
	}

	funcs[1](NULL);

	return rt;
}

int32_t InterfaceWrapper::AlgISPListCreate(int32_t id)
{
	int32_t rt = ISP_SUCCESS;
	uint32_t msg[MSG_NUM_MAX] = { 0 };

	msg[MSG_D0] = BZ_CMD_CREATE_PROC;
	msg[MSG_D1] = (uint32_t)id;

	rt = CALL_OPS(mLibsOPS.algOPS, BZEvent, msg);
	return rt;
}

int32_t InterfaceWrapper::AlgProcess(int32_t id, int32_t type, void *pCtrl)
{
	int32_t rt = ISP_SUCCESS;
	uint32_t msg[MSG_NUM_MAX] = { 0 };

	msg[MSG_D0] = BZ_CMD_PROCESS;
	msg[MSG_D1] = (uint32_t)id;
	msg[MSG_D2] = (uint32_t)type;
#if  __WORDSIZE ==  64
	msg[MSG_D3] = (uint64_t)pCtrl & 0xffffffff;
	msg[MSG_D4] = ((uint64_t)pCtrl >> 32) & 0xffffffff;
#elif __WORDSIZE == 32
	msg[MSG_D3] = (uint32_t)pCtrl;
#endif

	rt = CALL_OPS(mLibsOPS.algOPS, BZEvent, msg);

	return rt;
}

size_t InterfaceWrapper::GetAlgParamSize()
{
	return sizeof(BZParam);
}
