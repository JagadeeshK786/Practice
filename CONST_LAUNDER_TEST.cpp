#include <iostream>

struct X {
	const int n;
	const double d;
};

int main() {
	X* p = new X{7, 8.8};
	new_p = new(p) X{9710, 6.6}; //在p的位置构造一个新值
	int b = p->n;
	int c = p->n;
	double d = p->d;
	std::cout<< "old pointer:" << p->n << "  " << p->d << "\n";
	std::cout<< "new pointer:" << new_p->n << "  " << new_p->d << "\n";
}
