#include "DC_http.h"

#include <conio.h>

int main() {
	DC::http::request http_request(DC::http::title(1.1, "GET /index.html"), DC::http::headers(DC::http::addHeader("Connection", "Keep-Alive"), DC::http::addHeader("Accept-Language:zh-cn"), DC::http::addHeader("User-Agent:Mozila/4.0(compatible;MSIE5.01;Window NT5.0)")), DC::http::body("hi,this is body"));
	DC::http::response http_response(DC::http::title(1.1, "200 OK"), DC::http::headers(DC::http::addHeader("Server", "liuziangexitWebServer"), DC::http::addHeader("Content-Length", "15")), DC::http::body("hi,this is body"));
	std::cout << http_request.toStr();
	std::cout << "\n\n\n·Ö¸ô\n";
	std::cout << http_response.toStr();
	_getch();
}