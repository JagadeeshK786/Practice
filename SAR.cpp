#include <iostream>
#include <type_traits>
#include <assert.h>

//违反严格别名规则
bool is_negative_ub(float x) {
	return *reinterpret_cast<unsigned int *>(&x) & 0x80000000;
}

//通过memcpy访问
bool is_negative_memcpy(float x) {
	static_assert(sizeof(float) == sizeof(unsigned int));
	unsigned int pun;
	memcpy(&pun, &x, sizeof(x));
	return pun & 0x80000000;
}

//通过union
bool is_negative_union(float x) {
	alignas(float) union { float f; unsigned int i; } pun{ x };
	return pun.i & 0x80000000;
}

void common_initial_sequence() {
	//它们都是标准布局哦
	struct A { int t; };
	struct B { int t; char zzz; };
	struct C { int t; float xxx; };

	union U { A a; B b; C c; };

	U u{ { 0 } }; //激活a
	u.b = { 1,9 }; //激活b
	int look = u.a.t; //合法
	look = u.c.t; //合法
}

int main() {
	common_initial_sequence();

	if (!is_negative_ub(2) && is_negative_ub(-1))
		std::cout << "ub version ok";
	else
		std::cout << "ub version not ok";

	std::cout << "\n";

	if (!is_negative_memcpy(2) && is_negative_memcpy(-1))
		std::cout << "memcpy version ok";
	else
		std::cout << "memcpy version not ok";

	std::cout << "\n";

	if (!is_negative_union(2) && is_negative_union(-1))
		std::cout << "union version ok";
	else
		std::cout << "union version not ok";

	std::cout << "\n";
}
