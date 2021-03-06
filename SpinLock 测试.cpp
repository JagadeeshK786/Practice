#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h> 
#include <conio.h>

#include <iostream>
#include <Windows.h>
#include <vector>
#include <mutex>
#include <atomic>
#include <type_traits>

#include "可能这就是手动管理内存吧.h"

void dosth()noexcept {}

int main() {
	{
		SpinLock flag;
		int addthis = 0;
		auto func = [&flag, &addthis]()noexcept {
			flag.lock();
			std::cout << "已获得锁\n";

			addthis++;
			std::cout << addthis << "\n";
			Sleep(100);
			std::cout << "释放锁\n";
			flag.unlock();
		};

		std::thread t[10];
		for (int i = 0; i < 10; i++)
			t[i] = std::thread(func);
		for (int i = 0; i < 10; i++)
			t[i].join();
		_getch();
	}
	_CrtDumpMemoryLeaks();
}