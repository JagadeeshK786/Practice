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
#include "DC_jsonBuilder.h"

int main() {
	{
		try {
			using namespace DC::Web::jsonBuilder;
			value val("this is a string");
			value val2(true);
			value val3(getNull<value>());
			number num(9710);
			number num2(9.712);
			number num3(getNull<number>());


			DC::Web::jsonBuilder::object obj;
			obj.add("json string", val);
			obj.add("json bool", val2);
			obj.add("json null(value)", val3);

			/*//get测试
			//obj.get("");//此句引发异常
			auto& p = obj.get("json string");
			auto xdxd = p.GetName();
			xdxd = p.GetValue();
			p.SetName("new name");
			p.SetValue("new value");
			auto loooo = p.GetSeparator();
			//auto llsd = p.isSetOK();//此句不应该通过编译
			p.Set("");
			xdxd = p.GetName();
			xdxd = p.GetValue();
			*/

			/*//erase测试
			auto boollook = obj.erase("json bool");
			boollook = obj.erase("json null(value)");
			boollook = obj.erase("json null(value)");
			*/

			DC::Web::jsonBuilder::object obj2;
			obj2.add("json int", num);
			obj2.add("json double", num2);
			obj2.add("json null(number)", num3);
			obj2.add("object", obj);
			auto tostr = obj2.toString();

			array array1;
			array1.add("object", obj);
			array1.add("object", obj);
			array1.add("object", obj2);
			array1.add("object", obj);
			tostr = array1.toString();

			/*//拷贝构造和=运算符测试
			array testcopy(std::move(array1));
			array1 = std::move(testcopy);
			testcopy = array1;
			*/

			obj.clear();
			obj.add("array", array1);
			tostr = obj.toString();

			DC::Web::json::object deobj(tostr);
			auto lookjstr = deobj.at("array").to_array().at(2).to_object().at("object").to_object().at("json bool").to_value().as_bool();
			std::cout << lookjstr;
		}
		catch (DC::Exception& ex) {
			auto ss = ex;
		}
	}
	_CrtDumpMemoryLeaks();
}