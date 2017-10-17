/*
the revision is based on the codes from our book.

1.using explicit free list
2.for realloc, first using the free space behind the requirement
3.using first fit
4.the definitiong of mm_check is at teh bottom


 */
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <string.h>

#include "mm.h"
#include "memlib.h"

/*********************************************************
 * NOTE TO STUDENTS: Before you do anything else, please
 * provide your team information in the following struct.
 ********************************************************/
team_t team = {
    /* Team name */
    "5110379038",
    /* First member's full name */
    "Shuo Yang",
    /* First member's email address */
    "",
    /* Second member's full name (leave blank if none) */
    "",
    /* Second member's email address (leave blank if none) */
    ""
};
/* single word (4) or double word (8) alignment */
#define ALIGNMENT 8

/* rounds up to the nearest multiple of ALIGNMENT */
#define ALIGN(size) (((size) + (ALIGNMENT-1)) & ~0x7)


#define SIZE_T_SIZE (ALIGN(sizeof(size_t)))

/* Basic constants and macros */
#define WSIZE 4 /* word size (bytes) */
#define DSIZE 8 /* double word size (bytes) */
#define CHUNKSIZE (1<<12) /* Extend heap by this amount (bytes) */
#define MAX(x, y) ((x) > (y)? (x) : (y))

/* Pack a size and allocated bit into a word */
#define PACK(size, alloc) ((size) | (alloc))

/* Read and write a word at address p */
#define GET(p) (*(unsigned int *)(p))
#define PUT(p, val) (*(unsigned int *)(p) = (val))

/* Read the size and allocated fields from address p */
#define GET_SIZE(p) (GET(p) & ~0x7)
#define GET_ALLOC(p) (GET(p) & 0x1)

/* Given block ptr bp, compute address of its header and footer */
#define HDRP(bp) ((char *)(bp) - WSIZE)
#define FTRP(bp) ((char *)(bp) + GET_SIZE(HDRP(bp)) - DSIZE)

/* Given block ptr bp, compute address of next and previous blocks */
#define NEXT_BLKP(bp) ((char *)(bp) + GET_SIZE(((char *)(bp) - WSIZE)))
#define PREV_BLKP(bp) ((char *)(bp) - GET_SIZE(((char *)(bp) - DSIZE)))


//------------------------------------------------------------
static void *heap_listp = NULL;
static void *free_listp = NULL;

static void *extend_heap(size_t words);
static void *coalesce(void *bp);
static void *find_fit(size_t asize);
static void place(void *bp, size_t asize);
static void coal_free(void *bp);
static int mm_check();
static void free_check();

/*
 *  coal_free - for the use of free list
 */
 static void coal_free(void *bp){
	if((*((int*)bp) != (int)NULL) && (*((int*)bp+1) != (int)NULL)){
	 	*((int*)(*((int*)bp))+1) = *((int*)bp+1);
		*((int*)(*((int*)bp+1))) = *((int*)bp);
	}else if((*((int*)bp) != (int)NULL) && (*((int*)bp+1) == (int)NULL)){
		*((int*)(*((int*)bp))+1) = (int)NULL;
	}else if((*((int*)bp) == (int)NULL) && (*((int*)bp+1) != (int)NULL)){
		free_listp = *((int*)bp+1);
		*((int*)(*((int*)bp+1))) = (int)NULL;
	}else{
		free_listp = NULL;
	}
 }


/*
 * extend_heap - extend the size of heap
 */
static void *extend_heap(size_t words)
{
	char *bp;
	size_t size;

	/* Allocate an even number of words to maintain alignment */
	size = (words % 2) ? (words+1) * WSIZE : words * WSIZE;
	if ((long)(bp = mem_sbrk(size))  == -1)
		return NULL;

	/* Initialize free block header or footer and the epilogue header */
	PUT(HDRP(bp), PACK(size, 0)); 		/* free block header */
	PUT(FTRP(bp), PACK(size, 0));		/* free block footer */
	PUT(HDRP(NEXT_BLKP(bp)), PACK(0, 1)); /* new epilogue header */

	/* Coalesce if the previous block was free */
	return coalesce(bp);
}


/*
 * coalesce - coalesce the continuous free blocks
 */
static void *coalesce(void *bp)
{
	size_t prev_alloc = GET_ALLOC(FTRP(PREV_BLKP(bp)));
	size_t next_alloc = GET_ALLOC(HDRP(NEXT_BLKP(bp)));
	size_t size = GET_SIZE(HDRP(bp));
	
	if (prev_alloc && next_alloc) { /* Case 1 */
		return bp;
	}

	else if (prev_alloc && !next_alloc) { /* Case 2 */
		coal_free(NEXT_BLKP(bp));
		size += GET_SIZE(HDRP(NEXT_BLKP(bp)));
		PUT(HDRP(bp), PACK(size, 0));
		PUT(FTRP(bp), PACK(size,0));
		return bp;
	}

	else if (!prev_alloc && next_alloc) { /* Case 3 */
		coal_free(PREV_BLKP(bp));
		size += GET_SIZE(HDRP(PREV_BLKP(bp)));
		PUT(FTRP(bp), PACK(size, 0));
		PUT(HDRP(PREV_BLKP(bp)), PACK(size, 0));
		return PREV_BLKP(bp);
	}

	else { /* Case 4 */
		coal_free(PREV_BLKP(bp));
		coal_free(NEXT_BLKP(bp));
		size += GET_SIZE(HDRP(PREV_BLKP(bp))) +
             GET_SIZE(FTRP(NEXT_BLKP(bp)));
		PUT(HDRP(PREV_BLKP(bp)), PACK(size, 0));
		PUT(FTRP(NEXT_BLKP(bp)), PACK(size, 0));
		return PREV_BLKP(bp);
	}
}


/*
 * find_fit - find a block which fit the argument asize
 */
static void *find_fit(size_t asize)
{
	void *bp ;
	if(free_listp == NULL)  return NULL;
	    	
    /* first fit search */
    for (bp = free_listp; *((int*)bp+1) != NULL ; bp = *((int*)bp+1)) {
		if (asize <= GET_SIZE(HDRP(bp))) {
	 		return bp;
 		}
    }
	if(asize<=GET_SIZE(HDRP(bp))) return bp;
    return NULL;  /*no fit */
}


/*
 * place - alloc size of asize start from bp
 */
static void place(void *bp, size_t asize)
{
  	size_t csize = GET_SIZE(HDRP(bp));
	
   	if ( (csize - asize) >= (2 * DSIZE)) {
 		PUT(HDRP(bp), PACK(asize, 1));
 		PUT(FTRP(bp), PACK(asize, 1));

		*((int*)((char*)bp + asize)) = *((int*)bp);
		*((int*)((char*)bp + asize + WSIZE)) = *((int*)bp + 1);
		bp = NEXT_BLKP(bp);
 		PUT(HDRP(bp), PACK(csize-asize, 0));
 		PUT(FTRP(bp), PACK(csize-asize, 0));
        
		if(*((int*)bp) != (int)NULL){
			*((int*)(*((int*)bp))+1) = (int)bp;
		}else{
			free_listp = bp;
		}
		if(*((int*)bp+1) != (int)NULL){
		    *((int*)(*((int*)bp+1))) = (int)bp;
		}

 	} else {
		coal_free(bp);
 		PUT(HDRP(bp), PACK(csize, 1));
 		PUT(FTRP(bp), PACK(csize, 1));
 	}
}


//----------------------------------------------------------------

/* 
 * mm_init - initialize the malloc package.
 */
int mm_init(void)
{
	/* create the initial empty heap */
	if ((heap_listp = mem_sbrk(4*WSIZE)) == (void *) -1)
		return -1;
	PUT(heap_listp, 0); 			         /* alignment padding */
	PUT(heap_listp+(1*WSIZE), PACK(DSIZE, 1));  /* prologue header */
	PUT(heap_listp+(2*WSIZE), PACK(DSIZE, 1));  /* prologue footer */
	PUT(heap_listp+(3*WSIZE), PACK(0, 1)); 	         /* epilogue header */
	heap_listp += (2*WSIZE);

	/* Extend the empty heap with a free block of CHUNKSIZE bytes */
	if (extend_heap(CHUNKSIZE/WSIZE) == NULL)
		return -1;
	free_listp = heap_listp + DSIZE;
    *((int*)free_listp) = (int)NULL;
	*((int*)free_listp+1) = (int)NULL;

	return 0;
}

/* 
 * mm_malloc - Allocate a block by incrementing the brk pointer.
 *     Always allocate a block whose size is a multiple of the alignment.
 */
void *mm_malloc(size_t size)
{
	size_t asize; /* adjusted block size */
	size_t extendsize; /* amount to extend heap if no fit */
	char *bp;

	/* Ignore spurious requests */
	if (size == 0)
		return NULL;
	/* Adjust block size to include overhead and alignment reqs. */
	if (size <= DSIZE)
		asize = 2 * DSIZE;
	else
		asize = DSIZE * ((size + (DSIZE) + (DSIZE-1)) / DSIZE);
	/* Search the free list for a fit */
	if ((bp = find_fit(asize)) != NULL) {
		place (bp, asize);
//		mm_check();
//		free_check();
		return bp;
	}
	/* No fit found. Get more memory and place the block */


	extendsize = MAX (asize, CHUNKSIZE) ;
	if ((bp = extend_heap(extendsize/WSIZE)) == NULL)
		return NULL;


	if(free_listp != NULL){
		*((int*)bp + 1) = (int)free_listp;
		*((int*)free_listp) = (int)(bp);
		*((int*)bp) = (int)NULL;
		free_listp = bp;
	}else{
		free_listp = bp;
		*((int*)free_listp) = (int)NULL;
		*((int*)free_listp+1) = (int)NULL;

	}
	place(bp, asize);
//	mm_check();
//	free_check();
	return bp;
}
/*
 * mm_free - Freeing a block does nothing.
 */
void mm_free(void *ptr)
{
	size_t size = GET_SIZE(HDRP(ptr));
	PUT(HDRP(ptr), PACK(size, 0));
	PUT(FTRP(ptr), PACK(size, 0));
	ptr = coalesce(ptr);

	if(free_listp){
		*((int*)ptr + 1) = (int)free_listp;
		*((int*)free_listp) = (int)(ptr);
		*((int*)ptr) = (int)NULL;
		free_listp = ptr;
	}else{
		free_listp = ptr;
		*((int*)free_listp) = (int)NULL;
		*((int*)free_listp+1) = (int)NULL;
	}
//	mm_check();
//	free_check();
}


void *mm_realloc(void *oldptr, size_t size)
{
	void *newptr;
	size_t copySize;
	if(size == 0)  return NULL;

	copySize = GET_SIZE(HDRP(oldptr)) - DSIZE;
	if(size < copySize)  return oldptr;



	if(!GET_ALLOC(HDRP(NEXT_BLKP(oldptr)))){ 
		size_t asize = GET_SIZE(HDRP(oldptr));
		size_t bsize = GET_SIZE(HDRP(NEXT_BLKP(oldptr)));
		if(asize + bsize - DSIZE == ALIGN(size)){
			coal_free((char*)oldptr + asize);
			PUT(HDRP(oldptr), PACK(asize+bsize, 1));
			PUT(FTRP(oldptr), PACK(asize+bsize, 1));
			return oldptr;
		}else if(asize+bsize-2*DSIZE > ALIGN(size)){
			int old_prev = *((int*)((char*)oldptr + asize));
			int old_succ = *((int*)((char*)oldptr + WSIZE + asize));

		 	PUT(HDRP(oldptr), PACK((ALIGN(size) + DSIZE), 1));
		    PUT(FTRP(oldptr), PACK((ALIGN(size) + DSIZE), 1));

			*((int*)((char*)oldptr+ALIGN(size) + DSIZE)) = old_prev;
			*((int*)((char*)oldptr+ WSIZE + DSIZE +ALIGN(size))) = old_succ;


			PUT(HDRP(NEXT_BLKP(oldptr)), PACK((asize+bsize-DSIZE-ALIGN(size)), 0));
			PUT(FTRP(NEXT_BLKP(oldptr)), PACK((asize+bsize-DSIZE-ALIGN(size)), 0));
			
			if(*((int*)((char*)oldptr + DSIZE + ALIGN(size))) != (int)NULL){
				*((int*)(*((int*)((char*)oldptr + DSIZE + ALIGN(size))))+1) = (int)((char*)oldptr + DSIZE + ALIGN(size));
			}else{
				free_listp = (char*)oldptr+ALIGN(size)+DSIZE;
			}

			if(*((int*)((char*)oldptr+ALIGN(size)+DSIZE+WSIZE)) != (int)NULL){
				*((int*)(*((int*)((char*)oldptr+ALIGN(size)+DSIZE+WSIZE)))) = (int)((char*)oldptr+ DSIZE + ALIGN(size));
			}
			return oldptr;
		}
	}

	newptr = mm_malloc(size);
	memcpy(newptr, oldptr, copySize);
	mm_free(oldptr);

//	mm_check();
//	free_check();

	return newptr;
}


//-----------------------------------------------
//mm_check and free list check
//print the information of memory block and free list

//return 1 when there are two continuous free block which are not coalecsed
//return 0 when the mem block is right
static int mm_check(void){
	void *bp;
	int prev_alloc = 1;
	for(bp = heap_listp; GET_SIZE(HDRP(bp)) > 0; bp = NEXT_BLKP(bp)){
		printf("%x %x\n", GET_SIZE(HDRP(bp)), GET_ALLOC((HDRP(bp))));
		if(prev_alloc == 0 && GET_ALLOC(HDRP(bp)) == 0) return 0;
		prev_alloc = GET_ALLOC(HDRP(bp));
	}	
	printf("%x %x\n", GET_SIZE(HDRP(bp)), GET_ALLOC((HDRP(bp))));
	puts("\n");
	return 1;
}


static void free_check(){
	void *bp;
	if(free_listp){
		for(bp = free_listp; *((int*)bp+1) != (int)NULL; bp = *((int*)bp+1))
		{
			printf("%x %x %x\n", (int)bp, *((int*)bp), *((int*)bp+1));
		}
		printf("%x %x %x\n", (int)bp, *((int*)bp), *((int*)bp+1));
		puts("\n");
	}
}
