/***************************************************************/
/*                                                             */
/*   MIPS-32 Instruction Level Simulator                       */
/*                                                             */
/*   CS311 KAIST                                               */
/*   util.h                                                    */
/*                                                             */
/***************************************************************/

#ifndef _UTIL_H_
#define _UTIL_H_

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#define FALSE 0
#define TRUE  1

/* Basic Information */
#define MEM_TEXT_START	0x00400000
#define MEM_TEXT_SIZE	0x00100000
#define MEM_DATA_START	0x10000000
#define MEM_DATA_SIZE	0x00100000
#define MIPS_REGS	32
#define BYTES_PER_WORD	4
#define PIPE_STAGE	5

typedef struct CPU_State_Struct {
    uint32_t PC;		/* program counter */
    uint32_t REGS[MIPS_REGS];	/* register file */
    uint32_t PIPE[PIPE_STAGE];	/* pipeline stage */
} CPU_State;




typedef struct inst_s {
    short opcode;
    
    /*R-type*/
    short func_code;

    union {
        /* R-type or I-type: */
        struct {
	    unsigned char rs;
	    unsigned char rt;

	    union {
	        short imm;

	        struct {
		    unsigned char rd;
		    unsigned char shamt;
		} r;
	    } r_i;
	} r_i;
        /* J-type: */
        uint32_t target;
    } r_t;

    uint32_t value;
    
    //int32 encoding;
    //imm_expr *expr;
    //char *source_line;
} instruction;



typedef struct if_id_register {
    instruction Instr;
    uint32_t NPC;
}if_id;

typedef struct id_ex_register {
    uint32_t NPC;
    uint32_t REG1_data;
    uint32_t REG2_data;
    uint32_t IMM;
    uint32_t Rt_number;
    uint32_t Rd_number;
	uint32_t Rs_number;

    int WB_RegWrite;
    int WB_MemtoReg;
    int M_Branch;
    int M_MemRead;
    int M_MemWrite;
    int EX_RegDst;
    int EX_ALUOp;
    int EX_ALUSrc;


}id_ex;

typedef struct ex_mem_register{
    uint32_t ALU_OUT;
    uint32_t BR_TARGET;
    uint32_t MEM_IN;
    uint32_t Destination_Register_number;

    int Zero;
    int WB_RegWrite;
    int WB_MemtoReg;
    int M_Branch;
    int M_MemRead;
    int M_MemWrite;
}ex_mem;

typedef struct mem_wb_register{
    uint32_t ALU_OUT;
    uint32_t MEM_OUT;
    uint32_t Destination_Register_number;

    int WB_RegWrite;
    int WB_MemtoReg;

}mem_wb;



typedef struct {
    uint32_t start, size;
    uint8_t *mem;
} mem_region_t;


/* For -f, -nobp option passing */
extern int FORWARDING_SET;
extern int NO_BRANCH_PREDICTION_SET;


/* For State Registers */
extern if_id IF_ID;
extern id_ex ID_EX;
extern ex_mem EX_MEM;
extern mem_wb MEM_WB;

/* For Directly Used signal */
extern int PCsrc;
extern int Jump_signal;
extern uint32_t Jump_address;

/* For Control Stall instr */
extern int PCWrite;
extern int IF_IDWrite;

extern int EX_MEMRegWrite;
extern int EX_MEMRegisterRd;
extern int MEM_WBRegWrite;
extern int MEM_WBRegisterRd;


/* For PC * Registers */
extern CPU_State CURRENT_STATE;

/* For Instructions */
extern instruction *INST_INFO;
extern int NUM_INST;

/* For Memory Regions */
extern mem_region_t MEM_REGIONS[2];

/* For Execution */
extern int RUN_BIT;	/* run bit */
extern int INSTRUCTION_COUNT;
extern int CYCLE_COUNT;

/* Functions */
char**		str_split(char *a_str, const char a_delim);
int		fromBinary(char *s);
uint32_t	mem_read_32(uint32_t address);
void		mem_write_32(uint32_t address, uint32_t value);
void		cycle();
void		run(int num_cycles, int pdump_set);;
void		go();
void		mdump(int start, int stop);
void		rdump();
void		init_memory();
void		init_inst_info();
void        init_state_register();

/* YOU IMPLEMENT THIS FUNCTION */
void	process_instruction();

#endif
