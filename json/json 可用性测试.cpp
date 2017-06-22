#pragma once
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>  

//#include "DC_Encrypt.h"
#include "liuzianglib.h"
#include "DC_Any.h"
#include "DC_Exception.h"
#include "DC_File.h"
#include "DC_http.h"
#include "DC_IOCP.h"
#include "DC_json.h"
//#include "DC_MySQL.h"
#include "DC_STR.h"
#include "DC_ThreadPool.h"
#include "DC_timer.h"
#include "DC_type.h"
#include "DC_TypeInfo.h"
#include "DC_UIBox.h"
#include "DC_var.h"
#include "DC_WinSock.h"

int main() {
	{
		DC::Web::json::transparent js(R"(
{
"object":{"object3":{"object":"fuck"}},"object2":{"object4":{}},
"array":[{"array":[{"object":"fuck_1"},{"object":"fuck_2"},{"object":"fuck_4"}]},{"object":"fuck2"},{"object":"fuck4"}]
}
)");
		auto obj = js.to_object();
		auto look = obj.at("object").to_object();
		auto look2 = look.at("object3").to_object();
		auto lookstr = look2.at("object").as_value().as_string();

		auto arr = obj.at("array").to_array();
		lookstr = arr[1].to_object().at("object").as_value().as_string();

		arr = obj.at("array").to_array();
		lookstr = arr[0].to_object().at("array").to_array()[0].to_object().at("object").as_value().as_string();
	}
	_CrtDumpMemoryLeaks();
}