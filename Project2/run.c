/***************************************************************/
/*                                                             */
/*   MIPS-32 Instruction Level Simulator                       */
/*                                                             */
/*   CS311 KAIST                                               */
/*   run.c                                                     */
/*                                                             */
/***************************************************************/

#include <stdio.h>

#include "util.h"
#include "run.h"

/***************************************************************/
/*                                                             */
/* Procedure: process_instruction                              */
/*                                                             */
/* Purpose: Process one instrction                             */
/*                                                             */
/***************************************************************/
void process_instruction(){

    /* Your code */
    //11.02.2015, Junhui, Taejun
	
	int inst_index = (CURRENT_STATE.PC - 0x400000) / 4;

	instruction buffer = INST_INFO[inst_index];
 	
//	instruction buffer = buffer;
	CURRENT_STATE.PC = CURRENT_STATE.PC + 4;
	
	if(CURRENT_STATE.PC >= 0x400000 + NUM_INST*4){
		RUN_BIT = FALSE;
	}


	//R format(opcode == 0), changing REGS
	//addu, subu (unsigned addition and subtracition)
	if(buffer.opcode == 0){
		if(buffer.func_code == 0x21){ //addu
			CURRENT_STATE.REGS[buffer.r_t.r_i.r_i.r.rd] = CURRENT_STATE.REGS[buffer.r_t.r_i.rs] + CURRENT_STATE.REGS[buffer.r_t.r_i.rt];
		}
		else if(buffer.func_code == 0x23){ //subu
			CURRENT_STATE.REGS[buffer.r_t.r_i.r_i.r.rd] = CURRENT_STATE.REGS[buffer.r_t.r_i.rs] - CURRENT_STATE.REGS[buffer.r_t.r_i.rt];
		}
		else if(buffer.func_code == 0x24){ //and
			CURRENT_STATE.REGS[buffer.r_t.r_i.r_i.r.rd] = (CURRENT_STATE.REGS[buffer.r_t.r_i.rs] & CURRENT_STATE.REGS[buffer.r_t.r_i.rt]);
		}
		else if(buffer.func_code == 0x25){ //or
			CURRENT_STATE.REGS[buffer.r_t.r_i.r_i.r.rd] = (CURRENT_STATE.REGS[buffer.r_t.r_i.rs] | CURRENT_STATE.REGS[buffer.r_t.r_i.rt]);
		}
		else if(buffer.func_code == 0x27){ //nor
 			CURRENT_STATE.REGS[buffer.r_t.r_i.r_i.r.rd] = ~(CURRENT_STATE.REGS[buffer.r_t.r_i.rs] | CURRENT_STATE.REGS[buffer.r_t.r_i.rt]);
		}
		else if(buffer.func_code == 0x2b){ //sltu
			if(CURRENT_STATE.REGS[buffer.r_t.r_i.rs] < CURRENT_STATE.REGS[buffer.r_t.r_i.rt]){
				CURRENT_STATE.REGS[buffer.r_t.r_i.r_i.r.rd] = 1;
			}
			else
				CURRENT_STATE.REGS[buffer.r_t.r_i.r_i.r.rd] = 0;
		}
		else if(buffer.func_code == 0x00){ //sll
			CURRENT_STATE.REGS[buffer.r_t.r_i.r_i.r.rd] = CURRENT_STATE.REGS[buffer.r_t.r_i.rt] << buffer.r_t.r_i.r_i.r.shamt;
		}
		else if(buffer.func_code == 0x02){ //srl
 			CURRENT_STATE.REGS[buffer.r_t.r_i.r_i.r.rd] = CURRENT_STATE.REGS[buffer.r_t.r_i.rt] >> buffer.r_t.r_i.r_i.r.shamt;
		}
		else if(buffer.func_code == 0x08){ //jr
			CURRENT_STATE.PC = CURRENT_STATE.REGS[buffer.r_t.r_i.rs];
		}
	}
	//I format or J format
	else{
		
		//I format, changing REGS
		if (buffer.opcode == 0xc){	// andi
			short first_bit = (buffer.r_t.r_i.r_i.imm >> 15) & (0x0001);
			if (first_bit == 1)	{	
	  			CURRENT_STATE.REGS[buffer.r_t.r_i.rt] = CURRENT_STATE.REGS[buffer.r_t.r_i.rs] & (buffer.r_t.r_i.r_i.imm - 0xFFFF0000);
			}
			else{
				CURRENT_STATE.REGS[buffer.r_t.r_i.rt] = CURRENT_STATE.REGS[buffer.r_t.r_i.rs] & buffer.r_t.r_i.r_i.imm;
			}
		}
		else if (buffer.opcode == 0x9){		//addiu
			CURRENT_STATE.REGS[buffer.r_t.r_i.rt] = CURRENT_STATE.REGS[buffer.r_t.r_i.rs] + buffer.r_t.r_i.r_i.imm ;
		
		}
		else if (buffer.opcode == 0xd){		//ori
			short first_bit = (buffer.r_t.r_i.r_i.imm >> 15) & (0x0001);
			if (first_bit == 1)	{	
	  			CURRENT_STATE.REGS[buffer.r_t.r_i.rt] = CURRENT_STATE.REGS[buffer.r_t.r_i.rs] | (buffer.r_t.r_i.r_i.imm - 0xFFFF0000);
			}
			else{
				CURRENT_STATE.REGS[buffer.r_t.r_i.rt] = CURRENT_STATE.REGS[buffer.r_t.r_i.rs] | buffer.r_t.r_i.r_i.imm;
			}
		}
		else if (buffer.opcode == 0xb){ // sltiu
			if (CURRENT_STATE.REGS[buffer.r_t.r_i.rs] < buffer.r_t.r_i.r_i.imm){
				CURRENT_STATE.REGS[buffer.r_t.r_i.rt] = 1 ;
			}
			else {
				CURRENT_STATE.REGS[buffer.r_t.r_i.rt] = 0;
			}
		}
		else if (buffer.opcode == 0xf){ // lui
			CURRENT_STATE.REGS[buffer.r_t.r_i.rt] =  buffer.r_t.r_i.r_i.imm << 16;
		}

	    //I format, changing Data Memory
		if(buffer.opcode == 0x23){ //lw
			CURRENT_STATE.REGS[buffer.r_t.r_i.rt] = mem_read_32(CURRENT_STATE.REGS[buffer.r_t.r_i.rs] + buffer.r_t.r_i.r_i.imm);
		}
		else if(buffer.opcode == 0x2b){ //sw
			mem_write_32(CURRENT_STATE.REGS[buffer.r_t.r_i.rs] + buffer.r_t.r_i.r_i.imm , CURRENT_STATE.REGS[buffer.r_t.r_i.rt]);
		}
		// branch instructions
		else if(buffer.opcode == 0x04){ //beq
			if(CURRENT_STATE.REGS[buffer.r_t.r_i.rs] == CURRENT_STATE.REGS[buffer.r_t.r_i.rt]){
				CURRENT_STATE.PC = buffer.r_t.r_i.r_i.imm * 4 + CURRENT_STATE.PC ;
			}
		}
		else if(buffer.opcode == 0x05){ //bne
			if(CURRENT_STATE.REGS[buffer.r_t.r_i.rs] != CURRENT_STATE.REGS[buffer.r_t.r_i.rt]){
				CURRENT_STATE.PC = buffer.r_t.r_i.r_i.imm * 4 + CURRENT_STATE.PC;
			}
		}

		//J format, changing PC value
	    if (buffer.opcode == 0x2){ //j
	    	CURRENT_STATE.PC = buffer.r_t.target * 4;
	    }
	    if (buffer.opcode == 0x3){ //jal
	    	
	    	CURRENT_STATE.REGS[31] = CURRENT_STATE.PC;
	    	CURRENT_STATE.PC = buffer.r_t.target * 4;

	    }
	}

}
