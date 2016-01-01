#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <assert.h>
#define BYTES_PER_WORD 4

int total_reads;
int total_writes;
int write_backs;
int reads_hits;
int write_hits;
int reads_misses;
int write_misses;

uint32_t time;


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
/* Procedure : cdump                                           */
/*                                                             */
/* Purpose   : Dump cache configuration                        */   
/*                                                             */
/***************************************************************/
void cdump(int capacity, int assoc, int blocksize){

	printf("Cache Configuration:\n");
    	printf("-------------------------------------\n");
	printf("Capacity: %dB\n", capacity);
	printf("Associativity: %dway\n", assoc);
	printf("Block Size: %dB\n", blocksize);
	printf("\n");
}

/***************************************************************/
/*                                                             */
/* Procedure : sdump                                           */
/*                                                             */
/* Purpose   : Dump cache stat		                       */   
/*                                                             */
/***************************************************************/
void sdump(int total_reads, int total_writes, int write_backs,
	int reads_hits, int write_hits, int reads_misses, int write_misses) {
	printf("Cache Stat:\n");
    	printf("-------------------------------------\n");
	printf("Total reads: %d\n", total_reads);
	printf("Total writes: %d\n", total_writes);
	printf("Write-backs: %d\n", write_backs);
	printf("Read hits: %d\n", reads_hits);
	printf("Write hits: %d\n", write_hits);
	printf("Read misses: %d\n", reads_misses);
	printf("Write misses: %d\n", write_misses);
	printf("\n");
}


/***************************************************************/
/*                                                             */
/* Procedure : xdump                                           */
/*                                                             */
/* Purpose   : Dump current cache state                        */ 
/* 							       */
/* Cache Design						       */
/*  							       */
/* 	    cache[set][assoc][word per block]		       */
/*      						       */
/*      						       */
/*       ----------------------------------------	       */
/*       I        I  way0  I  way1  I  way2  I                 */
/*       ----------------------------------------              */
/*       I        I  word0 I  word0 I  word0 I                 */
/*       I  set0  I  word1 I  word1 I  work1 I                 */
/*       I        I  word2 I  word2 I  word2 I                 */
/*       I        I  word3 I  word3 I  word3 I                 */
/*       ----------------------------------------              */
/*       I        I  word0 I  word0 I  word0 I                 */
/*       I  set1  I  word1 I  word1 I  work1 I                 */
/*       I        I  word2 I  word2 I  word2 I                 */
/*       I        I  word3 I  word3 I  word3 I                 */
/*       ----------------------------------------              */
/*      						       */
/*                                                             */
/***************************************************************/
void xdump(int set, int way, uint32_t** cache)
{
	int i,j,k = 0;

	printf("Cache Content:\n");
    	printf("-------------------------------------\n");
	for(i = 0; i < way;i++)
	{
		if(i == 0)
		{
			printf("    ");
		}
		printf("      WAY[%d]",i);
	}
	printf("\n");

	for(i = 0 ; i < set;i++)
	{
		printf("SET[%d]:   ",i);
		for(j = 0; j < way;j++)
		{
			if(k != 0 && j == 0)
			{
				printf("          ");
			}
			printf("0x%08x  ", cache[i][j]);
		}
		printf("\n");
	}
	printf("\n");
}

void cycle(char *buffer, uint32_t** cache, int** dirty_bit, uint32_t** lru, int tag_bit, int index_bit, int block_offset_bit, int way){


	int i;
	int j;
	char command;
	uint32_t addr;

	// command('R' or 'W) , addr 
	strncpy(&command, buffer, 1);
	
	addr = strtol(&buffer[2], NULL, 16);

	uint32_t index;
	uint32_t tag;
	uint32_t block_off;

	// index, tag, block_offset
	tag = addr >> (32-tag_bit);

	index = (addr << tag_bit) >> (32-index_bit);

	block_off = (addr << (index_bit + tag_bit)) >> (32-block_offset_bit);

	int hit_flag = 0;
	int dst_way = 0;
	
	//increasing LRU value
//	for (j=0; j<way;j++){
//		if (cache[index][j] != 0){
//			lru[index][j] = lru[index][j]+1;
//		}
//	}


	//for Read instruction
	if(command == 'R'){
		
		total_reads++;
	
		for(i=0; i<way; i++){
			uint32_t temp_tag = cache[index][i] >> (32-tag_bit);

			//Read HIT
			if (temp_tag != 0 && temp_tag == tag){
				reads_hits++;
				lru[index][i] = time;
				hit_flag = 1;
			}
			
		}

		//Read MISS
		if (hit_flag == 0){
			reads_misses++;
			int selected_way = 0;

			for (j=0; j<way; j++){

				if (cache[index][j] == 0x0){
					selected_way = j;
					break;
				}
				else if (lru[index][j] < lru[index][selected_way]){
					selected_way = j;
				}
			}

			if (dirty_bit[index][selected_way] == 1){
				write_backs++;
			}
			cache[index][selected_way] = (addr >> block_offset_bit) << block_offset_bit;	
			lru[index][selected_way] = time;
			dirty_bit[index][selected_way] = 0;
		}

		return;
	}

	else if(command == 'W'){

		total_writes++;

		for(i=0; i<way; i++){
			uint32_t temp_tag = cache[index][i] >> (32-tag_bit);

			//Write HIT
			if(temp_tag != 0 && temp_tag == tag){
				write_hits++;
				dirty_bit[index][i] = 1; // dirty bit 
				lru[index][i] = time;
				hit_flag = 1;
			}
		}
			
		if(hit_flag == 0){
			write_misses++;
			int selected_way = 0;

			for (j=0; j<way; j++){
				if (cache[index][j] == 0x0){
					selected_way = j;
					break;
				}
				else if (lru[index][j] < lru[index][selected_way]){
					selected_way = j;
				}
			}
			if (dirty_bit[index][selected_way] == 1){
				write_backs++;
			}
			cache[index][selected_way] = (addr >> block_offset_bit) << block_offset_bit;
			lru[index][selected_way] = time;
			dirty_bit[index][selected_way] = 1;
		}
		
		return;
	}


}




int main(int argc, char *argv[]) {                              

	FILE *prog;
	char buffer[13];

	char** tokens;
	uint32_t** cache;
	int** dirty_bit;
	uint32_t** lru;
	
	int i, j, k;	
	int capacity;
	int way;
	int blocksize;
	int count = 1;

	//initializing global variables
	total_reads=0;
	total_writes=0;
	write_backs=0;
	reads_hits=0;
	write_hits=0;
	reads_misses=0;
	write_misses=0;
	
	int x_dump_set = 0;

	// test example

	while(count != argc-1){
		if(strcmp(argv[count], "-c") == 0){
			tokens = str_split(argv[++count], ':');
			
			capacity = (int)strtol(*(tokens), NULL, 10);
			way = (int)strtol(*(tokens+1),NULL, 10);
			blocksize = (int)strtol(*(tokens+2), NULL, 10);
		}
		else if(strcmp(argv[count], "-x") == 0){
			x_dump_set = 1;
		}
		else{
			printf("Error : usage: %s [-c capacity:way:blocksize] [-x] input_trace\n", argv[0]);
			exit(1);
		}
		count++;
	}

	//passing parameter for "cycle"
	int block_offset_bit = log10(blocksize)/log10(2);
	int index_bit = log10(capacity/(way*blocksize))/log10(2);
	
	//int tag_bit = 32-index_bit-block_offset_bit-2; //byte offset도 포함시켜야 하는 것인가??? 
	int tag_bit = 32-index_bit-block_offset_bit;

	int set = capacity/way/blocksize;
	int words = blocksize / BYTES_PER_WORD;	


	// allocate
	cache = (uint32_t**) malloc (sizeof(uint32_t*) * set);	
	lru = (uint32_t**) malloc (sizeof(uint32_t*) * set);
	dirty_bit = (int**) malloc (sizeof(int*) * set);
	
	for(i = 0; i < set; i++) {
		cache[i] = (uint32_t*) malloc(sizeof(uint32_t) * way);
		lru[i] = (uint32_t*) malloc(sizeof(uint32_t) * way);
		dirty_bit[i] = (int*) malloc(sizeof(int) * way);
	}
	
	

	for(i = 0; i < set; i++) {
		for(j = 0; j < way; j ++){ 
			cache[i][j] = 0x0;
			dirty_bit[i][j] = 0;
			lru[i][j] = 0;
		}
	}
	
	prog = fopen(argv[argc-1], "r");
	if(prog == NULL){
		printf("Error: Can't open program file %s\n", argv[argc-1]);
		exit(-1);
	}

	time = 1;

	while (fgets(buffer,13,prog) != NULL){

		cycle(buffer, cache, dirty_bit, lru, tag_bit, index_bit, block_offset_bit, way);
		//cdump(capacity,way,blocksize);
		//sdump(total_reads,total_writes,write_backs,reads_hits,write_hits,reads_misses,write_misses);
		time++;


	}

   	cdump(capacity, way, blocksize);
   	sdump(total_reads, total_writes, write_backs, reads_hits, write_hits, reads_misses, write_misses); 
    if(x_dump_set) {
		xdump(set, way, cache);
	}

	for(i =0; i<set; i++){
		free(cache[i]);
		free(lru[i]);
		free(dirty_bit[i]);
	}
	free(cache);
	free(lru);
	free(dirty_bit);

	fclose(prog);

	return 0;

}
