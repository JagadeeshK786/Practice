CXXFLAGS = -O3 -std=c++17

all: hookdemo.cpp main.cpp
		g++ -c -Wall -fPIC -g $(CXXFLAGS) hookdemo.cpp -o hookdemo.o
		g++ -shared hookdemo.o -o libhookdemo.so
		g++ main.cpp -g -ldl -lhookdemo -o main_hooked.out
		g++ main.cpp -g -o main_unhooked.out
		g++ main.cpp -g -static-libstdc++ -static-libgcc -o main_static_unhooked.out
		g++ main.cpp -g -ldl -lhookdemo -static-libstdc++ -static-libgcc -o main_static_hooked.out

install:
		cp ./libhookdemo.so /lib
		cp ./libhookdemo.so /lib64

clean:
		rm -rf *.o *.out *.a *.so
