CXXFLAGS = -O3 -std=c++17

all: hookdemo.cpp main.cpp
		g++ -c -Wall -fPIC $(CXXFLAGS) hookdemo.cpp -o hookdemo.o
		g++ -shared -o libhookdemo.so hookdemo.o
		g++ main.cpp -lhookdemo -ldl -o main_hooked.out
		g++ main.cpp -o main_unhooked.out

install:
		cp ./libhookdemo.so /lib
		cp ./libhookdemo.so /lib64

clean:
		rm -rf *.o *.out *.a *.so
