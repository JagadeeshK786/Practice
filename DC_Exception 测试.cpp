#pragma once
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>  

#include "DC_Encrypt.h"
#include "liuzianglib.h"
#include "DC_Any.h"
#include "DC_Exception.h"
#include "DC_File.h"
#include "DC_http.h"
#include "DC_IOCP.h"
#include "DC_json.h"
#include "DC_MySQL.h"
#include "DC_STR.h"
#include "DC_ThreadPool.h"
#include "DC_timer.h"
#include "DC_type.h"
#include "DC_TypeInfo.h"
#include "DC_UIBox.h"
//#include "DC_User.h"
#include "DC_var.h"
#include "DC_WinSock.h"

int main() {
	DC::Exception ex("hi","fuck");
	DC::Exception ex2(ex);
	auto look = ex2.GetDescription();
	look = ex2.GetTitle();
	ex = ex2;
	ex = std::move(ex2);
	ex2.SetTitle("xd");
	ex2.SetDescription("xd");
	_CrtDumpMemoryLeaks();
}