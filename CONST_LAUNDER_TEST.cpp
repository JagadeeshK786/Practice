#include <iostream>

struct X {
    const int n;
    const double d;
};

int main() {
    X* p = new X{7, 8.8};
    X* new_p = new(p) X{9710, 6.6};
    std::cout<< "old pointer:" << p->n << "  " << p->d << "\n";
    std::cout<< "new pointer:" << new_p->n << "  " << new_p->d << "\n";
}
