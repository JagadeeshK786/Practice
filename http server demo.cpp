#include "DC_http.h"
#include "DC_WinSock.h"

#include <conio.h>

int main() {
	if (!DC::WinSock::Startup(2, 2)) { std::cout << "startup fucked\n"; }

	SOCKET listen_s, main_s;
	DC::WinSock::SocketInit_TCP(listen_s);
	DC::WinSock::SocketInit_TCP(main_s);

	if (!DC::WinSock::Bind(listen_s, DC::WinSock::MakeAddr("127.0.0.1", 80))) { std::cout << "bind err\n"; }
	else std::cout << "bind ok\n";

	if (DC::WinSock::Listen(listen_s, 1)) std::cout << "listen ok\n"; else std::cout << "listen fucked\n";

	sockaddr_in temp;
	if (!DC::WinSock::GetConnection(main_s, listen_s, temp)) std::cout << "accept fucked\n"; else std::cout << "accept ok\n";

	std::cout << "start recv\n";

	std::string recv_s;
	DC::WinSock::Recv(main_s, recv_s, 2048);

	std::cout << "\nclient said:\n" << recv_s << "\n";
	auto request = DC::http::request_deserialization(recv_s);

	std::string replystr(R"(<html><head><title>啊哈哈</title></head><body>有机会一起写服务器</body></html>)");
	DC::http::response reply(DC::http::title(1.1, DC::http::status_codes::OK), DC::http::headers(DC::http::addHeader("Server", "liuziangexitWebServer Test"), DC::http::addHeader("Date", "2017.4.14"), DC::http::addHeader("Content-Length", DC::STR::toString(replystr.size()))), replystr);

	std::cout << "\nserver reply:\n" << reply.toStr() << "\n\n";
	if (!DC::WinSock::Send(main_s, reply.toStr())) std::cout << "reply fucked\n"; else std::cout << "reply ok";

	DC::WinSock::Cleanup();

	_getch();
}