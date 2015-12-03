/***************************************************************/
/*                                                             */
/*   MIPS-32 Instruction Level Simulator                       */
/*                                                             */
/*   CS311 KAIST                                               */
/*   cs311.c                                                   */
/*                                                             */
/***************************************************************/

/* !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! */
/*          DO NOT MODIFY THIS FILE!                            */
/*          You should only change hello_world.c!               */
/* !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! */

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "util.h"
#include "parse.h"
#include "run.h"

/**************************************************************/
/*                                                            */
/* Procedure : load_program                                   */
/*                                                            */
/* Purpose   : Load program and service routines into mem.    */
/*                                                            */
/**************************************************************/
void load_program(char *program_filename) {                   
    FILE *prog;
    int ii, word;
    char buffer[33];
    //to notifying data & text segment size
    int flag = 0;
    int text_index = 0;
    int data_index = 0;

    /* Open program file. */
    prog = fopen(program_filename, "r");
    if (prog == NULL) {
	printf("Error: Can't open program file %s\n", program_filename);
	exit(-1);
    }

    /* Read in the program. */
    ii = 0;

    //read 32bits + '\0' = 33
    while (fgets(buffer,33,prog) != NULL)
    {
	if(flag == 0)
	{
	    //check text segment size
	    text_size = fromBinary(buffer);
	    NUM_INST = text_size/4;
	    //initial memory allocation of text segment
	    INST_INFO = malloc(sizeof(instruction)*NUM_INST);
	    init_inst_info(NUM_INST);

	}

	else if(flag == 1)
	{
	    //check data segment size
	    data_size = fromBinary(buffer);
	}

	else
	{
	    if(ii < text_size){
		INST_INFO[text_index++] = parsing_instr(buffer, ii);
	    }
	    else if(ii < text_size + data_size){
		parsing_data(buffer, ii-text_size);
	    }
	    else
	    {
		//Do not enter this case
		//assert(0);
		//However, there is a newline in the input file
	    }
	    ii += 4;
	}
	flag++;
    }
    CURRENT_STATE.PC = MEM_TEXT_START;
    init_state_register();
  
    //printf("Read %d words from program into memory.\n\n", ii/4);
}

/************************************************************/
/*                                                          */
/* Procedure : initialize                                   */
/*                                                          */
/* Purpose   : Load machine language program                */ 
/*             and set up initial state of the machine.     */
/*                                                          */
/************************************************************/
void initialize(char *program_filename) { 
    int i;

    init_memory();
    load_program(program_filename);
    //NEXT_STATE = CURRENT_STATE;
    RUN_BIT = TRUE;
	INSTRUCTION_COUNT = 0;
	CYCLE_COUNT = 0;
}

/***************************************************************/
/*                                                             */
/* Procedure : main                                            */
/*                                                             */
/***************************************************************/
int main(int argc, char *argv[]) {                              
    char** tokens;
    int count = 1;
    int addr1 = 0;
    int addr2 = 0;
    int num_inst = 0;
    int i = 100;		//for loop

    int mem_dump_set = 0;
    int debug_set = 0;
    int num_inst_set = 0;
    int pipe_dump_set = 0;
	FORWARDING_SET = 0;
	NO_BRANCH_PREDICTION_SET = 0;

    /* Error Checking */
    if (argc < 2)
    {
	printf("Error: usage: %s [-nobp] [-f] [-m addr1:addr2] [-d] [-p] [-n num_instr] inputBinary\n", argv[0]);    
//	printf("Error: usage: %s [-m addr1:addr2] [-d] [-n num_instr] inputBinary\n", argv[0]);
	exit(1);
    }

    initialize(argv[argc-1]);

    //for checking parse result
    //print_parse_result();

    while(count != argc-1){
	if(strcmp(argv[count], "-m") == 0){
	    tokens = str_split(argv[++count],':');

	    addr1 = (int)strtol(*(tokens), NULL, 16);
	    addr2 = (int)strtol(*(tokens+1), NULL, 16);
	    mem_dump_set = 1;
	}
	else if(strcmp(argv[count], "-d") == 0)
	    debug_set = 1;
	else if(strcmp(argv[count], "-n") == 0){
	    num_inst = (int)strtol(argv[++count], NULL, 10);
	    num_inst_set = 1;
	}
	else if(strcmp(argv[count], "-p") == 0)
	    pipe_dump_set = 1;
	else if(strcmp(argv[count], "-f" == 0))
		FORWARDING_SET = 1;
	else if(strcmp(argv[count], "-nobp" == 0))
		NO_BRANCH_PREDICTION_SET = 1;
	else{
	    printf("Error: usage: %s [-nobp] [-f] [-m addr1:addr2] [-d] [-p] [-n num_instr] inputBinary\n", argv[0]);
	    //You must add nobp and f option yourself
	    exit(1);
	}
	count++;
    }

    if(num_inst_set) i = num_inst;

    if(debug_set){
	printf("Simulating for %d instructions...\n\n", i);

	while (INSTRUCTION_COUNT < i){
	    if (RUN_BIT == FALSE){
	    	printf("Simulator halted\n\n");
		break;
	    }
	    cycle();
		CYCLE_COUNT ++;
	    if(pipe_dump_set) pdump();
	    rdump();	
	    if(mem_dump_set) mdump(addr1, addr2);
	}
    }
    else{
	
	run(i, pipe_dump_set);
	rdump();
	
	printf("inst count : %d\n", INSTRUCTION_COUNT);
	if(mem_dump_set) mdump(addr1, addr2);
    }

    return 0;
}
