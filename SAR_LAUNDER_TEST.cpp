#include <iostream>
#include <new>
//#include <conio.h>

#if __GNUC__ >= 7

namespace std {
    
    template <typename T>
    constexpr  T* launder(T* ptr) {
        return __builtin_launder(ptr);
    }
    
}

#else

static_assert(false, "this source only support GCC7 or higher");

#endif

int test_without_launder(int* pi, float* pf) {
    *pi = 9710;
    *pf = 2.0;
    return *pi;
}

int test_with_launder(int* pi, float* pf) {
    *pi = 9710;
    *std::launder(pf) = 2.0;//b来自于reinterpret_cast<float*>(&a)，而float和int并不是兼容类型，所以必须使用launder
    return *std::launder(pi);//使编译器重新加载pi指向的值(而非编译器认为pi应该是的值)
}

int main() {
    int a = 0;
    int look = test_without_launder(&a, reinterpret_cast<float*>(&a));
    std::cout << a << "\n" << look << "\n";
    
    look = test_with_launder(&a, reinterpret_cast<float*>(&a));
    std::cout << *std::launder(&a)/* 使编译器重新加载a指向的值 */ << "\n" << look << "\n";
    
    //_getch();
}
