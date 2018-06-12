#include <iostream>
#include <new>

int test_without_launder(int* a, float* b){
    *a=9710;
    *b=2.0;
    return *a;
}

int test_without_launder(int& a, float& b){
    a=9710;
    b=2.0;
    return a;
}

int test_with_launder(int* a, float* b){
    *a=9710;
    //*__builtin_launder(b)=2.0;
    return *a;
}

int test_with_launder(int& a, float& b){
    a=9710;
    //*__builtin_launder(&b)=2.0;
    return a;
}

int main(){
    int a=0;
    int look=test_without_launder(&a,reinterpret_cast<float*>(&a));
    std::cout<<a<<"\n"<<look<<"\n\n";
    
    look=test_without_launder(a,reinterpret_cast<float&>(a));
    std::cout<<a<<"\n"<<look<<"\n\n";
    
    look=test_with_launder(&a,reinterpret_cast<float*>(&a));
    std::cout<<a<<"\n"<<look<<"\n\n";
    
    look=test_with_launder(a,reinterpret_cast<float&>(a));
    std::cout<<a<<"\n"<<look<<"\n\n";
    
    std::cout<<a<<"\n"<<look<<"\n\n";
}
