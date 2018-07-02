#include <iostream>

int test(int* pi, float* pf) {
	*pi = 9710;
	*pf = 2.0;
	return *pi;
}

int main() {
	int a = 0;
	std::cout << test(&a, reinterpret_cast<float*>(&a));
}
