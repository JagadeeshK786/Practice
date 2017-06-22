#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

#include <conio.h>

#include "liuzianglib.h"
#include "DC_Any.h"
//#include "DC_Encrypt.h"
#include "DC_Exception.h"
#include "DC_File.h"
#include "DC_http.h"
#include "DC_IOCP.h"
#include "DC_json.h"
#include "DC_MySQL.h"
#include "DC_STR.h"
#include "DC_ThreadPool.h"
#include "DC_timer.h"
#include "DC_type.h"
#include "DC_TypeInfo.h"
#include "DC_UIBox.h"
#include "DC_User.h"
#include "DC_var.h"
#include "DC_WinSock.h"
#include "DC_SELECT.h"

class WinSockRAII {
public:
	WinSockRAII() {
		std::cout << "¼ÓÔØ WinSock ¿â: ";
		if (DC::WinSock::Startup(2, 2)) std::cout << "OK\n"; else std::cout << "WinSock Startup Err\n";
	}

	~WinSockRAII() {
		DC::WinSock::Cleanup();
	}
};

int main() {
	{
		WinSockRAII loadlib;
		DC::Web::Server::SELECT::Server server(2);
		server.SetListenAddr("192.168.31.154", 80);
		auto look = server.Start();
		if (look != true) std::cout << "Æô¶¯Ê§°Ü";
		Sleep(10000);
		server.Stop();
	}
	_CrtDumpMemoryLeaks();
}