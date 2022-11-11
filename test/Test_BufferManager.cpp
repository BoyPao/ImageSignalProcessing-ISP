#include "Utils.h"
#include "BufferManager.h"

int main() {
	void* gMemPool = new MemoryPool<char>;
	ILOGI("MEM TEST");

	if (gMemPool) {
		char* pc = NULL;
		size_t size = 1920 * 1080;
		char* p1 = static_cast<MemoryPool<char>*>(gMemPool)->RequireBuffer(size);
		char* p2 = static_cast<MemoryPool<char>*>(gMemPool)->RequireBuffer(size);
		pc = p2;
		memset(pc, 1, size);
		ILOGI("memset p:%p size:%u", pc, size);
		char* p3 = static_cast<MemoryPool<char>*>(gMemPool)->RequireBuffer(size);
#if DBG_MEM_OVERWRITE_ON
		pc = p3;
		memset(pc, 1, size + 1);
		ILOGE("memset p:%p size:%u", pc, size + 1);
		pc = p3;
		memset(pc, 1, size + 3);
		ILOGE("memset p:%p size:%u", pc, size + 3);
#endif
		char* p4 = static_cast<MemoryPool<char>*>(gMemPool)->RequireBuffer(size);
#if DBG_MEM_OVERWRITE_ON
		pc = p4;
		memset(pc, 1, size + 2);
		ILOGE("memset p:%p size:%u", pc, size + 2);
#endif

		size = 4608 * 3456;
		char* p5 = static_cast<MemoryPool<char>*>(gMemPool)->RequireBuffer(size);
		static_cast<MemoryPool<char>*>(gMemPool)->RevertBuffer(p2);
		static_cast<MemoryPool<char>*>(gMemPool)->RevertBuffer(p4);
		static_cast<MemoryPool<char>*>(gMemPool)->RevertBuffer(p3);

		size = 1920 * 1080;
		p2 = static_cast<MemoryPool<char>*>(gMemPool)->RequireBuffer(size);
		static_cast<MemoryPool<char>*>(gMemPool)->RevertBuffer(p2);
		static_cast<MemoryPool<char>*>(gMemPool)->RevertBuffer(p1);
		static_cast<MemoryPool<char>*>(gMemPool)->RevertBuffer(p5);

		float* pf = NULL;
		size = 1920 * 1080;
		void* p6 = (void*)static_cast<MemoryPool<char>*>(gMemPool)->RequireBuffer(size * sizeof(float));
		pf = static_cast<float*>(p6);
		memset((void*)pf, 0.1,  size * sizeof(float));
		ILOGI("memset p:%p size:%u", pf, size * sizeof(float));

		void* p7 = (void*)static_cast<MemoryPool<char>*>(gMemPool)->RequireBuffer(size * sizeof(float));
#if DBG_MEM_OVERWRITE_ON
		pf = static_cast<float*>(p7);
		memset((void*)pf, 0.1, (size + 2) * sizeof(float));
		ILOGE("memset p:%p size:%u", pf, (size + 2) * sizeof(float));
#endif

		void* p8 = (void*)static_cast<MemoryPool<char>*>(gMemPool)->RequireBuffer(size * sizeof(float));
		static_cast<MemoryPool<char>*>(gMemPool)->RevertBuffer(static_cast<char*>(p6));
		static_cast<MemoryPool<char>*>(gMemPool)->RevertBuffer(static_cast<char*>(p7));
		static_cast<MemoryPool<char>*>(gMemPool)->RevertBuffer(static_cast<char*>(p8));

		usleep(10000);
		delete static_cast<MemoryPool<char>*>(gMemPool);
	} else {
		ILOGE("Failed to create MemoryPool");
	}

	return 0;
}
