// License: GPL-3.0-or-later
/*
 * ISP main function implementation.
 *
 * Copyright (c) 2019 Peng Hao <635945005@qq.com>
 */

#include "ISPCore.h"

int main(int argc, char *argv[], char *envp[])
{
	int rt = 0;

	IOInfo ioInfo = { 0 };
	ioInfo.argc = argc > MAX_IO_PARAM_CNT ? MAX_IO_PARAM_CNT : argc;
	for (int32_t i = 0; i < ioInfo.argc; i++) {
		ioInfo.argv[i] = argv[i];
		ioInfo.envp[i] = envp[i];
	}

	ISPItf* ispItf = ISPCore::GetInstance();
	if (ispItf) {
		rt = ispItf->Process(&ioInfo);
	}

	return rt;
}
