//////////////////////////////////////////////////////////////////////////////////////
// Author: Peng Hao <635945005@qq.com>
// License: GPL
//////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////
// @file: Interfacewrapper.h
// @brief: Interface wrapper head file.
//////////////////////////////////////////////////////////////////////////////////////

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

		ISPResult Init();
		ISPResult DeInit();
		ISPResult ISPLibConfig(void* pPM, ...);
		ISPResult AlgProcess(int32_t cmd, ...);

	private:
		ISPResult LoadLib(ISP_LIBS_ID libId, const char* path);
		ISPResult ReleaseLib(ISP_LIBS_ID libId);
		ISPResult InterfaceInit(ISP_LIBS_ID libId);
		ISPResult InterfaceDeInit(ISP_LIBS_ID libId);
		ISPResult AlgInterfaceInit();
		ISP_LIBS mLibs;
		ISP_LIBS_OPS mLibsOPS;
		LIB_PARAMS mISPLibParams;
		void* pParamMgr;
};

