#define _CRTDBG_MAP_ALLOC
#ifdef _WIN32
#include <stdlib.h>
#include <crtdbg.h>
#endif

#include <iostream>
#include <liuzianglib.h>
#include <DC_File.h>
#include <DC_STR.h>

#include "base64.h"

int main(int argc, char **argv)
{
	{
		auto pars = DC::GetCommandLineParameters(argc, argv);
		if (pars.size()<4) {
			std::cout << "Usage: base64 encode/decode filename output";
			return 0;
		}
		if (pars[1] != "encode"&&pars[1] != "decode") {
			std::cout << "Usage: base64 encode/decode filename output";
			return 0;
		}

		auto content = DC::File::read(pars[2]);
		auto fres = DC::STR::find(content, "\n");
		std::vector<std::string> lines;
		lines.reserve(fres.places.size());
		std::size_t pos = -1;
		for (const auto& p : fres.places) {
			lines.push_back(DC::STR::getSub(content, pos, p));
			pos = p;
		}
		std::string output;
		for (auto& p : lines) {
			bool result = false;
			if (pars[1] == "encode")
				result = Base64Encode(p, &p);
			else
				result = Base64Decode(p, &p);
			if (!result)
				std::cout << p << "base64 failed\n";
			output += p + "\n";
		}
		if (!DC::File::write(pars[3], output))
			std::cout << "write err";
	}
#ifdef _WIN32
	_CrtDumpMemoryLeaks();
#endif
}