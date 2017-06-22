#include "liuzianglib/liuzianglib.h"
#include "liuzianglib/DC_http.h"
#include "liuzianglib/DC_ThreadPool.h"
#include "liuzianglib/DC_WinSock.h"
#include "liuzianglib/DC_File.h"
#include "liuzianglib/DC_UIBox.h"

class WinSockRAII {
public:
	WinSockRAII() {
		if (DC::WinSock::Startup(2, 2)) std::cout << "WinSock Startup OK\n"; else std::cout << "WinSock Startup Err\n";
	}

	~WinSockRAII() {
		DC::WinSock::Cleanup();
	}
};

int main() {
	WinSockRAII W;
	SOCKET s;
	DC::WinSock::SocketInit_TCP(s);
	DC::WinSock::Connect(s, DC::WinSock::MakeAddr("127.0.0.1", 80));
	int i = 0;
	while (1) {
		std::string recv;
		DC::WinSock::Send(s, "GET / HTTP/1.1");
		if (DC::WinSock::Recv(s, recv, 1024)) i++;
		std::cout << i << "\n";
		Sleep(1000);
	}
}