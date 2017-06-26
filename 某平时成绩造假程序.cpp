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
		std::cout << "源数据文件名:";
		std::cin >> filename;
		std::cout << "波动(整数并且最好是2的倍数):";
		std::cin >> ud;
		std::cout << "执行次数(整数):";
		std::cin >> ne;
		std::cout << "将结果输出到:";
		std::cin >> outputfilename;

		system("cls");
		std::cout << "源数据文件名:" << filename << "\n波动:" << ud << "\n执行次数:" << ne << "\n结果输出到:" << outputfilename << "\n开始执行吗?(Y/N)";
		char ch(_getch());
		if (!(ch == 'Y' || ch == 'y')) exit(0);
		int32_t min = 0 - ud / 2, max = ud / 2;
		auto makesingleresult = [&min, &max](const int32_t& r) {
			return r + DC::randomer(min, max);
		};
		std::cout << "\n\n读取源数据...";
		try {
			resource = DC::File::read(filename);
		}
		catch (const DC::Exception& ex) {
			std::cout << "\n无法读取源数据，错误信息:\"" << ex.GetTitle() << "\",\"" << ex.GetDescription() << "\"\n";
			DC::UIBox::ConBox();
			exit(0);
		}
		std::cout << "\n生成中...";

		std::vector<int32_t> vresource;
		std::string temp;
		for (const auto& p : resource) {
			if (p == '\n') { vresource.push_back(DC::STR::toType<int32_t>(temp)); temp.clear(); continue; }
			temp.push_back(p);
		}
		vresource.push_back(DC::STR::toType<int32_t>(temp));
		temp.clear();

		for (auto p = vresource.begin(); p != vresource.end();) {
			bool flag = true;//生成的序列中只能有一个值与源数据相等
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
			std::cout << "正在输出到" << outputfilename << "...";
			if (!DC::File::write(outputfilename, temp)) {
				std::cout << "\n无法写入文件，输入一个新的输出文件名:";
				std::cin >> outputfilename;
				continue;
			}
			break;
		}
		std::cout << "\n完成\n";
		DC::UIBox::ConBox();
	}
	catch (...) {
		DC::UIBox::ConBox("\n\n程序发生了未知错误。");
		exit(0);
	}
}