// License: GPL-3.0-or-later
/*
 * Interface wrapper head file.
 *
 * Copyright (c) 2019 Peng Hao <635945005@qq.com>
 */

#pragma once
#include "Utils.h"
#ifdef LINUX_SYSTEM
#include "../BZ/interface/LibInterface.h"
#elif defined WIN32_SYSTEM
#include "../../libbzalg/BZ/interface/LibInterface.h"
#endif

enum ALG_PROCESS_CMD {
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

enum ISP_LIBS_ID {
	ISP_ALG_LIB = 0,
	/* TODO: Add lib if need */
	ISP_LIBS_NUM
};

struct ISP_LIBS {
	void* pAlgLib;
	/* TODO: Add lib if need */
};

struct ISP_LIBS_OPS {
	LIB_OPS algOPS;
	/* TODO: Add lib if need */
};

class InterfaceWrapper {
	public:
		InterfaceWrapper();
		~InterfaceWrapper();

		int32_t Init();
		int32_t DeInit();
		int32_t ISPLibConfig(void* pPM, ...);
		int32_t AlgProcess(int32_t cmd, ...);

		int32_t NotifyMain();

	private:
		int32_t LoadLib(ISP_LIBS_ID libId, const char* path);
		int32_t ReleaseLib(ISP_LIBS_ID libId);
		int32_t InterfaceInit(ISP_LIBS_ID libId);
		int32_t InterfaceDeInit(ISP_LIBS_ID libId);
		int32_t AlgInterfaceInit();
		int32_t AlgInterfaceDeInit();
		ISP_LIBS mLibs;
		ISP_LIBS_OPS mLibsOPS;
		LIB_PARAMS mISPLibParams;
		void* pParamMgr;
};

