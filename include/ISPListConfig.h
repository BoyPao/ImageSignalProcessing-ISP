#pragma once

#include <stdint.h>

#define NODE_NAME_MAX_SZIE 15

enum PROCESS_TYPE {
	PROCESS_BLC = 0,
	PROCESS_LSC,
	PROCESS_Demosaic,
	PROCESS_WB,
	PROCESS_CC,
	PROCESS_GAMMA,
	PROCESS_WNR,
	PROCESS_EE,
	PROCESS_TYPE_NUM
};

enum NODE_SWITCH {
	NODE_OFF = 0,
	NODE_ON
};

struct ISP_NODE_PROPERTY {
	char name[NODE_NAME_MAX_SZIE];
	PROCESS_TYPE type;
	NODE_SWITCH enable;
};

static ISP_NODE_PROPERTY DefaultNodesConfigure[] = {
	{"BLC",				PROCESS_BLC,			NODE_ON},
	{"LSC",				PROCESS_LSC,			NODE_ON},
	{"Demosaic",		PROCESS_Demosaic,		NODE_ON},
	{"WB",				PROCESS_WB,				NODE_ON},
	{"CC",				PROCESS_CC,				NODE_ON},
	{"Gamma",			PROCESS_GAMMA,			NODE_ON},
	{"WNR",				PROCESS_WNR,			NODE_ON},
	{"EE",				PROCESS_EE,				NODE_ON},
};
