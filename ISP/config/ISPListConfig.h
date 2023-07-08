// License: GPL-3.0-or-later
/*
 * Static ISPList configurations.
 *
 * Copyright (c) 2019 Peng Hao <635945005@qq.com>
 */

#pragma once
#include "ISPList.h"

const ISPListProperty defaultListConfig = {
	{{"BLC",			PROCESS_BLC,			NODE_ON},
	 {"LSC",			PROCESS_LSC,			NODE_ON},
	 {"Demosaic",		PROCESS_Demosaic,		NODE_ON},
	 {"WB",				PROCESS_WB,				NODE_ON},
	 {"CC",				PROCESS_CC,				NODE_ON},
	 {"Gamma",			PROCESS_GAMMA,			NODE_ON},
	 {"WNR",			PROCESS_WNR,			NODE_ON},
	 {"EE",				PROCESS_EE,				NODE_ON},
	 {"CODER",			PROCESS_CODER,			NODE_ON}}
};
