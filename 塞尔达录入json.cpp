#define _CRTDBG_MAP_ALLOC
#ifdef _WIN32
#include <stdlib.h>
#include <crtdbg.h>
#endif

#include <iostream>
#include <liuzianglib.h>
#include <DC_File.h>
#include <DC_STR.h>
#include <stdexcept>
#include <initializer_list>

#include "base64.h"

struct weapon {
	std::string ChineseName, Image, ChineseIntro;
	std::string line1, line2, line3, line4;
	int number;
};

inline std::size_t findNeXTchar(const std::string& rawStr, std::size_t startpos) {//找到下一个字符，比如"name:  s"，从5开始找，找到s结束，忽略途中所有格式控制符
																				  //找不到抛异常
	if (rawStr.empty()) throw false;
	for (; startpos < rawStr.size(); startpos++)
		if (rawStr[startpos] != ' '&&rawStr[startpos] != '\n'&&rawStr[startpos] != '\r'&&rawStr[startpos] != '\t')
			return startpos;
	throw false;
}

inline std::size_t findNeXTchar(const std::string& rawStr, const char& findthis, std::size_t startpos) {//找到下一个字符
																										//找不到抛异常
	if (rawStr.empty()) throw false;
	for (; startpos < rawStr.size(); startpos++)
		if (rawStr[startpos] == findthis)
			return startpos;
	throw false;
}

inline std::size_t findNeXTchar(const std::string& rawStr,std::initializer_list<char> findthis , std::size_t startpos) {//找到下一个字符
																										//找不到抛异常
	if (rawStr.empty()) throw false;
	char *ft = reinterpret_cast<char*>(malloc(sizeof(char)*findthis.size() + 1));
	ft[findthis.size()] = 0;
	auto bgit = findthis.begin();
	for (int i = 0; i < findthis.size(); i++,bgit++) {
		ft[i] = *bgit;
	}	
	for (; startpos < rawStr.size(); startpos++) {
		bool isok = true;
		for (int ii = 0; ii < findthis.size(); ii++)
			if (ft[ii] != rawStr[startpos + ii])
				isok = false;
		if (isok)
			return startpos;
	}

	free(ft);
	throw false;
}

int main(int argc, char **argv)
{
	{
		std::string output("C:\\Users\\liuziang\\Desktop\\output\\");
		auto content = DC::File::read("C:\\Users\\liuziang\\Desktop\\weapons.md");
		auto fres = DC::STR::find(content, "\n");
		std::vector<std::string> lines;
		std::vector<weapon> weapons;
		lines.reserve(fres.places.size());
		std::size_t pos = -1;
		for (const auto& p : fres.places) {
			lines.push_back(DC::STR::getSub(content, pos, p));
			pos = p;
		}
		for (auto it = lines.begin(); it != lines.end(); it++) {
			if ((*it)[0] != '#' || (*it)[1] != '#')
				throw std::out_of_range("");
			auto fres = DC::STR::find(*it, "/");
			weapon w;
			w.number = DC::STR::toType<int>(DC::STR::getSub(*it, 2, *fres.places.begin()));
			auto cnbg = findNeXTchar(*it, ' ', 3);
			auto cned = findNeXTchar(*it, {-17,-68,-120}, cnbg + 1);
			w.ChineseName = DC::STR::getSub(*it, cnbg, cned);
			if (!Base64Encode(w.ChineseName, &w.ChineseName))
				throw std::exception();
			it += 5;
			weapons.push_back(std::move(w));
		}
	}
#ifdef _WIN32
	_CrtDumpMemoryLeaks();
#endif
}