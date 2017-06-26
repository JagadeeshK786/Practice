#define _CRTDBG_MAP_ALLOC
//#include <stdlib.h>
//#include <crtdbg.h>

#include <conio.h>

#include "liuzianglib.h"
//#include "DC_Any.h"
//#include "DC_Encrypt.h"
//#include "DC_Exception.h"
#include "DC_File.h"
//#include "DC_http.h"
//#include "DC_IOCP.h"
//#include "DC_json.h"
//#include "DC_MySQL.h"
#include "DC_STR.h"
//#include "DC_ThreadPool.h"
//#include "DC_timer.h"
//#include "DC_type.h"
//#include "DC_TypeInfo.h"
#include "DC_UIBox.h"
//#include "DC_User.h"
//#include "DC_var.h"
//#include "DC_WinSock.h"
//#include "DC_SELECT.h"
//#include "DC_jsonBuilder.h"
//#include "base64\base64.h"
#include <numeric>

int main() {
	try {
		std::string filename, resource, outputfilename;
		int32_t ud = 0, ne = 0;
		DC::UIBox::SetWindow();
		system("cls");
		std::cout << "Դ�����ļ���:";
		std::cin >> filename;
		std::cout << "����(�������������2�ı���):";
		std::cin >> ud;
		std::cout << "ִ�д���(����):";
		std::cin >> ne;
		std::cout << "����������:";
		std::cin >> outputfilename;

		system("cls");
		std::cout << "Դ�����ļ���:" << filename << "\n����:" << ud << "\nִ�д���:" << ne << "\n��������:" << outputfilename << "\n��ʼִ����?(Y/N)";
		char ch(_getch());
		if (!(ch == 'Y' || ch == 'y')) exit(0);
		int32_t min = 0 - ud / 2, max = ud / 2;
		auto makesingleresult = [&min, &max](const int32_t& r) {
			return r + DC::randomer(min, max);
		};
		std::cout << "\n\n��ȡԴ����...";
		try {
			resource = DC::File::read(filename);
		}
		catch (const DC::Exception& ex) {
			std::cout << "\n�޷���ȡԴ���ݣ�������Ϣ:\"" << ex.GetTitle() << "\",\"" << ex.GetDescription() << "\"\n";
			DC::UIBox::ConBox();
			exit(0);
		}
		std::cout << "\n������...";

		std::vector<int32_t> vresource;
		std::string temp;
		for (const auto& p : resource) {
			if (p == '\n') { vresource.push_back(DC::STR::toType<int32_t>(temp)); temp.clear(); continue; }
			temp.push_back(p);
		}
		vresource.push_back(DC::STR::toType<int32_t>(temp));
		temp.clear();

		for (auto p = vresource.begin(); p != vresource.end();) {
			bool flag = true;//���ɵ�������ֻ����һ��ֵ��Դ�������
			std::vector<int32_t> line;
			auto getlineaverage = [&line]()->int32_t {
				if (line.empty()) return 0;
				return std::accumulate(line.begin(), line.end(), 0) / line.size();
			};
			for (int32_t i = 0; i < ne; i++) {
				auto srv = makesingleresult(*p);
				if (srv == *p&&flag) { flag = false; i--; continue; }
				line.emplace_back(srv);
			}

			if (getlineaverage() != *p)
				continue;
			p++;
			for (const auto& p2 : line)
				temp += DC::STR::toString(p2) + " ";
			if (!temp.empty()) temp.erase(temp.rbegin().base() - 1);
			temp.push_back('\n');
		}
		if (!temp.empty()) temp.erase(temp.rbegin().base() - 1);

		std::cout << "\n";

		while (1) {
			std::cout << "���������" << outputfilename << "...";
			if (!DC::File::write(outputfilename, temp)) {
				std::cout << "\n�޷�д���ļ�������һ���µ�����ļ���:";
				std::cin >> outputfilename;
				continue;
			}
			break;
		}
		std::cout << "\n���\n";
		DC::UIBox::ConBox();
	}
	catch (...) {
		DC::UIBox::ConBox("\n\n��������δ֪����");
		exit(0);
	}
}