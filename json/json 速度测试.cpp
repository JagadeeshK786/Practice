#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

#include <conio.h>

#include "DC_File.h"
#include "DC_json.h"

int main() {
	{
		auto str = DC::File::read("test.txt");
		std::string look;

		try {
			DC::Web::json::object JSOBJ(str);
			look = JSOBJ.at("result").to_array()[10].to_object().at("weapons").to_array()[6].to_object().at("name").as_value().as_string();
			look = JSOBJ.at("result").to_array()[10].to_object().at("weapons").to_array()[6].to_object().at("name").as_value().as_string();
			look = JSOBJ.at("result").to_array()[10].to_object().at("weapons").to_array()[6].to_object().at("name").as_value().as_string();
			look = JSOBJ.at("result").to_array()[10].to_object().at("weapons").to_array()[6].to_object().at("name").as_value().as_string();
		}//1320 530 500
		catch (const std::exception& ex) {
			abort();
		}
	}
	_CrtDumpMemoryLeaks();
}