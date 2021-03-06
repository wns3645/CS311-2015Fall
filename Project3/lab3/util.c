/***************************************************************/
/*                                                             */
/*   MIPS-32 Instruction Level Simulator                       */
/*                                                             */
/*   CS311 KAIST                                               */
/*   util.c                                                    */
/*                                                             */
/***************************************************************/

#include "util.h"

/***************************************************************/
/* Main memory.                                                */
/***************************************************************/

/* memory will be dynamically allocated at initialization */
mem_region_t MEM_REGIONS[] = {
    { MEM_TEXT_START, MEM_TEXT_SIZE, NULL },
    { MEM_DATA_START, MEM_DATA_SIZE, NULL },
};

#define MEM_NREGIONS (sizeof(MEM_REGIONS)/sizeof(mem_region_t))


if_id IF_ID;
id_ex ID_EX;
ex_mem EX_MEM;
mem_wb MEM_WB;
int Jump_signal;
int PCsrc;
uint32_t Jump_address;
uint32_t BR_target;
uint32_t EM_MEMBR_target;
int PCWrite;
int IF_IDWrite;
int IF_FLUSH;
int EX_MEMRegWrite;
int MEM_WBRegWrite;
int EX_MEMRegisterRd;
int MEM_WBRegisterRd;
int MEM_WBWritedata;
int MEM_WBBranch;

int FORWARDING_SET;
int NO_BRANCH_PREDICTION_SET;


/***************************************************************/
/* CPU State info.                                             */
/***************************************************************/
CPU_State CURRENT_STATE;
int RUN_BIT;		/* run bit */
int INSTRUCTION_COUNT;
int CYCLE_COUNT;

/***************************************************************/
/* CPU State info.                                             */
/***************************************************************/
instruction *INST_INFO;
int NUM_INST;

/***************************************************************/
/*                                                             */
/* Procedure: str_split                                        */
/*                                                             */
/* Purpose: To parse main function argument                    */
/*                                                             */
/***************************************************************/
char** str_split(char *a_str, const char a_delim)
{
    char** result    = 0;
    size_t count     = 0;
    char* tmp        = a_str;
    char* last_comma = 0;
    char delim[2];
    delim[0] = a_delim;
    delim[1] = 0;

    /* Count how many elements will be extracted. */
    while (*tmp)
    {
	if (a_delim == *tmp)
	{
	    count++;
	    last_comma = tmp;
	}
	tmp++;
    }

    /* Add space for trailing token. */
    count += last_comma < (a_str + strlen(a_str) - 1);

    /* Add space for terminating null string so caller
     *        knows where the list of returned strings ends. */
    count++;

    result = malloc(sizeof(char*) * count);

    if (result)
    {
	size_t idx  = 0;
	char* token = strtok(a_str, delim);

	while (token)
	{
	    assert(idx < count);
	    *(result + idx++) = strdup(token);
	    token = strtok(0, delim);
	}
	assert(idx == count - 1);
	*(result + idx) = 0;
    }

    return result;
}

/***************************************************************/
/*                                                             */
/* Procedure: fromBinary                                       */
/*                                                             */
/* Purpose: From binary to integer                             */
/*                                                             */
/***************************************************************/
int fromBinary(char *s)
{
    return (int) strtol(s, NULL, 2);
}

/***************************************************************/
/*                                                             */
/* Procedure: mem_read_32                                      */
/*                                                             */
/* Purpose: Read a 32-bit word from memory                     */
/*                                                             */
/***************************************************************/
uint32_t mem_read_32(uint32_t address)
{
    int i;
    int valid_flag = 0;

    for (i = 0; i < MEM_NREGIONS; i++) {
	if (address >= MEM_REGIONS[i].start &&
		address < (MEM_REGIONS[i].start + MEM_REGIONS[i].size)) {
	    uint32_t offset = address - MEM_REGIONS[i].start;

	    valid_flag = 1;

	    return
		(MEM_REGIONS[i].mem[offset+3] << 24) |
		(MEM_REGIONS[i].mem[offset+2] << 16) |
		(MEM_REGIONS[i].mem[offset+1] <<  8) |
		(MEM_REGIONS[i].mem[offset+0] <<  0);
	}
    }

    if (!valid_flag){
	printf("Memory Read Error: Exceed memory boundary 0x%x\n", address);
	exit(1);
    }


    return 0;
}

/***************************************************************/
/*                                                             */
/* Procedure: mem_write_32                                     */
/*                                                             */
/* Purpose: Write a 32-bit word to memory                      */
/*                                                             */
/***************************************************************/
void mem_write_32(uint32_t address, uint32_t value)
{
    int i;
    int valid_flag = 0;

    for (i = 0; i < MEM_NREGIONS; i++) {
	if (address >= MEM_REGIONS[i].start &&
		address < (MEM_REGIONS[i].start + MEM_REGIONS[i].size)) {
	    uint32_t offset = address - MEM_REGIONS[i].start;

	    valid_flag = 1;

	    MEM_REGIONS[i].mem[offset+3] = (value >> 24) & 0xFF;
	    MEM_REGIONS[i].mem[offset+2] = (value >> 16) & 0xFF;
	    MEM_REGIONS[i].mem[offset+1] = (value >>  8) & 0xFF;
	    MEM_REGIONS[i].mem[offset+0] = (value >>  0) & 0xFF;
	    return;
	}	
    }
    if(!valid_flag){
	printf("Memory Write Error: Exceed memory boundary 0x%x\n", address);
	exit(1);
    }
}

/***************************************************************/
/*                                                             */
/* Procedure : cycle                                           */
/*                                                             */
/* Purpose   : Execute a cycle                                 */
/*                                                             */
/***************************************************************/
void cycle() {                                                
    process_instruction();
}


/***************************************************************/
/*                                                             */
/* Procedure : pdump                                           */
/*                                                             */
/* Purpose   : Dump current pipeline PC state                  */   
/*                                                             */
/***************************************************************/
void pdump() {                               
    int k; 

    printf("Current pipeline PC state :\n");
    printf("-------------------------------------\n");
    printf("CYCLE %d:", CYCLE_COUNT );
    for(k = 0; k < 5; k++)
    {
    	if(CURRENT_STATE.PIPE[k])
	    printf("0x%08x", CURRENT_STATE.PIPE[k]);
	else
	    printf("          ");
	
	if( k != PIPE_STAGE - 1 )
	    printf("|");
    }
    printf("\n\n");
}

/***************************************************************/
/*                                                             */
/* Procedure : run n                                           */
/*                                                             */
/* Purpose   : Simulate MIPS for n cycles                      */
/*                                                             */
/***************************************************************/
void run(int num_cycles, int pdump_set) {
    int i;

    if (RUN_BIT == FALSE) {
	printf("Can't simulate, Simulator is halted\n\n");
	return;
    }

    printf("Simulating for %d cycles...\n\n", num_cycles);
    while(INSTRUCTION_COUNT < num_cycles) {
	if (RUN_BIT == FALSE) {
	    printf("Simulator halted\n\n");
	    break;
	}
	cycle();
	CYCLE_COUNT++;	
	if(pdump_set){
		pdump();
	}
    }
}

/***************************************************************/
/*                                                             */
/* Procedure : go                                              */
/*                                                             */
/* Purpose   : Simulate MIPS until HALTed                      */
/*                                                             */
/***************************************************************/
void go() {
    if (RUN_BIT == FALSE) {
	printf("Can't simulate, Simulator is halted\n\n");
	return;
    }

    printf("Simulating...\n\n");
    while (RUN_BIT)
	cycle();
    printf("Simulator halted\n\n");
}

/***************************************************************/ 
/*                                                             */
/* Procedure : mdump                                           */
/*                                                             */
/* Purpose   : Dump a word-aligned region of memory to the     */
/*             output file.                                    */
/*                                                             */
/***************************************************************/
void mdump(int start, int stop) {          
    int address;

    printf("Memory content [0x%08x..0x%08x] :\n", start, stop);
    printf("-------------------------------------\n");
    for (address = start; address <= stop; address += 4)
	printf("0x%08x: 0x%08x\n", address, mem_read_32(address));
    printf("\n");
}

/***************************************************************/
/*                                                             */
/* Procedure : rdump                                           */
/*                                                             */
/* Purpose   : Dump current register and bus values to the     */   
/*             output file.                                    */
/*                                                             */
/***************************************************************/
void rdump() {                               
    int k; 

    printf("Current register values :\n");
    printf("-------------------------------------\n");
    printf("PC: 0x%08x\n", CURRENT_STATE.PC);
    printf("Registers:\n");
    for (k = 0; k < MIPS_REGS; k++)
	printf("R%d: 0x%08x\n", k, CURRENT_STATE.REGS[k]);
    printf("\n");
}

/***************************************************************/
/*                                                             */
/* Procedure : init_memory                                     */
/*                                                             */
/* Purpose   : Allocate and zero memory                        */
/*                                                             */
/***************************************************************/
void init_memory() {                                           
    int i;
    for (i = 0; i < MEM_NREGIONS; i++) {
	MEM_REGIONS[i].mem = malloc(MEM_REGIONS[i].size);
	memset(MEM_REGIONS[i].mem, 0, MEM_REGIONS[i].size);
    }
}

/***************************************************************/
/*                                                             */
/* Procedure : init_inst_info                                  */
/*                                                             */
/* Purpose   : Initialize instruction info                     */
/*                                                             */
/***************************************************************/
void init_inst_info()
{
    int i;

    for(i = 0; i < NUM_INST; i++)
    {
	INST_INFO[i].value = 0;
	INST_INFO[i].opcode = 0;
	INST_INFO[i].func_code = 0;
	INST_INFO[i].r_t.r_i.rs = 0;
	INST_INFO[i].r_t.r_i.rt = 0;
	INST_INFO[i].r_t.r_i.r_i.r.rd = 0;
	INST_INFO[i].r_t.r_i.r_i.imm = 0;
	INST_INFO[i].r_t.r_i.r_i.r.shamt = 0;
	INST_INFO[i].r_t.target = 0;
    }
}

void init_state_register()
{
    
    IF_ID.Instr.value = 0;
    IF_ID.Instr.opcode = 0;
    IF_ID.Instr.func_code = 0;
    IF_ID.Instr.r_t.r_i.rs = 0;
    IF_ID.Instr.r_t.r_i.rt = 0;
    IF_ID.Instr.r_t.r_i.r_i.r.rd = 0;
    IF_ID.Instr.r_t.r_i.r_i.imm = 0;
    IF_ID.Instr.r_t.r_i.r_i.r.shamt = 0;
    IF_ID.Instr.r_t.target = 0;
    

    IF_ID.NPC = 0;
    
   
    ID_EX.REG1_data = 0;
    ID_EX.REG2_data = 0;
    ID_EX.IMM = 0 ;
    ID_EX.Rt_number = 0;
    ID_EX.Rd_number = 0;
	ID_EX.Rs_number = 0;

    ID_EX.WB_RegWrite = 0;
    ID_EX.WB_MemtoReg = 0;
    ID_EX.M_Branch = 0;
    ID_EX.M_MemRead = 0;
    ID_EX.M_MemWrite = 0;
    ID_EX.EX_RegDst = 0;
    ID_EX.EX_ALUOp = 0;
    ID_EX.EX_ALUSrc = 0;


    EX_MEM.ALU_OUT = 0;
    EX_MEM.MEM_IN = 0;
    EX_MEM.Destination_Register_number = 0;

    EX_MEM.Zero = 0;
    EX_MEM.WB_RegWrite = 0; 
    EX_MEM.WB_MemtoReg = 0;
    EX_MEM.M_Branch = 0;
    EX_MEM.M_MemRead = 0;
    EX_MEM.M_MemWrite = 0;

    MEM_WB.ALU_OUT = 0;
    MEM_WB.MEM_OUT = 0;
    MEM_WB.Destination_Register_number = 0;

    MEM_WB.WB_RegWrite = 0;
    MEM_WB.WB_MemtoReg = 0;
    MEM_WB.M_MemRead = 0;
	MEM_WB.M_Branch = 0;

	EX_MEMRegWrite = 0;
	MEM_WBRegWrite = 0;
	EX_MEMRegisterRd = 0;
	MEM_WBRegisterRd = 0;
	MEM_WBWritedata = 0;
	MEM_WBBranch = 0;


    PCsrc = 0;
    Jump_signal = 0;
    Jump_address = 0;
	PCWrite = 1;
	IF_IDWrite = 1;
    BR_target = 0;
	EX_MEM.BR_target = 0;

    IF_FLUSH = 0;
}

