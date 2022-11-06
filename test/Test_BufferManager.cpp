#include "Utils.h"
#include "BufferManager.h"

int main() {
	void* gMemPool = new MemoryPool<char>;

	if (gMemPool) {
		char* p1 = static_cast<MemoryPool<char>*>(gMemPool)->RequireBuffer(1920 * 1080);
		char* p2 = static_cast<MemoryPool<char>*>(gMemPool)->RequireBuffer(1920 * 1080);
		char* p3 = static_cast<MemoryPool<char>*>(gMemPool)->RequireBuffer(1920 * 1080);
		char* p4 = static_cast<MemoryPool<char>*>(gMemPool)->RequireBuffer(1920 * 1080);
		char* p5 = static_cast<MemoryPool<char>*>(gMemPool)->RequireBuffer(1920 * 1080);
		char* p6 = static_cast<MemoryPool<char>*>(gMemPool)->RequireBuffer(1920 * 1080);
		char* p7 = static_cast<MemoryPool<char>*>(gMemPool)->RequireBuffer(4608 * 3456);
		static_cast<MemoryPool<char>*>(gMemPool)->RevertBuffer(p2);
		static_cast<MemoryPool<char>*>(gMemPool)->RevertBuffer(p4);
		static_cast<MemoryPool<char>*>(gMemPool)->RevertBuffer(p3);
		p2 = static_cast<MemoryPool<char>*>(gMemPool)->RequireBuffer(1920 * 1080);
		static_cast<MemoryPool<char>*>(gMemPool)->RevertBuffer(p2);
		static_cast<MemoryPool<char>*>(gMemPool)->RevertBuffer(p1);
		static_cast<MemoryPool<char>*>(gMemPool)->RevertBuffer(p5);
		static_cast<MemoryPool<char>*>(gMemPool)->RevertBuffer(p6);
		static_cast<MemoryPool<char>*>(gMemPool)->RevertBuffer(p7);

		delete static_cast<MemoryPool<char>*>(gMemPool);
	} else {
		ILOGE("Failed to create MemoryPool");
	}

	return 0;
}
