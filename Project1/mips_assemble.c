// CS311 Computer Organiztion, KAIST


// Team 17
// 20130189 Taejun Kim
// 20130273 Junhui Park

// Term Project 1
// MIPS-Assembler
// 2015.10.03


#include <stdio.h> // standard C library
#include <stdlib.h>  
#include <string.h> // for string functions

//define MACRO
#define TRUE 1
#define FALSE 0

// for making array of labels(label table)
typedef struct label{
		char name[127];
		int addr;
}LABEL;


//fucntion definition
void inst_convert(char* buff, char* answer, LABEL* data_table, int data_index, LABEL* text_table, int text_index, int PC); // parse instruction lines
void int_binary(int num, char *str, int size);  // convert decimal integer to binary string


//main
int main(int argc, char* argv[]){
		FILE *fp;   // file pointer
		FILE *fp_out; // output file
		char buff[255]; // contents of .s file
		char* temp; // temporary buffer
		char temp_write[65];

		char text[] = ".text"; // directive for text section
		char data[] = ".data"; // directive for data section
		char word[] = ".word"; // directive for word
	
		int PC = 0;

		// size of data and text section
		int data_size = 0;
		int text_size = 0;

		// the number of labels of data and text section
		int data_label = 0;
		int text_label = 0;

		int is_data = FALSE;
		int is_text = FALSE; 

		

		//file open
		fp = fopen(argv[1], "r");


		if( fp != NULL)
		{		
				// check the number of labels and size
				while(!feof(fp))
				{
						fgets(buff, sizeof(buff), fp);
						if(strstr(buff, data) != NULL)
						{
								is_data = TRUE;
								is_text = FALSE;
								continue;
						}
						else if(strstr(buff, text) != NULL)
						{
								is_data = FALSE;
								is_text = TRUE;
								continue;
						}
						
						//check data section
						if(is_data)
						{
								if(strstr(buff, ":") != NULL)
										data_label++;
								if(strstr(buff, word) != NULL)
										data_size++;
						}
						//check text section
						else if(is_text)
						{
								if(strstr(buff, ":") != NULL)
										text_label++;
								else
										text_size++;
						}
				}

				text_size--; //because of feof(), text_size has one more count.

				fseek(fp, 0, SEEK_SET);
				is_text = FALSE;
				is_data = FALSE;

				//make a label struct point array;
				LABEL data_table[data_label];
				LABEL text_table[text_label];

				int temp_index = 0;
				int temp_PC = -1;

				char value[32*data_size+1];
				char* temp_ptr;
				char temp_name[127];

				while(!feof(fp))
				{
						fgets(buff, sizeof(buff), fp);
						strtok(buff, "\n");

						if(strstr(buff, data) != NULL)
						{
								is_data = TRUE;
								is_text = FALSE;
								continue;
						}
						else if(strstr(buff, text) != NULL)
						{
								is_data = FALSE;
								is_text = TRUE;
								temp_index = 0;
								temp_PC = 0;
								continue;
						}
							
						//data section table
						if(is_data)
						{
								temp_ptr = strchr(buff, ':');
								if(strstr(buff, word) != NULL)
								{
										temp_PC++;
										if(temp_PC < data_size)
										{
												int_binary((int)strtol(strrchr(buff, '\t'), NULL, 0), &value[32*temp_PC], 32); // make a array of values in data section
										}
								}
								strcpy(temp_name, strtok(buff, ":"));
								if(temp_ptr != NULL)
								{
										strcpy(data_table[temp_index].name, temp_name);
										data_table[temp_index].addr = temp_PC;
										temp_index++;
										memset(temp_name, 0, sizeof(temp_name));
								}
						}
						else if(is_text)
						{
								temp_ptr = strchr(buff, ':');
								strcpy(temp_name, strtok(buff, ":"));
								if(temp_ptr != NULL)
								{
										strcpy(text_table[temp_index].name, temp_name);
										text_table[temp_index].addr = temp_PC;
										temp_index++;
								}
								else
								{
										//check whether la make one or two instrucitons.
										if(strstr(buff, "\tla\t") != NULL)
										{
												for(int k=0; k<data_label; k++)
												{
														if(!strcmp(strstr(buff, ", ")+2, data_table[k].name))
														{
																if(data_table[k].addr == 0)
																{
																		temp_PC++;
																		break;
																}
																else
																{
																		temp_PC += 2;
																		text_size ++;
																		break;
																}
														}
												}
										}
										else
											temp_PC++;
								}
						}
				}
	
				value[32*data_size] = '\0';

				//make new objective file for output
				fp_out = fopen("test.o", "w");
				//fp_out = fopen(strcat(strtok(argv[1], ".s"), ".o") , "w");

				int_binary(text_size*4, temp_write, 32);
				temp_write[32] = '\0';
				fputs(temp_write, fp_out);

				int_binary(data_size*4, temp_write, 32);
				fputs(temp_write, fp_out);

				fseek(fp, 0, SEEK_SET);
				is_text = FALSE;
				is_data = FALSE;

				while(!feof(fp))
				{

						if(fgets(buff, sizeof(buff), fp) == NULL)
								break;
						strtok(buff, "\n");
						// find text section directive
						if(strstr(buff, text) != NULL) 
						{
								is_text = TRUE;
								continue;
						}
						if(is_text == TRUE) // read and parse instruction lines
						{
								temp = strchr(buff, ':');
								if(temp == NULL)
								{
										if(strstr(buff, "\tla\t") != NULL)
										{
												for(int k=0; k<data_label; k++)
												{
														if(!strcmp(strstr(buff, ", ")+2, data_table[k].name))
														{
																if(data_table[k].addr == 0)
																{
																		PC++;
																		break;
																}
																else
																{
																		PC += 2;
																		break;
																}
														}
												}
										}
										else
											PC++;
										inst_convert(buff, temp_write, data_table, data_label, text_table, text_label, PC);
										fputs(temp_write, fp_out);
								}
						}
				}

				fputs(value, fp_out);	

//print contents of tables for test
/*   
				printf("text size : %d, data size : %d\n", text_size, data_size);
				for(int i=0; i<data_label; i++)
					printf("data_table[%d] : %s, %d\n", i, data_table[i].name, data_table[i].addr);
				for(int i=0; i<text_label; i++)
						printf("text_table[%d] : %s, %d\n", i, text_table[i].name, text_table[i].addr);
*/
		}
		fclose(fp);
		fclose(fp_out);
}



void inst_convert(char* buff, char* answer, LABEL* data_table, int data_index, LABEL* text_table, int text_index, int PC){
		char inst[6];
		int after_inst = FALSE;
		int k = 0;
		int temp = 0;
		
		//parse instruction
		for(int i=0; i<255; i++)
		{
				if(buff[i] == '\t' || buff[i] == ' ')
				{
						if(after_inst)
						{
								inst[k] = 0;
								break;
						}
						else
							continue;
				}
				else
				{
						after_inst = TRUE;
						inst[k] = buff[i];
						k++;
				}
		}

		int count = 0;
		int index = 0;

		//R format instructions

		//addu, and, or, nor, slut, subu
		if(!strcmp(inst, "addu") || !strcmp(inst, "and") || !strcmp(inst, "or") || !strcmp(inst, "nor") || !strcmp(inst, "sltu") || !strcmp(inst, "subu"))
		{
				strncpy(&answer[0], "0000000", 6);
				for(int i=0; i<255; i++)
				{
						if(buff[i] == '$')
						{
								temp = atoi(&buff[i+1]);
								if(count == 0)
										index = 16;
								else
										index = 6+5*(count-1);
								int_binary(temp, &answer[index], 5);
								count++;
						}
						else if(buff[i] == '\0')
								break;
				}
				strcpy(&answer[21], "00000");
				if(!strcmp(inst, "addu"))
						strcpy(&answer[26], "100001");
				else if(!strcmp(inst, "and"))
						strcpy(&answer[26], "100100");
				else if(!strcmp(inst, "or"))
						strcpy(&answer[26], "100101");
				else if(!strcmp(inst, "nor"))
						strcpy(&answer[26], "100111");
				else if(!strcmp(inst, "sltu"))
						strcpy(&answer[26], "101011");
				else
						strcpy(&answer[26], "100011");
				count = 0;
				index = 0;
		}

		//sll, srl
		else if(!strcmp(inst, "sll") || !strcmp(inst, "srl"))
		{
				strncpy(&answer[0], "00000000000", 11);
				for(int i=0; i<255; i++)
				{
						if(buff[i] == '$')
						{
								temp = atoi(&buff[i+1]);
								if(count == 0)
										index = 16;
								else
										index = 11;
								int_binary(temp, &answer[index], 5);
								count++;
						}
						else if(count == 2 && buff[i] == ',')
						{
								temp = atoi(&buff[i+2]);
								int_binary(temp, &answer[21], 5);
						}
						else if(buff[i] == '\0')
								break;

				}
				if(!strcmp(inst, "sll"))
						strcpy(&answer[26], "000000");
				else
						strcpy(&answer[26], "000010");
				count = 0;
				index = 0;
		}

		//jr
		else if(!strcmp(inst, "jr"))
		{
				strncpy(&answer[0], "000000", 6);
				for(int i=0; i<255; i++)
				{
						if(buff[i] == '$')
						{
								temp = atoi(&buff[i+1]);
								int_binary(temp, &answer[6], 5);
						}
						else if(buff[i] == '\0')
								break;
				}
				strcpy(&answer[11], "000000000000000");
				strcpy(&answer[26], "001000");
		}

		//I format

		//addiu, andi, ori, sltiu
		else if(!strcmp(inst, "addiu") || !strcmp(inst, "andi") || !strcmp(inst, "ori") || !strcmp(inst, "sltiu"))
		{
				if(!strcmp(inst, "addiu"))
						strncpy(&answer[0], "001001", 6);
				else if(!strcmp(inst, "andi"))
						strncpy(&answer[0], "001100", 6);
				else if(!strcmp(inst, "ori"))
						strncpy(&answer[0], "001101", 6);
				else
						strncpy(&answer[0], "001011", 6);
				for(int i=0; i<255; i++)
				{
						if(buff[i] == '$')
						{
								temp = atoi(&buff[i+1]);
								if(count == 0)
										index = 11;
								else 
										index = 6;
								int_binary(temp, &answer[index], 5);
								count ++;
						}
						else if(count == 2 && buff[i] == ',')
						{
								if(!strncmp(&buff[i+2], "0x", 2))
										int_binary((int)strtol(&buff[i+2], NULL, 0), &answer[16], 16);
								else
								{
										temp = atoi(&buff[i+2]);
										int_binary(temp, &answer[16], 16);
								}
						}
						else if(buff[i] == '\0')
								break;
				}
				answer[32] = '\0';
				count = 0;
				index = 0;
		}
		//lui
		else if(!strcmp(inst, "lui"))
		{
				strncpy(&answer[0], "00111100000", 11);
				for(int i=0; i<255; i++)
				{
						if(buff[i] == '$')
						{
								temp = atoi(&buff[i+1]);
								int_binary(temp, &answer[11], 5);
								count++;
						}
						else if(count == 1 && buff[i] == ',')
						{
								if(!strncmp(&buff[i+2], "0x", 2))
										int_binary((int)strtol(&buff[i+2], NULL, 0), &answer[16], 16);
								else
								{
										temp = atoi(&buff[i+2]);
										int_binary(temp, &answer[16], 16);
								}
						}
						else if(buff[i] == '\0')
								break;
				}
				answer[32] = '\0';
				count = 0;
		}
		//lw, sw
		else if(!strcmp(inst, "lw") || !strcmp(inst, "sw"))
		{
				if(!strcmp(inst, "lw"))
						strncpy(&answer[0], "100011", 6);
				else
						strncpy(&answer[0], "101011", 6);
				for(int i= 0; i<255; i++)
				{
						if(buff[i] == '$' && count == 0)
						{
								temp = atoi(&buff[i+1]);
								int_binary(temp, &answer[11], 5);
								count++;
						}
						else if(buff[i] == ',' && count == 1)
						{
								if(!strncmp(&buff[i+2], "0x", 2))
										int_binary((int)strtol(&buff[i+2], NULL, 0), &answer[16], 16);
								else
								{
										temp = atoi(&buff[i+2]);
										int_binary(temp, &answer[16], 16);
								}
								count++;
						}
						else if(buff[i] == '$' && count == 2)
						{
								temp = atoi(&buff[i+1]);
								int_binary(temp, &answer[6], 5);
								count ++;
						}
						else if(buff[i] == '\0')
								break;
				}
				answer[32] = '\0';
				count = 0;
		}
		//la
		else if(!strcmp(inst, "la"))
		{
				for(int i=0; i<255; i++)
				{
						if(buff[i] == '$')
								temp = atoi(&buff[i+1]);
						else if(buff[i] == ',')
						{
								for(int k=0; k<data_index; k++)
								{
										if(!strcmp(&buff[i+2], data_table[k].name))
										{
												if(data_table[k].addr != 0)
												{
														strncpy(&answer[0], "00111100000", 11);
														int_binary(temp, &answer[11], 5);
														strncpy(&answer[16], "0001000000000000", 16);

														strncpy(&answer[32], "001101", 6);
														int_binary(temp, &answer[38], 5);
														int_binary(temp, &answer[43], 5);
														int_binary(data_table[k].addr *4, &answer[48], 16);
														answer[64] = '\0';
												}
												else
												{
														strncpy(&answer[0], "00111100000", 11);
														int_binary(temp, &answer[11], 5);
														strncpy(&answer[16], "0001000000000000", 16);
														answer[32] = '\0';
												}
												break;
										}
								}
						}
				}
		}
		//J format
		//beq, bne
		else if(!strcmp(inst, "beq") || !strcmp(inst, "bne"))
		{
				if(!strcmp(inst, "beq"))
						strncpy(&answer[0], "000100", 6);
				else
						strncpy(&answer[0], "000101", 6);

				for(int i=0; i<255; i++)
				{
						if(buff[i] == '$')
						{
								temp = atoi(&buff[i+1]);
								int_binary(temp, &answer[6+(count*5)], 5);
								count ++;
						}
						else if(buff[i] == ',' && count == 2)
						{
								for(int k=0; k<text_index; k++)
								{
										if(!strcmp(&buff[i+2], text_table[k].name))
										{
												int_binary((text_table[k].addr - PC), &answer[16], 16);
										}
								}
						}
				}
				answer[32] = '\0';
				count = 0;
		}
		//j, jal
		else if(!strcmp(inst, "j") || !strcmp(inst, "jal"))
		{
				int t = 0;
				if(!strcmp(inst, "j"))
				{
						strncpy(&answer[0], "000010", 6);
						t = 3;
				}
				else
				{
						strncpy(&answer[0], "000011", 6);
						t = 5;
				}

				for(int k=0; k<text_index; k++)
				{
						if(!strcmp(&buff[t], text_table[k].name))
						{
								int_binary(0x100000 + text_table[k].addr, &answer[6], 26);
						}
				}
				answer[32] = '\0';
		}
		else
		{
				strcpy(answer, inst);
		}


// print each line parsed		
//		puts(answer);


		return;
}
		
		

void int_binary(int num, char *str, int size)
{
		int temp;
		if(size > 16)
		{
				int t = num >> 16;
				temp = 0x01 << (size-16);
				while(temp >>= 1)
						*str++ = !!(temp & t) + '0';
				temp = 0x01 << 16;
				while(temp >>= 1)
						*str++ = !!(temp & num) + '0';
		}
		else
		{
				temp = 0x01 << size;
				while(temp >>= 1)
					*str++ = !!(temp & num) + '0';
		}
}



