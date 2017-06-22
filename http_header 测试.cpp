#include "DC_http.h"

int main() {
	DC::http::httpSpace::header header_test;
	auto lookbool = header_test.isSetOK();
	header_test.Set("name:value");
	lookbool = header_test.isSetOK();
	
	DC::http::httpSpace::header header_test2(header_test);
	DC::http::httpSpace::header header_test3(std::move(header_test));
}