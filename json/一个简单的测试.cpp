#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

#include <conio.h>

#include "liuzianglib.h"
#include "DC_Any.h"
//#include "DC_Encrypt.h"
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
//#include "DC_User.h"
#include "DC_var.h"
#include "DC_WinSock.h"
#include "DC_SELECT.h"

int main() {
	{
		try {
			DC::Web::json::object jsStr("{\"array\":[{\"name\":\"value\",\"name2\":\"value2\"}]}");
			auto arr = jsStr.at("array");
			auto obj = arr.to_array()[0].to_object();
			auto str = obj.at("name2").to_value().to_string();
		}
		catch (DC::Exception& ex) {
			auto ss = ex;
		}
	}
	_CrtDumpMemoryLeaks();
}