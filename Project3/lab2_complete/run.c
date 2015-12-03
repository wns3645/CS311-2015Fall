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
/* Procedure: get_inst_info                                    */
/*                                                             */
/* Purpose: Read insturction information                       */
/*                                                             */
/***************************************************************/
instruction* get_inst_info(uint32_t pc) 
{ 
    return &INST_INFO[(pc - MEM_TEXT_START) >> 2];
}

instruction get_inst(uint32_t pc)
{
	return INST_INFO[(pc - MEM_TEXT_START) >> 2];
}


/***************************************************************/
/*                                                             */
/* Procedure: process_instruction                              */
/*                                                             */
/* Purpose: Process one instrction                             */
/*                                                             */
/***************************************************************/

//IF stage 
void IF_stage(){



	CURRENT_STATE.PIPE[0] = CURRENT_STATE.PC;

	if(IF_IDWrite){
		IF_ID.NPC = CURRENT_STATE.PC + BYTES_PER_WORD;

		switch (Jump_signal)
		{

			case 1:
				IF_ID.Instr.value = 0;
				IF_ID.Instr.opcode = 0;
				IF_ID.Instr.func_code = 0;
				IF_ID.Instr.r_t.r_i.rs = 0;
				IF_ID.Instr.r_t.r_i.rt = 0;
				IF_ID.Instr.r_t.r_i.r_i.r.rd = 0;
			    IF_ID.Instr.r_t.r_i.r_i.imm = 0;
				IF_ID.Instr.r_t.r_i.r_i.r.shamt = 0;
				IF_ID.Instr.r_t.target = 0;
				break;
			case 0:
				IF_ID.Instr = get_inst(CURRENT_STATE.PC);
				break;
		}
	}

	uint32_t target1;
	uint32_t target2;
	
	switch (PCsrc)
	{
		case 1:
			target1 = EX_MEM.BR_TARGET;
			break;
		
		case 0:
			target1 = CURRENT_STATE.PC + BYTES_PER_WORD;
			break;
	}
	
	switch (Jump_signal)
	{
		case 0:
			target2 = target1;
			break;
		
		case 1:
			target2 = Jump_address;
			break;
	}

	if (PCWrite){
		CURRENT_STATE.PC = target2;
	}

}

//ID stage
void ID_stage(){


	if(PCWrite == 0) CURRENT_STATE.PIPE[1] = CURRENT_STATE.PIPE[1];
	else CURRENT_STATE.PIPE[1] = CURRENT_STATE.PIPE[0];;
	//Hazard detection

	printf("EX_MEM regwrite= = %d\n", EX_MEMRegWrite);
	printf("EX_MEM. RegisterRd= %d\n", EX_MEMRegisterRd);
	printf("ifid 의 RT = %d\n", RS(&IF_ID.Instr));
	printf("ifid 의 Rs = %d\n", RT(&IF_ID.Instr));
	printf("FORWARDING_SET = %d\n", FORWARDING_SET);
	// Data Hazard for LW
	if(ID_EX.M_MemRead && ((ID_EX.Rt_number == RT(&IF_ID.Instr)) || (ID_EX.Rt_number == RS(&IF_ID.Instr)))){
		
		printf("lw Hazard!!\n");
		ID_EX.WB_RegWrite = 0;
		ID_EX.WB_MemtoReg = 0;
		ID_EX.M_Branch = 0;
		ID_EX.M_MemRead = 0;
		ID_EX.M_MemWrite = 0;
		ID_EX.EX_RegDst = 0;
		ID_EX.EX_ALUOp = 0;
		ID_EX.EX_ALUSrc = 0;
		
		PCWrite = 0;
		IF_IDWrite = 0;
	
		return;
	}
	else if((FORWARDING_SET != 1) && EX_MEMRegWrite && (EX_MEMRegisterRd != 0) && (EX_MEMRegisterRd == RS(&IF_ID.Instr))){	
		printf("r Hazard!!\n");
		ID_EX.WB_RegWrite = 0;
		ID_EX.WB_MemtoReg = 0;
		ID_EX.M_Branch = 0;
		ID_EX.M_MemRead = 0;
		ID_EX.M_MemWrite = 0;
		ID_EX.EX_RegDst = 0;
		ID_EX.EX_ALUOp = 0;
		ID_EX.EX_ALUSrc = 0;
		
		PCWrite = 0;
		IF_IDWrite = 0;

		return;
	}
	else if((FORWARDING_SET != 1) && EX_MEMRegWrite && (EX_MEMRegisterRd != 0) && (EX_MEMRegisterRd == RT(&IF_ID.Instr))){
		printf("r Hazard!!\n");
		ID_EX.WB_RegWrite = 0;
		ID_EX.WB_MemtoReg = 0;
		ID_EX.M_Branch = 0;
		ID_EX.M_MemRead = 0;
		ID_EX.M_MemWrite = 0;
		ID_EX.EX_RegDst = 0;
		ID_EX.EX_ALUOp = 0;
		ID_EX.EX_ALUSrc = 0;
		
		PCWrite = 0;
		IF_IDWrite = 0;	

		return;
	}	
	else if((FORWARDING_SET != 1) && MEM_WBRegWrite && (MEM_WBRegisterRd != 0) && (MEM_WBRegisterRd == RS(&IF_ID.Instr))){
		printf("mem Hazard!!\n");
		ID_EX.WB_RegWrite = 0;
		ID_EX.WB_MemtoReg = 0;
		ID_EX.M_Branch = 0;
		ID_EX.M_MemRead = 0;
		ID_EX.M_MemWrite = 0;
		ID_EX.EX_RegDst = 0;
		ID_EX.EX_ALUOp = 0;
		ID_EX.EX_ALUSrc = 0;
		
		PCWrite = 0;
		IF_IDWrite = 0;		
		return;
	}
	else if((FORWARDING_SET != 1) && MEM_WBRegWrite && (MEM_WBRegisterRd != 0) && (MEM_WBRegisterRd == RT(&IF_ID.Instr))){
		printf("mem Hazard!!\n");
		ID_EX.WB_RegWrite = 0;
		ID_EX.WB_MemtoReg = 0;
		ID_EX.M_Branch = 0;
		ID_EX.M_MemRead = 0;
		ID_EX.M_MemWrite = 0;
		ID_EX.EX_RegDst = 0;
		ID_EX.EX_ALUOp = 0;
		ID_EX.EX_ALUSrc = 0;
		
		PCWrite = 0;
		IF_IDWrite = 0;		
		return;
	}


	ID_EX.NPC = IF_ID.NPC;
	ID_EX.REG1_data = CURRENT_STATE.REGS[RS(&IF_ID.Instr)];
	ID_EX.REG2_data = CURRENT_STATE.REGS[RT(&IF_ID.Instr)];
	ID_EX.IMM = IF_ID.Instr.value & 0xffff;
	
	ID_EX.Rs_number = RS(&IF_ID.Instr);
	ID_EX.Rt_number = RT(&IF_ID.Instr);
	ID_EX.Rd_number = RD(&IF_ID.Instr);

	Jump_signal = 0;
	Jump_address = 0;
	PCWrite = 1;
	printf("PCwrite = %d\n", PCWrite);
	IF_IDWrite = 1;	
	
	switch (OPCODE(&IF_ID.Instr))
    {
	case 0x9:		//(0x001001)ADDIU
		ID_EX.WB_RegWrite = 1;
		ID_EX.WB_MemtoReg = 1;
		ID_EX.M_Branch = 0;
		ID_EX.M_MemRead = 0;
		ID_EX.M_MemWrite = 0;
		ID_EX.EX_RegDst = 0;
		ID_EX.EX_ALUOp = 4;
		ID_EX.EX_ALUSrc = 1;
	    break;
	case 0xc:		//(0x001100)ANDI
	    ID_EX.WB_RegWrite = 1;
		ID_EX.WB_MemtoReg = 1;
		ID_EX.M_Branch = 0;
		ID_EX.M_MemRead = 0;
		ID_EX.M_MemWrite = 0;
		ID_EX.EX_RegDst = 0;
		ID_EX.EX_ALUOp = 5;
		ID_EX.EX_ALUSrc = 1;
	    break;
	case 0xf:		//(0x001111)LUI	
	   	ID_EX.WB_RegWrite = 1;
		ID_EX.WB_MemtoReg = 1;
		ID_EX.M_Branch = 0;
		ID_EX.M_MemRead = 0;
		ID_EX.M_MemWrite = 0;
		ID_EX.EX_RegDst = 0;
		ID_EX.EX_ALUOp = 8;
		ID_EX.EX_ALUSrc = 1;
		break;
	case 0xd:		//(0x001101)ORI
		ID_EX.WB_RegWrite = 1;
		ID_EX.WB_MemtoReg = 1;
		ID_EX.M_Branch = 0;
		ID_EX.M_MemRead = 0;
		ID_EX.M_MemWrite = 0;
		ID_EX.EX_RegDst = 0;
		ID_EX.EX_ALUOp = 6;
		ID_EX.EX_ALUSrc = 1;
	    break;
	case 0xb:		//(0x001011)SLTIU 
	    ID_EX.WB_RegWrite = 1;
		ID_EX.WB_MemtoReg = 1;
		ID_EX.M_Branch = 0;
		ID_EX.M_MemRead = 0;
		ID_EX.M_MemWrite = 0;
		ID_EX.EX_RegDst = 0;
		ID_EX.EX_ALUOp = 7;
		ID_EX.EX_ALUSrc = 1;
		break;
	case 0x23:		//(0x100011)LW	
		ID_EX.WB_RegWrite = 1;
		ID_EX.WB_MemtoReg = 0;
		ID_EX.M_Branch = 0;
		ID_EX.M_MemRead = 1;
		ID_EX.M_MemWrite = 0;
		ID_EX.EX_RegDst = 0;
		ID_EX.EX_ALUOp = 0;
		ID_EX.EX_ALUSrc = 1;
		break;
	case 0x2b:		//(0x101011)SW
		ID_EX.WB_RegWrite = 0;
		ID_EX.WB_MemtoReg = 0;
		ID_EX.M_Branch = 0;
		ID_EX.M_MemRead = 0;
		ID_EX.M_MemWrite = 1;
		ID_EX.EX_RegDst = 0;
		ID_EX.EX_ALUOp = 0;
		ID_EX.EX_ALUSrc = 1;
	    break;
	case 0x4:		//(0x000100)BEQ
		ID_EX.WB_RegWrite = 0;
		ID_EX.WB_MemtoReg = 0;
		ID_EX.M_Branch = 1;
		ID_EX.M_MemRead = 0;
		ID_EX.M_MemWrite = 0;
		ID_EX.EX_RegDst = 0;
		ID_EX.EX_ALUOp = 1;
		ID_EX.EX_ALUSrc = 0;
	    break;
	case 0x5:		//(0x000101)BNE
		ID_EX.WB_RegWrite = 0;
		ID_EX.WB_MemtoReg = 0;
		ID_EX.M_Branch = 1;
		ID_EX.M_MemRead = 0;
		ID_EX.M_MemWrite = 0;
		ID_EX.EX_RegDst = 0;
		ID_EX.EX_ALUOp = 2;
		ID_EX.EX_ALUSrc = 0;
	    break;

	case 0x0:		//(0x000000)ADDU, AND, NOR, OR, SLTU, SLL, SRL, SUBU  if JR
	    {
		ID_EX.WB_MemtoReg = 1; 
		ID_EX.M_Branch = 0;
		ID_EX.M_MemRead = 0;
		ID_EX.M_MemWrite = 0;
		ID_EX.EX_RegDst = 1;
		ID_EX.EX_ALUOp = 3;
		ID_EX.EX_ALUSrc = 0;
		if (FUNC (&IF_ID.Instr) == 0x8){ //JR
			ID_EX.WB_RegWrite = 0;
			Jump_address = CURRENT_STATE.REGS[31];
		}
		else if(FUNC (&IF_ID.Instr) == 0x00){ //first cycle
			ID_EX.WB_RegWrite = 0;
		}
		else{  // ADDU, AND, NOR, OR, SLTU, SLL, SRL, SUBU
			ID_EX.WB_RegWrite = 1;
		}
	    }
	    break;

	case 0x2:		//(0x000010)J
	   	ID_EX.WB_RegWrite = 0;
		ID_EX.WB_MemtoReg = 0;
		ID_EX.M_Branch = 0;
		ID_EX.M_MemRead = 0;
		ID_EX.M_MemWrite = 0;
		ID_EX.EX_RegDst = 0;
		ID_EX.EX_ALUOp = 0;
		ID_EX.EX_ALUSrc = 0;
		Jump_signal = 1;
		Jump_address = (IF_ID.NPC & 0xf0000000) | TARGET(&IF_ID.Instr);
		break;
	case 0x3:		//(0x000011)JAL
	    ID_EX.WB_RegWrite = 0;
		ID_EX.WB_MemtoReg = 0;
		ID_EX.M_Branch = 0;
		ID_EX.M_MemRead = 0;
		ID_EX.M_MemWrite = 0;
		ID_EX.EX_RegDst = 0;
		ID_EX.EX_ALUOp = 0;
		ID_EX.EX_ALUSrc = 0;
		Jump_signal = 1;
		Jump_address = (IF_ID.NPC & 0xF0000000) | TARGET(&IF_ID.Instr);
		CURRENT_STATE.REGS[31] = IF_ID.NPC + 4;
		break;
	}
	
}

//EX stage
void EX_stage(){

	//initialize
	EX_MEM.ALU_OUT = 0;
	EX_MEM.BR_TARGET = 0;
	EX_MEM.MEM_IN = 0;
	EX_MEM.Destination_Register_number = 0;

	EX_MEM.Zero = 0;
	EX_MEM.WB_RegWrite = 0;
	EX_MEM.WB_MemtoReg = 0;
	EX_MEM.M_Branch = 0;
	EX_MEM.M_MemRead = 0;
	EX_MEM.M_MemWrite = 0;

	if(PCWrite == 0) CURRENT_STATE.PIPE[2] = 0;
	else CURRENT_STATE.PIPE[2] = CURRENT_STATE.PIPE[1];

	// passing control signal
	EX_MEM.WB_RegWrite = ID_EX.WB_RegWrite;
	EX_MEM.WB_MemtoReg = ID_EX.WB_MemtoReg;
	EX_MEM.M_Branch = ID_EX.M_Branch;
	EX_MEM.M_MemWrite = ID_EX.M_MemWrite;
	EX_MEM.M_MemRead = ID_EX.M_MemRead;


	//passing branch target
	EX_MEM.BR_TARGET = ID_EX.NPC + (ID_EX.IMM << 2);

	

	//For ALU_OUT, case of ALUOp : 1 = beq 2 = R format 0 = lw, sw(don't use ALU) 
	int function_field = (ID_EX.IMM & 63);
	
	printf("imm : 0x%x\n", ID_EX.IMM);
	printf("aluop : ???? %d\n", ID_EX.EX_ALUOp);
	printf("function_field :/????  0x%x\n", function_field);


	uint32_t ALU_source1 = ID_EX.REG1_data;	// making sources of ALU
	uint32_t ALU_source2;
	if (ID_EX.EX_ALUSrc == 0)
		ALU_source2 = ID_EX.REG2_data;
	else if(ID_EX.EX_ALUSrc == 1)
		ALU_source2 = ID_EX.IMM;

	if (ID_EX.EX_ALUOp == 0){ //sw, lw
		EX_MEM.ALU_OUT = ALU_source1 + ALU_source2;
	}
	else if(ID_EX.EX_ALUOp == 1){	//beq
		if (ALU_source1 == ALU_source2)
			EX_MEM.Zero = 1;
	}

	else if (ID_EX.EX_ALUOp == 2){	//bne
		if (ALU_source1 != ALU_source2)
			EX_MEM.Zero = 1;
	}
	else if (ID_EX.EX_ALUOp == 4)	//addiu
		EX_MEM.ALU_OUT = ALU_source1 + ALU_source2;
	else if (ID_EX.EX_ALUOp == 5)	//andi
		EX_MEM.ALU_OUT = ALU_source1 & ALU_source2;
	else if (ID_EX.EX_ALUOp == 6)	//ori
		EX_MEM.ALU_OUT = ALU_source1 | ALU_source2;
	else if (ID_EX.EX_ALUOp == 7){	//sltiu
		if (ALU_source1 < ALU_source2)
			EX_MEM.ALU_OUT = 1;
		else
			EX_MEM.ALU_OUT = 0;
	}
	else if (ID_EX.EX_ALUOp == 8)	//lui
		EX_MEM.ALU_OUT = ALU_source2 << 16;
	else if (ID_EX.EX_ALUOp == 3){	//R-format	
		
		switch(function_field){
		    case 0x21:	//ADDU
				printf("add!!!!!!\n");
				EX_MEM.ALU_OUT = ALU_source1 + ALU_source2;
		  		break;
		   	case 0x24:	//AND
				printf("1 : %d, 2: %d\n", ALU_source1, ALU_source2);
		   		EX_MEM.ALU_OUT = ALU_source1 & ALU_source2;
				break;
		   	case 0x27:	//NOR
		   		EX_MEM.ALU_OUT = ~ (ALU_source1 | ALU_source2);
				break;
		 	case 0x25:	//OR
		 		EX_MEM.ALU_OUT = ALU_source1 | ALU_source2;
				break;
			case 0x2B:	//SLTU
				if (ALU_source1 < ALU_source2)
					EX_MEM.ALU_OUT = 1;
				else
					EX_MEM.ALU_OUT = 0;
				break;
			case 0x23:	//SUBU
				EX_MEM.ALU_OUT = ALU_source1 - ALU_source2;
				break;
		    case 0x0:	//SLL
			{
			   int shamt = ((ID_EX.IMM & 1984) >> 6);
			   if (shamt >= 0 && shamt < 32)
					EX_MEM.ALU_OUT = ALU_source2 << shamt;
			   break;
			}
			case 0x2:	//SRL
			{
			   int shamt = ((ID_EX.IMM & 1984) >> 6);
			   if (shamt >= 0 && shamt < 32)
					EX_MEM.ALU_OUT = ALU_source2 >> shamt;
			   break;
			}

		}
	}




	// passing memory write data
	EX_MEM.MEM_IN = ID_EX.REG2_data;

	//passing destination register number
	if (ID_EX.EX_RegDst == 0) {
		EX_MEM.Destination_Register_number = ID_EX.Rt_number;
	}
	else if(ID_EX.EX_RegDst == 1){
		EX_MEM.Destination_Register_number = ID_EX.Rd_number;
	}

	EX_MEMRegWrite = EX_MEM.WB_RegWrite;	
	EX_MEMRegisterRd = EX_MEM.Destination_Register_number;

}

//MEM stage
void MEM_stage(){

	//initialize
	MEM_WB.ALU_OUT = 0;
	MEM_WB.MEM_OUT = 0;
	MEM_WB.Destination_Register_number = 0;

	MEM_WB.WB_RegWrite = 0;
	MEM_WB.WB_MemtoReg = 0;

	//passing control signal
	MEM_WB.WB_RegWrite = EX_MEM.WB_RegWrite;
	MEM_WB.WB_MemtoReg = EX_MEM.WB_MemtoReg;

	CURRENT_STATE.PIPE[3] = CURRENT_STATE.PIPE[2];
	//data memory unit
	if (EX_MEM.M_MemWrite == 1)
		mem_write_32(EX_MEM.ALU_OUT, EX_MEM.MEM_IN);
	else if (EX_MEM.M_MemRead == 1){
		printf("mem alu out : %d, mem read : %d\n", EX_MEM.ALU_OUT, mem_read_32(EX_MEM.ALU_OUT));
		MEM_WB.MEM_OUT = mem_read_32(EX_MEM.ALU_OUT);
	}

	
	//passing PCsrc
	PCsrc = (EX_MEM.M_Branch & EX_MEM.Zero);

	//passing Destination register number
	MEM_WB.Destination_Register_number = EX_MEM.Destination_Register_number;
	
	// passing Read data
	MEM_WB.ALU_OUT = EX_MEM.ALU_OUT;

	MEM_WBRegisterRd = MEM_WB.Destination_Register_number;
	MEM_WBRegWrite = MEM_WB.WB_RegWrite;

}

//WB stage
void WB_stage(){

	//for register write
	uint32_t Write_Register_data;

	
	CURRENT_STATE.PIPE[4] = CURRENT_STATE.PIPE[3];

	printf("memtoreg : %d, memout ; %d, alu out : %d \n", MEM_WB.WB_MemtoReg, MEM_WB.MEM_OUT, MEM_WB.ALU_OUT);

	if (MEM_WB.WB_MemtoReg == 0)
		Write_Register_data = MEM_WB.MEM_OUT;
	else if (MEM_WB.WB_MemtoReg == 1)
		Write_Register_data = MEM_WB.ALU_OUT;

	if (MEM_WB.WB_RegWrite == 1) {
		printf("reg number = %d, data to write = %d\n", MEM_WB.Destination_Register_number, Write_Register_data);
		CURRENT_STATE.REGS[MEM_WB.Destination_Register_number] = Write_Register_data;
	}
	
	if(CURRENT_STATE.PIPE[4] != 0) INSTRUCTION_COUNT ++;


}




void process_instruction(){
    instruction *inst;
    int i;		// for loop

    /* pipeline */
//    for ( i = PIPE_STAGE - 1; i > 0; i--)
//	CURRENT_STATE.PIPE[i] = CURRENT_STATE.PIPE[i-1];
//	CURRENT_STATE.PIPE[0] = CURRENT_STATE.PC;

    //inst = get_inst_info(CURRENT_STATE.PC);
    //CURRENT_STATE.PC += BYTES_PER_WORD;
	
	WB_stage();
	MEM_stage();
	EX_stage();
	ID_stage();
	IF_stage();


    
}
