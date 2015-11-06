


#include<stdio.h>

int main(){



	short imm = 0xFFFF;
	
	unsigned int rs;

	unsigned int rt = 0xfffffff;

	short msb = (imm >> 15) & 0x0001;
	if(msb == 1){
		rs = rt & (imm - 0xffff0000);
	}
	else{
		rs = rt & imm;
	}

	printf("rs : %x\n", rs);


#if 0
	if (temp == 1){
		printf("zero extension? : %x\n", imm - 0xFFFF0000);
		printf("andi : %x\n", 0xFFFFFFFF &   (imm-0xFFFF0000));
	}
	else{
		printf("imm zero extension? : %x\n", imm);
		printf("imm andi : %x\n", 0xFFFFFFFF & imm);
	}

#endif
	return 0;
}

