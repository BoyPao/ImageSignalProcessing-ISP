// License: GPL-3.0-or-later
/*
 * Interface wrapper head file.
 *
 * Copyright (c) 2019 Peng Hao <635945005@qq.com>
 */

#pragma once
#include "Utils.h"
#include "BZInterface.h"

enum ISPLibsId {
	ISP_ALG_LIB = 0,
	/* TODO: Add lib if need */
	ISP_LIBS_NUM
};

struct ISPLibs {
	void* pAlgLib;
	/* TODO: Add lib if need */
};

struct ISPLibsOps {
	BZOps algOPS;
	/* TODO: Add lib if need */
};

enum ItfWrapperState {
	ITFWRAPPER_STATE_NOT_READY = 0,
	ITFWRAPPER_STATE_READY
};

class InterfaceWrapperBase {
	public:
		~InterfaceWrapperBase() {};

		virtual int32_t AlgISPListCreate(int32_t id) = 0;
		virtual int32_t AlgProcess(int32_t id, int32_t type, void *pCtrl) = 0;
		virtual int32_t NotifyMain() = 0;
		virtual int32_t IsReady() = 0;
		virtual size_t GetAlgParamSize() = 0;
};

class InterfaceWrapper : public InterfaceWrapperBase {
	public:
		static InterfaceWrapper* GetInstance();
		virtual int32_t AlgISPListCreate(int32_t id);
		virtual int32_t AlgProcess(int32_t id, int32_t type, void *pCtrl);
		virtual int32_t NotifyMain() { return 0; };
		virtual int32_t IsReady();
		virtual size_t GetAlgParamSize();

	private:
		InterfaceWrapper();
		virtual ~InterfaceWrapper();
		int32_t Init();
		int32_t DeInit();
		int32_t LoadLib(int32_t libId, const char* path);
		int32_t ReleaseLib(int32_t libId);
		int32_t InterfaceInit(int32_t libId);
		int32_t InterfaceDeInit(int32_t libId);
		int32_t AlgInterfaceInit();
		int32_t AlgInterfaceDeInit();
		ISPLibs mLibs;
		ISPLibsOps mLibsOPS;
		BZParam mISPLibParams;
		int32_t mState;
};

