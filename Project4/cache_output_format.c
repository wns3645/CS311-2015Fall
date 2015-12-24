#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <math.h>

#define BYTES_PER_WORD 4

int total_reads;
int total_writes;
int write_backs;
int reads_hits;
int write_hits;
int reads_misses;
int write_misses;


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

void cycle(char *buffer, uint32_t* cache, int tag_bit, int index_bit, int block_offset_bit){


	char command;
	uint32_t addr;

	// command('R' or 'W) , addr 
	strncpy(command, buffer, 1);
	
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

	//for Read instruction
	if(command == 'R'){
		
		total_reads++;
	
		for(int i=0; i<sizeof(cache[index]); i++){
			uint32_t temp_tag = cache[index][i] >> (32-tag_bit);

			//Read HIT
			if (temp_tag == tag){
				reads_hits++;
				lru[index][i] = 1;
				hit = 1;
				break;
			}
			
		}

		//Read MISS
		if (hit == 0){
			reads_misses++;
			cache[index][dst_way] = (addr >> block_offset_bit) << block_offset_bit;
			lru[index][dst_way] = 1;
		}

		return;
	}

	else if(command == 'W'){

		total_writes++;

		for(int i=0; i<sizeof(cache[index]); i++){
			uint32_t temp_tag = cache[index][i] >> (32-tag_bit);

			//Write HIT
			if(temp_tag == tag){
				write_hits++;
				dirty[index][i] = 1; // dirty bit 
				lru[index][i] = 1;
				hit = 1;
				break;
			}
		}
			
		if(hit == 0){
			write_misses++;
			
	}

}




int main(int argc, char *argv[]) {                              

	FILE *prog;
	char buffer[13];

	char** tokens;
	uint32_t** cache;
	int i, j, k;	
	int capacity = 256;
	int way = 4;
	int blocksize = 8;
	int set = capacity/way/blocksize;
	int words = blocksize / BYTES_PER_WORD;	
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
	// allocate
	cache = (uint32_t**) malloc (sizeof(uint32_t*) * set);
	for(i = 0; i < set; i++) {
		cache[i] = (uint32_t*) malloc(sizeof(uint32_t) * way);
	}
	for(i = 0; i < set; i++) {
		for(j = 0; j < way; j ++) 
			cache[i][j] = 0x0;
	}


	// test example

	while(count != argc-1){
		if(strcmp(argv[count], "-c") == 0){
			tokens = str_split(argv[++count], ':');
			
			capacity = (int)strtol(*(tokens), NULL, 16);
			way = (int)strtol(*(tokens+1),NULL,16);
			blocksize = (int)strtol(*(tokens+2), NULL, 16);
		}
		if(strcmp(argv[count], "-x") == 0){
			x_dump_set = 1;
		}
	
	}

	//passing parameter for "cyclfe"
	int var1 = log10(2);
	int var3 = log10(blocksize/4);
	int block_offset_bit = var3/var1;

	int var2 = log10(capacity/(way*blocksize));
	int index_bit = var2/var1;
	
	//int tag_bit = 32-index_bit-block_offset_bit-2; //byte offset도 포함시켜야 하는 것인가??? 
	int tag_bit = 32-index_bit-block_offset_bit;

	prog = fopen(program_filename, "r");
	if(prog == NULL){
		printf("Error: Can't open program file %s\n", argv[argc-1]);
		exit(-1);
	}
	
	
	while (fgets(buffer,33,prog) != NULL){


		cycle(buffer, cache, tag_bit, index_bit, block_offset_bit);
		cdump(capacity,way,blocksize);
		sdump(total_reads,total_writes,write_backs,read_hits,write_hits,read_misses,write_misses);



	}


   	cdump(capacity, way, blocksize);
   	sdump(total_reads, total_writes, write_backs, read_hits, write_hits, read_misses, write_misses); 
    if(x_dump_set) xdump(set, way, cache);

   	return 0;
}
