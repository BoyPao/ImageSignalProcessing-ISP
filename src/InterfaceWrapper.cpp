// License: GPL-3.0-or-later
/*
 * ISP interface wrapper, supports ISP interact with other interfaces.
 *
 * Copyright (c) 2019 Peng Hao <635945005@qq.com>
 */

#include "InterfaceWrapper.h"
#include "FileManager.h"
#ifdef LINUX_SYSTEM
#include <dlfcn.h>
#elif defined WIN32_SYSTEM
#include <WINDOWS.H>
#endif

#ifdef LINUX_SYSTEM
#define ALG_DYNAMIC_LIB_PATH "/home/penghao/HAO/test_project/ISP/ISP/lib/libbzalg.so"
#elif defined WIN32_SYSTEM
#define ALG_DYNAMIC_LIB_PATH "D:\\test_project\\ISP_NEW\\ISP_NEW\\x64\\Debug\\libbzalg.dll"
#endif

#define CALL_OPS(ops, op, params...)		\
				(((ops).op) ?				\
				({							\
					(ops).op(params);		\
					ISP_SUCCESS;			\
				})							\
				: ISP_FAILED)

//static ISP_CALLBACKS gISPCallbacks;

const char LIB_SYMBLE[LIB_FUNCS_NUM][SYMBLE_SIZE_MAX] = {
	"LibInit",
	"LibDeInit",
	"RegistCallbacks"
};

InterfaceWrapper::InterfaceWrapper()
{
	mLibs = { nullptr };
	mLibsOPS = { 0 };
	mISPLibParams = { 0 };
	pParamMgr = nullptr;
}

InterfaceWrapper::~InterfaceWrapper()
{
	ISPResult rt = ISP_SUCCESS;

	rt = DeInit();

	if (!SUCCESS(rt)) {
		ILOGE("Fail to deinit!");
	} else {
		memset((void*)&mISPLibParams, 0, sizeof(LIB_PARAMS));
		pParamMgr = nullptr;
	}
}

ISPResult InterfaceWrapper::Init()
{
	ISPResult rt = ISP_SUCCESS;

	rt = LoadLib(ISP_ALG_LIB, ALG_DYNAMIC_LIB_PATH);
	if (SUCCESS(rt)) {
		rt = InterfaceInit(ISP_ALG_LIB);
	} else {
		ILOGE("Failed to load lib");
	}
	/* TODO: load libs if need */

	return rt;
}

ISPResult InterfaceWrapper::DeInit()
{
	ISPResult rt = ISP_SUCCESS;

	for (int32_t index = ISP_ALG_LIB; index < ISP_LIBS_NUM; index++) {
		rt = InterfaceDeInit((ISP_LIBS_ID)index);
		if (!SUCCESS(rt)) {
			ILOGE("Failed to deinit interface:%d", index);
			break;
		}
	}

	for (int32_t index = ISP_ALG_LIB; index < ISP_LIBS_NUM; index++) {
		rt = ReleaseLib((ISP_LIBS_ID)index);
		if (!SUCCESS(rt)) {
			ILOGE("Failed to release lib:%d", index);
			break;
		}
	}

	return rt;
}

ISPResult InterfaceWrapper::LoadLib(ISP_LIBS_ID libId, const char* path)
{
	ISPResult rt = ISP_SUCCESS;

	void** pLib = nullptr;

	switch(libId) {
		case ISP_ALG_LIB:
			pLib = &mLibs.pAlgLib;
			break;
		case ISP_LIBS_NUM:
		default:
			rt = ISP_INVALID_PARAM;
			ILOGE("Invalid lib id:%d", libId);
	}

	if (pLib) {
#ifdef LINUX_SYSTEM
		*pLib = dlopen(path, RTLD_LAZY);
#elif defined WIN32_SYSTEM
		int32_t length = 0;
		for (length = 0; length < FILE_PATH_MAX_SIZE; length++) {
			if (path[length] == '\0') {
				break;
			}
		}
		if (length >= FILE_PATH_MAX_SIZE - 1)
		{
			rt = ISP_INVALID_PARAM;
			ILOGE("Invalid lib path length:%d", length);
		}

		if (SUCCESS(rt)) {
			WCHAR wPath[FILE_PATH_MAX_SIZE] = { L'\0' };
			MultiByteToWideChar(CP_ACP, 0, path, length, wPath,
				length);
			*pLib = LoadLibrary(wPath);
		}
#endif
		if (*pLib) {
			ILOGDI("Load lib:%d %s", libId, path);
		} else {
			rt = ISP_FAILED;
			ILOGE("Faild to open lib:%s", path);
		}
	}

	return rt;
}

ISPResult InterfaceWrapper::ReleaseLib(ISP_LIBS_ID libId)
{
	ISPResult rt = ISP_SUCCESS;

	void** pLib = nullptr;

	switch(libId) {
		case ISP_ALG_LIB:
			pLib = &mLibs.pAlgLib;
			break;
		case ISP_LIBS_NUM:
		default:
			rt = ISP_INVALID_PARAM;
			ILOGE("Invalid lib id:%d", libId);
	}

	if (pLib) {
		if (!(*pLib)) {
			rt = ISP_FAILED;
			ILOGE("Lib not load:%d", libId);
		} else {
#ifdef LINUX_SYSTEM
			dlclose(*pLib);
#elif defined WIN32_SYSTEM
			FreeLibrary((HMODULE)*pLib);
#endif
			*pLib = nullptr;
			ILOGDI("Release lib:%d", libId);
		}
	}

	return rt;
}

ISPResult InterfaceWrapper::InterfaceInit(ISP_LIBS_ID libId)
{
	ISPResult rt = ISP_SUCCESS;

	switch(libId) {
		case ISP_ALG_LIB:
			rt = AlgInterfaceInit();
			break;
		case ISP_LIBS_NUM:
		default:
			rt = ISP_INVALID_PARAM;
			ILOGE("Invalid lib id:%d", libId);
	}

	return rt;
}

ISPResult InterfaceWrapper::InterfaceDeInit(ISP_LIBS_ID libId)
{
	ISPResult rt = ISP_SUCCESS;

	switch(libId) {
		case ISP_ALG_LIB:
			rt = AlgInterfaceDeInit();
			break;
		case ISP_LIBS_NUM:
		default:
			rt = ISP_INVALID_PARAM;
			ILOGE("Invalid lib id:%d", libId);
	}

	return rt;
}

ISPResult InterfaceWrapper::AlgInterfaceInit()
{
	ISPResult rt = ISP_SUCCESS;
	LIB_VOID_FUNC_ADDR funcs[LIB_FUNCS_NUM] = {nullptr};

	if (!mLibs.pAlgLib) {
		rt = ISP_FAILED;
		ILOGE("Wrap not init!");
	}

	if (SUCCESS(rt)) {
		for (int32_t i = 0; i < LIB_FUNCS_NUM; i++) {
#ifdef LINUX_SYSTEM
			funcs[i] = (LIB_VOID_FUNC_ADDR)dlsym(mLibs.pAlgLib, LIB_SYMBLE[i]);
#elif defined WIN32_SYSTEM
			funcs[i] = (LIB_VOID_FUNC_ADDR)GetProcAddress((HMODULE)mLibs.pAlgLib, LIB_SYMBLE[i]);
#endif
			ILOGDI("Lib Func[%d]:%p", i, funcs[i]);
		}
	}

	if (SUCCESS(rt)) {
		if (funcs[0]) {
			funcs[0]((void*)&mLibsOPS.algOPS);
		} else {
			rt = ISP_FAILED;
			ILOGE("Lib Func[0]:%p", funcs[0]);
		}
	}

	if (SUCCESS(rt)) {
		if (funcs[2]) {
			//TODO: add callbacks if need
			ISP_CALLBACKS CBs;
			CBs.ISP_Notify = nullptr;
			CBs.UtilsFuncs.Log = LogBase;
			CBs.UtilsFuncs.DumpDataInt = DumpDataInt;
			funcs[2]((void*)&CBs);
		} else {
			rt = ISP_FAILED;
			ILOGE("Lib Func[2]:%p", funcs[2]);
		}
	}

	return rt;
}

ISPResult InterfaceWrapper::AlgInterfaceDeInit()
{
	ISPResult rt = ISP_SUCCESS;
	LIB_VOID_FUNC_ADDR funcs[LIB_FUNCS_NUM] = {nullptr};

	if (!mLibs.pAlgLib) {
		rt = ISP_FAILED;
		ILOGE("Wrap not init!");
	}

	if (SUCCESS(rt)) {
		memset(&mLibsOPS.algOPS, 0, sizeof(LIB_OPS));
	}

	if (SUCCESS(rt)) {
		for (int32_t i = 0; i < LIB_FUNCS_NUM; i++) {
#ifdef LINUX_SYSTEM
			funcs[i] = (LIB_VOID_FUNC_ADDR)dlsym(mLibs.pAlgLib, LIB_SYMBLE[i]);
#elif defined WIN32_SYSTEM
			funcs[i] = (LIB_VOID_FUNC_ADDR)GetProcAddress((HMODULE)mLibs.pAlgLib, LIB_SYMBLE[i]);
#endif
			ILOGDI("Lib Func[%d]:%p", i, funcs[i]);
		}
	}

	if (SUCCESS(rt)) {
		if (funcs[1]) {
			funcs[1](nullptr);
		} else {
			rt = ISP_FAILED;
			ILOGE("Lib Func[1]:%p", funcs[1]);
		}
	}

	return rt;
}

ISPResult InterfaceWrapper::ISPLibConfig(void* pPM, ...)
{
	ISPResult rt = ISP_SUCCESS;

	pParamMgr = pPM;
	if (!pParamMgr) {
		rt = ISP_INVALID_PARAM;
		ILOGE("ParamManager is null! rt:%d", rt);
	}

	if (SUCCESS(rt)) {
		ISPParamManager* tmpPM = static_cast<ISPParamManager*>(pParamMgr);
		rt = tmpPM->GetImgInfo(&mISPLibParams);
		if (!SUCCESS(rt)) {
			ILOGE("Get image info failed. rt:%d", rt);
		}
	}

	return rt;
}

ISPResult InterfaceWrapper::AlgProcess(int32_t cmd, ...)
{
	ISPResult rt = ISP_SUCCESS;
	ISPParamManager* pPM = nullptr;
	va_list va;

	pPM = static_cast<ISPParamManager*>(pParamMgr);

	if (!pPM) {
		rt = ISP_FAILED;
		ILOGE("Itf not congfig!");
	}

	if (SUCCESS(rt)) {
		rt = pPM->GetParamByCMD(&mISPLibParams, cmd);
	}

	if (SUCCESS(rt)) {
		va_start(va, cmd);
		LIB_MSG tmpMsg;
		tmpMsg.cmd = (LIB_CMD)cmd;
		tmpMsg.pSrc = static_cast<void*>(va_arg(va, void*));
		if (cmd == ALG_CMD_CTS_RAW2RGB ||
				cmd == ALG_CMD_CTS_RGB2YUV ||
				cmd == ALG_CMD_CTS_YUV2RGB) {
			tmpMsg.pDst = static_cast<void*>(va_arg(va, void*));
		} else {
			tmpMsg.pDst = nullptr;
		}
		tmpMsg.pParam = &mISPLibParams;
		tmpMsg.enable = static_cast<bool>(va_arg(va, uint32_t));
		va_end(va);

		rt = CALL_OPS(mLibsOPS.algOPS, LIB_Event, (void*)&tmpMsg);
	}

	return rt;
}
