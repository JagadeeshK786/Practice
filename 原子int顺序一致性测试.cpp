//
//  main.cpp
//  CPP Project
//
//  Created by 刘子昂 on 2018/1/20.
//  Copyright © 2018年 刘子昂. All rights reserved.
//

#include <iostream>
#include <atomic>
#include <thread>
#include "DC_timer.h"
using namespace std;

thread t[8];

bool test_atomic(){
    atomic<int> value(0);
    auto func=[&value]{
        //方法1-无法保证顺序一致性
        //value.store(value.load(std::memory_order_acquire)+1,std::memory_order_release);
        
        //方法2-保证顺序一致性
        //value.fetch_add(1);
        
        //方法3-保证顺序一致性
        //int tmp=value.load(memory_order_acquire);
        //while(!value.compare_exchange_weak(tmp, tmp+1));
    };
    for(int i=0;i<8;i++)
        t[i]=thread(func);
    for(int i=0;i<8;i++)
        t[i].join();
    if(value==8)
        return true;
    return false;
}

bool test_noSync(){
    int value(0);
    auto func=[&value]{
        value=value+1;
    };
    
    for(int i=0;i<8;i++)
        t[i]=thread(func);
    for(int i=0;i<8;i++)
        t[i].join();
    if(value==8)
        return true;
    return false;
}

int main(){
    DC::timer timer;
    timer.start();
    int success_count=0;
    for(int i=0;i<40000;i++)
        if(test_atomic())
        //    if(test_noSync())
            success_count++;
    timer.stop();
    
    //你会发现99.9%的开销不是数值操作，而是创建线程。。
    cout<<success_count<<"\n"<<timer.getms()/1000<<"\n";
}
