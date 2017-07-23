#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>  

#include "БъЭЗ.h"

#define ServerNameToShow "liuziangWebServer beta 0.1, build time 2017.7.14 with liuzianglib 2.4.21V13"

void onAccept(int a) {

}

void onRecv() {

}

void onSend() {

}

void onExcept() {

}

int main() {
	{
		//auto look(sizeof(DC::WinSock::Address));
		DC::WinSock::Startup(2, 2);
		std::function<void()> func(std::bind(onAccept, 1));
		DC::Web::IOCP::Server<std::function<void()>, decltype(&onRecv), decltype(&onSend), decltype(&onExcept)> server(1, 1, "192.168.31.154", 80, func, &onRecv, &onSend, &onExcept);
		
		server.start();
		Sleep(50000);

		DC::WinSock::Cleanup();
	}
	_CrtDumpMemoryLeaks();
}