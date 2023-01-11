// License: GPL-3.0-or-later
/*
 * ISP singleton head file.
 *
 * Copyright (c) 2019 Peng Hao <635945005@qq.com>
 */

#pragma once

class ISPItf {
	public:
		virtual ~ISPItf() {};
		virtual int32_t Process(void *pInfo, ...) = 0;
		virtual bool IsActive() = 0;
		virtual bool NeedExit() = 0;
		virtual void* GetThreadParam() = 0;
};

