/***************************************************************/
/*                                                             */
/*   MIPS-32 Instruction Level Simulator                       */
/*                                                             */
/*   CS311 KAIST                                               */
/*   parse.c                                                   */
/*                                                             */
/***************************************************************/

#include <stdio.h>

#include "util.h"
#include "parse.h"

int text_size;
int data_size;

/**************************************************************/
/*                                                            */
/* Procedure : parsing_instr                                  */
/*                                                            */
/* Purpose   : parse binary format instrction and return the  */
/*             instrcution data                               */
/*                                                            */
/* Argument  : buffer - binary string for the instruction     */
/*             index  - order of the instruction              */
/*                                                            */
/**************************************************************/
instruction parsing_instr(const char *buffer, const int index) {
	instruction instr = {0, };


    instr.value = fromBinary((char *)buffer); 

	//R format
	char t_buf1[6];
	char t_buf2[7];

	//R-instructions have opcode '0'
	if(!strncmp(buffer, "000000", 6))
	{
		instr.opcode = 0;
		strncpy(t_buf2, &buffer[26], 6);
		t_buf2[6]=0;
		instr.func_code = fromBinary(t_buf2);

		strncpy(t_buf1, &buffer[6], 5);
		t_buf1[5] =0;
		instr.r_t.r_i.rs = fromBinary(t_buf1);

		strncpy(t_buf1, &buffer[11], 5);
		t_buf1[5] =0;
		instr.r_t.r_i.rt = fromBinary(t_buf1);

		strncpy(t_buf1, &buffer[16], 5);
		t_buf1[5] =0;
		instr.r_t.r_i.r_i.r.rd = fromBinary(t_buf1);

		strncpy(t_buf1, &buffer[21], 5);
		t_buf1[5] =0;
		instr.r_t.r_i.r_i.r.shamt = fromBinary(t_buf1);
	}

	//Make opcode value for I, J format 

	char opcode_string[7];
 	int op_value;
	strncpy(opcode_string, &buffer[0], 6);
	opcode_string[6] = 0;
	op_value = fromBinary(opcode_string);
     

	//I format
	if (op_value == 0x9 || op_value == 0xc || op_value == 0xd || op_value == 0xb || op_value == 0xf || op_value ==  0x23 || op_value ==  0x2b || op_value ==  0x4 || op_value == 0x5) {
    	
		char rs_string[6];
    	char rt_string[7];
		char imme_string[17];


 		instr.opcode = op_value;
 		

 		strncpy(rs_string, &buffer[6], 5);
 		rs_string[5] = 0;
    	instr.r_t.r_i.rs = fromBinary(rs_string);

    	strncpy(rt_string, &buffer[11], 5);
    	rt_string[5] = 0;
    	instr.r_t.r_i.rt = fromBinary(rt_string);

    	strncpy(imme_string, &buffer[16], 16);
    	imme_string[16] = 0;
    	instr.r_t.r_i.r_i.imm = fromBinary(imme_string);
    
    }

   	 //J format
   	 if (op_value == 0x2 || op_value == 0x3){

    		char target_string[27];
    
    		instr.opcode = op_value;
    		opcode_string[0] = 0;

 
    		
    		strncpy(target_string, &buffer[6], 26);
 			target_string[26]= 0;
 			instr.r_t.target = fromBinary(target_string);


   	 }



    	return instr;
}

/**************************************************************/
/*                                                            */
/* Procedure : parsing_data                                   */
/*                                                            */
/* Purpose   : parse binary format data and store data into   */
/*             the data region                                */
/*                                                            */
/* Argument  : buffer - binary string for data                */
/*             index  - order of data                         */
/*                                                            */
/**************************************************************/
void parsing_data(const char *buffer, const int index) {

	mem_write_32(0x10000000+index, fromBinary((char *)buffer));
    /* Your code */

}
