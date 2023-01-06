// License: GPL-3.0-or-later
/*
 * Interface wrapper head file.
 *
 * Copyright (c) 2019 Peng Hao <635945005@qq.com>
 */

#pragma once
#include "Utils.h"
#include "LibInterface.h"

enum AlgProcessCmd {
	ALG_CMD_BLC = 0,
	ALG_CMD_LSC,
	ALG_CMD_DEMOSAIC,
	ALG_CMD_WB,
	ALG_CMD_CC,
	ALG_CMD_GAMMA,
	ALG_CMD_WNR,
	ALG_CMD_EE,
	/* TODO: Support more cmd */

	ALG_CMD_CTS_RAW2RGB,
	ALG_CMD_CTS_RGB2YUV,
	ALG_CMD_CTS_YUV2RGB,
	ALG_CMD_NUM
};

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

		virtual int32_t ISPLibConfig(void* pPM, ...) = 0;
		virtual int32_t AlgProcess(int32_t cmd, ...) = 0;
		virtual int32_t NotifyMain() = 0;
		virtual int32_t IsReady() = 0;
};

class InterfaceWrapper : public InterfaceWrapperBase {
	public:
		static InterfaceWrapper* GetInstance();
		virtual int32_t ISPLibConfig(void* pPM, ...);
		virtual int32_t AlgProcess(int32_t cmd, ...);
		virtual int32_t NotifyMain() { return 0; };
		virtual int32_t IsReady();

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
		void* pParamMgr;
		int32_t mState;
};

