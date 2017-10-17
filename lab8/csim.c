//name: Shuo Yang
//ID:   5110379038

#include "cachelab.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include <string.h>
#include <malloc.h>

int main(int argc, char **argv)
{
	//variables
	int getoptCh = 0;
	int nHits = 0;
	int nMisses = 0;
	int nEvicts = 0;
	int cache_s = 0;
	int cache_E = 0;
	int cache_b = 0;
	long addr = 0;
	long addr_t = 0;
	long addr_s = 0;
	long size = 0;
	char *fname;
	FILE *pF = NULL;
	long* cache = NULL;
	char op[2];

	//use getopt to get paramaters
	while((getoptCh = getopt(argc, argv, "s:E:b:t:")) != -1){
		switch(getoptCh){
		case 's':
			cache_s = atoi(optarg);
			break;
		case 'E':
			cache_E = atoi(optarg);
			break;
		case 'b':
			cache_b = atoi(optarg);
			break;
		case 't':
			fname = (char*)malloc(sizeof(char)*strlen(optarg)+1);
			memcpy(fname, optarg, sizeof(char)*strlen(optarg)+1);
			break;
		default:
			break;
		}
	}
	//open file
	pF = fopen(fname, "r");

	//generate a cache block
	cache = malloc(sizeof(long) * (1<<cache_s) * cache_E);
	memset(cache, 0, sizeof(long) * (1<<cache_s) * cache_E);
	//parse the address with cache
	while(fscanf(pF, "%s", op) != EOF){
		fscanf(pF, "%lx,%ld", &addr, &size);
		if(op[0] == 'I')  continue;
		//get s and t from address
		addr_s = addr>>cache_b & ((1<<cache_s)-1);
		addr_t = addr>>(cache_b+cache_s);
		//set valid bit
		addr_t = addr_t | (((long)1)<<63);

		//simulate the cache with address
		int hitFlag = 0;
		int i= 0;
		for(i=0; i< cache_E; i++){
			long cacheLine = *(cache + addr_s * cache_E + i);
			if((cacheLine >> 63) == 0) break;
			if(cacheLine == addr_t){
				hitFlag = 1;
				break;
			}
		}
		if(hitFlag){
			nHits++;
			if(op[0] == 'M')  nHits++;
			long tmpLine = *(cache + addr_s * cache_E + i);
			for(; i<cache_E-1; i++){
				if(((*(cache+addr_s*cache_E+i+1))>>63) == 0)  break;
				*(cache+addr_s*cache_E+i) = *(cache+addr_s*cache_E+i+1);
			}
			*(cache+addr_s*cache_E+i) = tmpLine;
		}
		else{
			nMisses++;
			if(op[0] == 'M')  nHits++;
			if(i != cache_E)
				*(cache + addr_s * cache_E + i) = addr_t;
			else{
				nEvicts++;
				for(i= 0; i< cache_E-1; i++){
					*(cache+addr_s*cache_E+i) = *(cache+addr_s*cache_E+i+1);
				}
				*(cache+addr_s*cache_E+i) = addr_t;
			}
		}
	}
	free(fname);
	free(cache);
	//print the result
    printSummary(nHits, nMisses, nEvicts);
    return 0;
}
