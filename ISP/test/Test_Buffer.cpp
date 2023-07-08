// License: GPL-3.0-or-later
/*
 * Buffer test.
 *
 * Copyright (c) 2019 Peng Hao <635945005@qq.com>
 */

#include "Utils.h"
#include "Buffer.h"

using namespace asteroidaxis::isp::resource;

#define OVERWRITE_OFFSET 0
#define TEST_NAME "BUF"

int main() {
	ILOGI("%s TEST S", TEST_NAME);

	size_t size = 1920 * 1080;
	ILOGI("size:%u", size);
	Buffer* buf = Buffer::Alloc(size);
	ILOGI("buf(%u %p)", buf->Size(), buf->Addr());

	Buffer::Free(&buf);
	ILOGI("buf:%p", buf);

	ILOGI("%s TEST E", TEST_NAME);
	return 0;
}
