#include "DC_http.h"

int main() {
	DC::http::httpSpace::headers h1;
	h1.data().emplace_back("Connection:Keep-Alive");
	h1.data().emplace_back("Accept-Language:zh-cn");
	h1.data().emplace_back("User-Agent:Mozila/4.0(compatible;MSIE5.01;Window NT5.0)");
	DC::http::httpSpace::headers h2(h1);
	DC::http::httpSpace::headers h3(std::move(h1));
	auto look = h3.toStr();
	std::cout << look;
}