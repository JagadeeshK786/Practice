#include <iostream>

struct X { const int n; };
union U { X x; float f; };

int main() {
	U u = { { 1 } }; //聚合初始化一个U，它的活跃成员是x，x.n为1
	X *p = new (&u.x) X{ 9710 }; //在 u.x 的位置上重新构造一个 X
	std::cout << u.x.n;
}