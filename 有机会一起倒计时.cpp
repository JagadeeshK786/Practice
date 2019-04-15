//201904: 我看你是没机会了
#include <time.h>
#include <iostream>
#include <conio.h>

int main() {
    while (true) {
        auto now = time(0);
        
        tm target{ 0 };
        target.tm_year = 118;
        target.tm_mon = 6;
        target.tm_mday = 26;
        target.tm_hour = 21;
        target.tm_min = 0;
        target.tm_sec = 0;
        
        auto distance = mktime(&target);
        
        if (distance > now) {
            distance -= now;
        }
        else {
            std::cout << "2018.7.26 9:00 pm 已经过去";
            _getch();
            std::cout << "\n";
        }
        
        target = *gmtime(&distance);
        
        std::cout << target.tm_yday << "天/" << (((double)365 - (double)target.tm_yday) / (double)365) * 100 << "%";
        _getch();
        std::cout << "\n";
    }
    
    return 0;
}
