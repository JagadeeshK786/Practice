//
//  main.cpp
//  CPP Project
//
//  Created by 刘子昂 on 2017/12/12.
//  Copyright © 2017年 刘子昂. All rights reserved.
//

#include <iostream>
#include <stdio.h>

#include "DC_File.h"

int main(int argc, const char * argv[]) {
	auto file = DC::File::read<DC::File::binary>("in.jpg");
	DC::File::write<DC::File::binary>("out.jpg", file);
}
