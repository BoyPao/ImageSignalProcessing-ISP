// License: GPL-3.0-or-later
/*
 * I/O helper head file.
 *
 * Copyright (c) 2019 Peng Hao <635945005@qq.com>
 */

#include "Utils.h"
#include "DefaultIOCfg.h"

#define MAX_IO_PARAM_CNT 10

struct IO_INFO {
	int argc;
	char* argv[MAX_IO_PARAM_CNT];
	char* envp[MAX_IO_PARAM_CNT];
};

class IOHelper {
	public:
		IOHelper();
		virtual ~IOHelper();
		virtual int32_t Input(IO_INFO in) = 0;
		virtual int32_t CheckInput(IO_INFO ioInfo) = 0;
		virtual void HelpMenu() = 0;
		/* TODO: parse input config file */
	protected:
		void* pDynamicInfo;
		void* pStaticInfo;

	private:
};

