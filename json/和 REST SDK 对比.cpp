#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

#include <conio.h>

#include "DC_File.h"
#include "DC_json.h"
#include "DC_timer.h"

#include "cpprest\json.h"
#include "cpprest/http_client.h"

int main() {
	//DC::json::transparent test(R"({"name":{"name":"value"}})");
	//auto obj = test.as_object();
	std::cout << "准备测试内容中...\n";
	auto flook = DC::File::getSize("test.txt");
	auto str = DC::File::read("test.txt");
	std::string look;
	std::wstring lookw;
	DC::timer timer;
	web::http::http_response response;
	response.set_body(DC::STR::toType<std::wstring>(str));
	response.headers().set_content_type(web::http::details::mime_types::application_json);
	response.set_status_code(web::http::status_codes::OK);

	timer.start();
	try {
		auto xd = response.extract_json().get().as_object();
		lookw = xd.at(L"result").as_array()[10].as_object().at(L"weapons").as_array()[6].as_object().at(L"name").as_string();
		lookw = xd.at(L"result").as_array()[10].as_object().at(L"weapons").as_array()[6].as_object().at(L"name").as_string();
		lookw = xd.at(L"result").as_array()[10].as_object().at(L"weapons").as_array()[6].as_object().at(L"name").as_string();
		lookw = xd.at(L"result").as_array()[10].as_object().at(L"weapons").as_array()[6].as_object().at(L"name").as_string();
	}
	catch (const std::exception& ex) {
		std::cout << "\n\nREST SDK 已失败\n";
	}
	timer.stop();
	std::wcout << L"REST SDK 结果是:" << lookw;
	std::cout << "\nREST SDK 用时:" << timer.getms() << "毫秒\n";

	timer.reset();
	timer.start();
	try {
		DC::Web::json::object JSOBJ(str);
		look = JSOBJ.at("result").to_array()[10].to_object().at("weapons").to_array()[6].to_object().at("name").as_value().as_string();
		look = JSOBJ.at("result").to_array()[10].to_object().at("weapons").to_array()[6].to_object().at("name").as_value().as_string();
		look = JSOBJ.at("result").to_array()[10].to_object().at("weapons").to_array()[6].to_object().at("name").as_value().as_string();
		look = JSOBJ.at("result").to_array()[10].to_object().at("weapons").to_array()[6].to_object().at("name").as_value().as_string();
	}//1320 530 495
	catch (const std::exception& ex) {
		std::cout << "\n\nliuzianglib 已失败\n";
	}
	timer.stop();
	std::cout << "\n\nliuzianglib 结果是:" << look << "\nliuzianglib 用时:" << timer.getms() << "毫秒\n\n";
	_getch();
}