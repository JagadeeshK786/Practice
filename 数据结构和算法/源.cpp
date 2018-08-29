#include <iostream>

void first2end(char* str, unsigned int size) {
	if (size < 2)
		return;
	char first = str[0];
	for (int i = 1; i < size; i++)
		str[i - 1] = str[i];
	str[size - 1] = first;
}

void some2end(char *str , unsigned int size, int count) {
	while (count--)
		first2end(str, size);
}

int main() {
	char hi[] = "123456";
	some2end(hi, 6, 2);
	std::cout << "";
}
