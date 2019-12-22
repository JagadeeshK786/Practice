cc -Wall -O0 -c main.c -o main.o
cc main.o -o main.dll
rm -rf main.o
printf "Running program...\n"
./main.dll
printf "Stopped\n"
