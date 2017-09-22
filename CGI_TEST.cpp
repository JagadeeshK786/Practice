#include <iostream>
#include <string>
#include <stdio.h>

int main() {
        std::cout<<"Content-type: text/html\r\n\r\n";
        std::cout<<"<title>test</title>";
        std::cout<<"<h1>HOST: "<<getenv("SERVER_NAME")<<"</h1>";
        std::cout<<"CONTENT_LENGTH: "<<getenv("CONTENT_LENGTH")<<"<br>";
        std::cout<<"CONTENT_TYPE: "<<getenv("CONTENT_TYPE")<<"<br>";
        std::string post_data;
        std::cin>>post_data;
        std::cout<<"POST_DATA: "<<post_data;
	return 0;
}
