#pragma once
#include <vector>
#include <winsock2.h>
#include <MSWSock.h>
#pragma comment(lib,"ws2_32.lib")
#include "DC_WinSock.h"
#include "DC_IOCP.h"
#include "DC_Any.h"
#include <stdlib.h>
#include <Windows.h>
#include <conio.h>
#include <mutex>
#include "DC_ThreadPool.h"
#include "DC_timer.h"
#include <atomic>
using namespace DC::IOCP;

class WinSockRAII {
public:
	WinSockRAII() {
		if (DC::WinSock::Startup(2, 2)) std::cout << "WinSock Startup OK\n"; else std::cout << "WinSock Startup Err\n";
	}

	~WinSockRAII() {
		DC::WinSock::Cleanup();
	}
};

void thread(std::atomic_int32_t& connectfuckNum, std::atomic_int32_t& recvfuckNum, const char* ip, const int requesttime) {
	SOCKET s;
	std::string str;
	DC::WinSock::SocketInit_TCP(s);
	
	if (!DC::WinSock::Connect(s, DC::WinSock::MakeAddr(ip, 80))) { connectfuckNum++; return; }
	DC::WinSock::SetRecvTimeOut(s, 1000);
	DC::WinSock::SetSendTimeOut(s, 1000);
	for (int i = 0; i < requesttime; i++) {
		DC::WinSock::Send(s, "GET / HTTP/1.1");
		if (!DC::WinSock::Recv(s, str, 4096)) recvfuckNum++;
	}
}

int main() {
	WinSockRAII fuck;
	std::string serverip;
	int32_t threadnum, requesttime;
	std::atomic_int32_t connectfuckNum(0), recvfuckNum(0);
	while (1) {
		system("cls");
		std::cout << "input server ip:";
		std::cin >> serverip;
		std::cout << "input thread number(concurrent client number):";
		std::cin >> threadnum;
		std::cout << "input number of requests per thread:";
		std::cin >> requesttime;
		int flag = 0;
		while (1) {
			system("cls");
			if (flag == -1) { flag = 0; break; }
			if (flag == 1) { break; }
			std::cout << "server:" << serverip << ":80\nthread number:" << threadnum << "\nnumber of requests per thread:" << requesttime << "\n\nare you sure to start?(Y/N)";
			switch (_getch()) {
			case 'Y': {flag = -1; continue; }break;
			case 'y': {flag = -1; continue; }break;
			case 'N': {flag = 1; continue; }break;
			case 'n': {flag = 1; continue; }break;
			}
		}
		if (flag != 0) continue;
		std::cout << "Starting...";
		std::unique_ptr<DC::ThreadPool> TP(new DC::ThreadPool(threadnum));
		for (int i = 0; i < threadnum; i++)
			TP->async(thread, std::ref(connectfuckNum), std::ref(recvfuckNum), serverip.c_str(), requesttime);
		DC::timer timer;
		timer.start();
		TP->start();		
		std::cout << "\nRunning...";
		TP.reset();
		timer.stop();
		system("cls");
		auto out = requesttime*threadnum - recvfuckNum.load() - connectfuckNum.load()*requesttime;
		if (out < 0) out = 0;
		std::cout << "success:" << out << "\nConnectFailed:" << connectfuckNum.load()<<"\nRecvFailed:" << recvfuckNum.load() << "\nTook " << timer.getsecond() << " seconds to complete\n\npress any key to continue...";
		_getch();
	}
}