#include <iostream>

class test{
    int a;
    char b;
};

class alignas(16) test_aligned{
    int a;
    char b;
};

int main(){
    std::cout<<"max allowed align size:"<<alignof(std::max_align_t)<<"\n";
    std::cout<<"sizeof: "<<sizeof(test)<<"\nalignof:"<<alignof(test);
    std::cout<<"\n";
    std::cout<<"sizeof: "<<sizeof(test_aligned)<<"\nalignof:"<<alignof(test_aligned);
    std::cout<<"\n";
}
