//
//  main.cpp
//  CPP Project
//
//  Created by 刘子昂 on 2017/12/12.
//  Copyright © 2017年 刘子昂. All rights reserved.
//

#include <iostream>
#include <stdio.h>

int main(int argc, const char * argv[]) {
	uint32_t buff_size = 10000000;
	using elem_t = char;

	FILE *fp = fopen("in.jpg", "rb");
	elem_t *buff = reinterpret_cast<elem_t*>(malloc(buff_size * sizeof(elem_t)));
	auto read_count = fread(buff, sizeof(elem_t), buff_size, fp);
	fclose(fp);
	fp = fopen("out.jpg", "wb");
	fwrite(buff, sizeof(elem_t), read_count, fp);
	fclose(fp);
	free(buff);
}
