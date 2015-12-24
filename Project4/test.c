

#include<stdio.h>
#include<stdint.h>

int main(){

	uint32_t x = 0xf0000000;

	x = x >> 4;

	printf("%x\n", x);

}
