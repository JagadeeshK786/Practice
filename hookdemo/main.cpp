#include <iostream>
#include <stdlib.h>

int main() {
	free(malloc(1));
	std::cout << "end" << std::endl;
}
