#include <iostream>
#include <cstddef>//for std::size_t
#include <dlfcn.h>//only works on linux?

//malloc hook
void* malloc(std::size_t s) {
	auto real_malloc = reinterpret_cast<void*(*)(std::size_t)>(dlsym(RTLD_NEXT, "malloc"));
	std::cout << "malloc has been called" << std::endl;
	return real_malloc(s);
}
