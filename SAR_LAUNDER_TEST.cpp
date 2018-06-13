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

int test_without_launder(int* a, float* b) {
    *a = 9710;
    *b = 2.0;
    return *a;
}

int test_with_launder(int* a, float* b) {
    *a = 9710;
    //*b = 2.0;
    *std::launder(b) = 2.0;
    //return *a;
    return *std::launder(a);
}

int main() {
    int a = 0;
    int look = test_without_launder(&a, reinterpret_cast<float*>(&a));
    std::cout << a << "\n" << look << "\n";
    
    look = test_with_launder(&a, reinterpret_cast<float*>(&a));
    std::cout << a << "\n" << look << "\n";
    std::cout << *std::launder(&a) << "\n" << look << "\n";
    
    //_getch();
}
