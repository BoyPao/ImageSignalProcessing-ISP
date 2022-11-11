// License: GPL-3.0-or-later
/*
 * ISP singleton head file.
 *
 * Copyright (c) 2019 Peng Hao <635945005@qq.com>
 */

#pragma once

#include "ISPCore.h"

#include <mutex>

class ISPSingleton :
	public ISPCore {
	public:
		static ISPSingleton *GetInstance();
		static int32_t ReleaseInstance();

	private:
		ISPSingleton() {};
		~ISPSingleton() {};
		ISPSingleton(const ISPSingleton &src) = delete;
		ISPSingleton &operator = (const ISPSingleton &src) = delete;

		static ISPSingleton *gISP;
		static std::mutex gLock;
		static int32_t rCnt;
};
