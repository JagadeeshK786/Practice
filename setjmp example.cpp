#include <iostream>
#include "userthread.h"

jmp_buf sa;
jmp_buf sb;

void tb();

void ta() {
	std::cout << "1" << std::endl;
	if (setjmp(sa) == 0)
		tb();
	std::cout << "3" << std::endl;
	if (setjmp(sa) == 0)
		longjmp(sb, 1);
}
void tb() {
	std::cout << "2" << std::endl;
	if (setjmp(sb) == 0)
		longjmp(sa, 1);
	std::cout << "4" << std::endl;
	longjmp(sa, 1);
}

int main() {
	ta();
	std::cout << "";
}