//name: Shuo Yang
//ID:   5110379038

/* 
 * trans.c - Matrix transpose B = A^T
 *
 * Each transpose function must have a prototype of the form:
 * void trans(int M, int N, int A[N][M], int B[M][N]);
 *
 * A transpose function is evaluated by counting the number of misses
 * on a 1KB direct mapped cache with a block size of 32 bytes.
 */ 
#include <stdio.h>
#include "cachelab.h"

int is_transpose(int M, int N, int A[N][M], int B[M][N]);

/* 
 * transpose_submit - This is the solution transpose function that you
 *     will be graded on for Part B of the assignment. Do not change
 *     the description string "Transpose submission", as the driver
 *     searches for that string to identify the transpose function to
 *     be graded. 
 */
char transpose_submit_desc[] = "Transpose submission";
void transpose_submit(int M, int N, int A[N][M], int B[M][N])
{
	int i,j,k,l,tmp;	//5 local variables in total
	if(M == 32 && N == 32){
		//block size is 8 * 8
		for(i= 0; i< 4; i++){
		for(j= 0; j< 4; j++){
			if(i == j){
				//diagonal line block
				for(k= 0; k< 8; k++){
					for(l= 0; l< k; l++)  B[8*j+l][8*i+k] = A[8*i+k][8*j+l];
					for(l=k+1; l<8; l++)  B[8*j+l][8*i+k] = A[8*i+k][8*j+l];
					l = k;
					B[8*j+l][8*i+k] = A[8*i+k][8*j+l];
				}
			}else{
				//rest blocks
				for(k= 0; k< 8; k++){
				for(l= 0; l< 8; l++){
					B[8*j+l][8*i+k] = A[8*i+k][8*j+l];
				}
				}
			}
		}
		}
	}else if(M == 64 && N == 64){
		//block size is 8 * 8
		for(i= 0; i< 8; i++){
		for(j= 0; j< 8; j++){
			if(i == j){
				//diagonal line block
				//cut each block into 4 small blocks

				//line0
				B[8*j+1][8*i+0] = A[8*i+0][8*j+1];
				B[8*j+2][8*i+0] = A[8*i+0][8*j+2];
				B[8*j+3][8*i+0] = A[8*i+0][8*j+3];
				B[8*j+2][8*i+4] = A[8*i+0][8*j+4];
				B[8*j+3][8*i+4] = A[8*i+0][8*j+5];
				B[8*j+1][8*i+4] = A[8*i+0][8*j+7];
				tmp = A[8*i+0][8*j+0];
				B[8*j+0][8*i+4] = A[8*i+0][8*j+6];
				B[8*j+0][8*i+0] = tmp;

				//line1
				B[8*j+0][8*i+1] = A[8*i+1][8*j+0];
				B[8*j+2][8*i+1] = A[8*i+1][8*j+2];
				B[8*j+3][8*i+1] = A[8*i+1][8*j+3];
				B[8*j+2][8*i+5] = A[8*i+1][8*j+4];
				B[8*j+3][8*i+5] = A[8*i+1][8*j+5];
				B[8*j+0][8*i+5] = A[8*i+1][8*j+6];
				tmp = A[8*i+1][8*j+1];
				B[8*j+1][8*i+5] = A[8*i+1][8*j+7];
				B[8*j+1][8*i+1] = tmp;

				//line2
				B[8*j+0][8*i+2] = A[8*i+2][8*j+0];
				B[8*j+1][8*i+2] = A[8*i+2][8*j+1];
				B[8*j+3][8*i+2] = A[8*i+2][8*j+3];
				B[8*j+3][8*i+6] = A[8*i+2][8*j+5];
				B[8*j+0][8*i+6] = A[8*i+2][8*j+6];
				B[8*j+1][8*i+6] = A[8*i+2][8*j+7];
				tmp = A[8*i+2][8*j+2];
				B[8*j+2][8*i+6] = A[8*i+2][8*j+4];
				B[8*j+2][8*i+2] = tmp;

				//line3
				B[8*j+0][8*i+3] = A[8*i+3][8*j+0];
				B[8*j+1][8*i+3] = A[8*i+3][8*j+1];
				B[8*j+2][8*i+3] = A[8*i+3][8*j+2];
				B[8*j+2][8*i+7] = A[8*i+3][8*j+4];
				B[8*j+0][8*i+7] = A[8*i+3][8*j+6];
				B[8*j+1][8*i+7] = A[8*i+3][8*j+7];
				tmp = A[8*i+3][8*j+3];
				B[8*j+3][8*i+7] = A[8*i+3][8*j+5];
				B[8*j+3][8*i+3] = tmp;


				for(k= 4; k< 6; k++){
				for(l= 0; l< 4; l++){
					B[8*j+k][8*i+l] = B[8*j+k-2][8*i+l+4];
				}
				}

				B[8*j+5][8*i+4] = A[8*i+4][8*j+5];
				B[8*j+2][8*i+4] = A[8*i+4][8*j+2];
				B[8*j+3][8*i+4] = A[8*i+4][8*j+3];
				B[8*j+4][8*i+4] = A[8*i+4][8*j+4];

				B[8*j+2][8*i+5] = A[8*i+5][8*j+2];
				B[8*j+3][8*i+5] = A[8*i+5][8*j+3];
				B[8*j+4][8*i+5] = A[8*i+5][8*j+4];
				B[8*j+5][8*i+5] = A[8*i+5][8*j+5];

				B[8*j+3][8*i+6] = A[8*i+6][8*j+3];
				B[8*j+4][8*i+6] = A[8*i+6][8*j+4];
				B[8*j+5][8*i+6] = A[8*i+6][8*j+5];
				B[8*j+2][8*i+6] = A[8*i+6][8*j+2];

				B[8*j+4][8*i+7] = A[8*i+7][8*j+4];
				B[8*j+5][8*i+7] = A[8*i+7][8*j+5];
				B[8*j+2][8*i+7] = A[8*i+7][8*j+2];
				B[8*j+3][8*i+7] = A[8*i+7][8*j+3];

				for(k= 6; k< 8; k++){
				for(l= 0; l< 4; l++){
					B[8*j+k][8*i+l] = B[8*j+k-6][8*i+l+4];
				}
				}

				B[8*j+1][8*i+4] = A[8*i+4][8*j+1];
				B[8*j+6][8*i+4] = A[8*i+4][8*j+6];
				B[8*j+7][8*i+4] = A[8*i+4][8*j+7];
				B[8*j+0][8*i+4] = A[8*i+4][8*j+0];

				B[8*j+6][8*i+5] = A[8*i+5][8*j+6];
				B[8*j+7][8*i+5] = A[8*i+5][8*j+7];
				B[8*j+0][8*i+5] = A[8*i+5][8*j+0];
				B[8*j+1][8*i+5] = A[8*i+5][8*j+1];

				B[8*j+7][8*i+6] = A[8*i+6][8*j+7];
				B[8*j+0][8*i+6] = A[8*i+6][8*j+0];
				B[8*j+1][8*i+6] = A[8*i+6][8*j+1];
				B[8*j+6][8*i+6] = A[8*i+6][8*j+6];

				B[8*j+0][8*i+7] = A[8*i+7][8*j+0];
				B[8*j+1][8*i+7] = A[8*i+7][8*j+1];
				B[8*j+6][8*i+7] = A[8*i+7][8*j+6];
				B[8*j+7][8*i+7] = A[8*i+7][8*j+7];
			}else{
				//rest blocks
				//cut each block into 4 small blocks
				for(k= 0; k< 4; k++)
				for(l= 0; l< 4; l++)
					B[8*j+l][8*i+k] = A[8*i+k][8*j+l];
				for(k= 0; k< 4; k++){
					B[8*j+2][8*i+k+4] = A[8*i+k][8*j+4];
					B[8*j+3][8*i+k+4] = A[8*i+k][8*j+5];
					B[8*j+0][8*i+k+4] = A[8*i+k][8*j+6];
					B[8*j+1][8*i+k+4] = A[8*i+k][8*j+7];
				}
				for(k= 4; k< 6; k++)
				for(l= 0; l< 4; l++)
					B[8*j+k][8*i+l] = B[8*j+k-2][8*i+l+4];
				for(k= 4; k< 8; k++)
				for(l= 2; l< 6; l++)
					B[8*j+l][8*i+k] = A[8*i+k][8*j+l];
				for(k= 6; k< 8; k++)
				for(l= 0; l< 4; l++)
					B[8*j+k][8*i+l] = B[8*j+k-6][8*i+l+4];
				for(k= 4; k< 8; k++)
				for(l= 0; l< 2; l++)
					B[8*j+l][8*i+k] = A[8*i+k][8*j+l];
				for(k= 4; k< 8; k++)
				for(l= 6; l< 8; l++)
					B[8*j+l][8*i+k] = A[8*i+k][8*j+l];
			}
		}
		}
	}else if(M == 61 && N == 67){
		//8*8 blocks in general , 5*8 3*8 for the rest block
		for(i= 0; i< 8; i++){
		for(j= 0; j< 8; j++){
			//since the sequence of the sets in the block is very disordered, so it is good to consider different cases specially
			if((i==0&&j==0) || (i==5&&j==6)){
				//6*6 2*2 and 6*2 block
				for(l= 0; l< 6; l++)
				for(k= 0; k< 6; k++)
					B[8*j+l][8*i+k] = A[8*i+k][8*j+l];
				for(k= 0; k< 6; k++)
				for(l= 6; l< 8; l++)
					B[8*j+l][8*i+k] = A[8*i+k][8*j+l];
				for(k= 6; k< 8; k++)
				for(l= 0; l< 6; l++)
					B[8*j+l][8*i+k] = A[8*i+k][8*j+l];
				for(k= 6; k< 8; k++)
				for(l= 6; l< 8; l++)
					B[8*j+l][8*i+k] = A[8*i+k][8*j+l];
			}else if((i==3&&j==6) || (i==5&&j==6)){
				//2*2 6*6 and 6*2 block
				for(l= 0; l< 2; l++)
				for(k= 0; k< 2; k++)
					B[8*j+l][8*i+k] = A[8*i+k][8*j+l];
				for(k= 0; k< 2; k++)
				for(l= 2; l< 8; l++)
					B[8*j+l][8*i+k] = A[8*i+k][8*j+l];
				for(k= 2; k< 8; k++)
				for(l= 0; l< 2; l++)
					B[8*j+l][8*i+k] = A[8*i+k][8*j+l];
				for(k= 2; k< 8; k++)
				for(l= 2; l< 8; l++)
					B[8*j+l][8*i+k] = A[8*i+k][8*j+l];
			}else if((i==0&&j==6) || (i==1&&j==4) || (i==2&&j==2) || (i==3&&j==0) || (i==4&&j==4) || (i==5&&j==2) || (i==6&&j==0) || (i==7&&j==2)){
				//5*5 3*3 and 5*3 block
				for(l= 0; l< 5; l++)
				for(k= 0; k< 5; k++)
					B[8*j+l][8*i+k] = A[8*i+k][8*j+l];
				for(k= 0; k< 5; k++)
				for(l= 5; l< 8; l++)
					B[8*j+l][8*i+k] = A[8*i+k][8*j+l];
				for(k= 5; k< 8; k++)
				for(l= 0; l< 5; l++)
					B[8*j+l][8*i+k] = A[8*i+k][8*j+l];
				for(k= 5; k< 8; k++)
				for(l= 5; l< 8; l++)
					B[8*j+l][8*i+k] = A[8*i+k][8*j+l];
			}else if((i==0&&j==3) || (i==1&&j==1) || (i==5&&j==1) || (i==6&&j==4)){
				//3*3 5*5 and 5*3 block
				for(l= 0; l< 3; l++)
				for(k= 0; k< 3; k++)
					B[8*j+l][8*i+k] = A[8*i+k][8*j+l];
				for(k= 0; k< 3; k++)
				for(l= 3; l< 8; l++)
					B[8*j+l][8*i+k] = A[8*i+k][8*j+l];
				for(k= 3; k< 8; k++)
				for(l= 0; l< 3; l++)
					B[8*j+l][8*i+k] = A[8*i+k][8*j+l];
				for(k= 3; k< 8; k++)
				for(l= 3; l< 8; l++)
					B[8*j+l][8*i+k] = A[8*i+k][8*j+l];
			}else if(j != 7){
				//j == 7 is the 5*8 block, this will be moved later
				for(l= 0; l< 4; l++)
				for(k= 0; k< 4; k++)
					B[8*j+l][8*i+k] = A[8*i+k][8*j+l];
				for(k= 4; k< 8; k++)
				for(l= 0; l< 4; l++)
					B[8*j+l][8*i+k] = A[8*i+k][8*j+l];
				for(k= 0; k< 4; k++)
				for(l= 4; l< 8; l++)
					B[8*j+l][8*i+k] = A[8*i+k][8*j+l];
				for(k= 4; k< 8; k++)
				for(l= 4; l< 8; l++)
					B[8*j+l][8*i+k] = A[8*i+k][8*j+l];
			}else{
				//5*8 block
				for(l= 0; l< 5; l++)
				for(k= 0; k< 8; k++)
					B[8*j+l][8*i+k] = A[8*i+k][8*j+l];
			}
		}
		}
		for(j= 0; j< 61; j++){
			//3*8 block
			if(j == 60){
				B[j][66] = A[66][j];
				B[j][65] = A[65][j];
				B[j][64] = A[64][j];
			}else{
				B[j][64] = A[64][j];
				B[j][65] = A[65][j];
				B[j][66] = A[66][j];
			}
		}
	}else{
		//cases other than 32*32 64*64 and 61*67
		for (i = 0; i < N; i++) {
			for (j = 0; j < M; j++) {
				B[j][i] = A[i][j];
			}
		}    
	}
}

/* 
 * You can define additional transpose functions below. We've defined
 * a simple one below to help you get started. 
 */ 

/* 
 * trans - A simple baseline transpose function, not optimized for the cache.
 */
char trans_desc[] = "Simple row-wise scan transpose";
void trans(int M, int N, int A[N][M], int B[M][N])
{
    int i, j, tmp;

    for (i = 0; i < N; i++) {
        for (j = 0; j < M; j++) {
            tmp = A[i][j];
            B[j][i] = tmp;
        }
    }    

}

/*
 * registerFunctions - This function registers your transpose
 *     functions with the driver.  At runtime, the driver will
 *     evaluate each of the registered functions and summarize their
 *     performance. This is a handy way to experiment with different
 *     transpose strategies.
 */
void registerFunctions()
{
    /* Register your solution function */
    registerTransFunction(transpose_submit, transpose_submit_desc); 

    /* Register any additional transpose functions */
    registerTransFunction(trans, trans_desc); 

}

/* 
 * is_transpose - This helper function checks if B is the transpose of
 *     A. You can check the correctness of your transpose by calling
 *     it before returning from the transpose function.
 */
int is_transpose(int M, int N, int A[N][M], int B[M][N])
{
    int i, j;

    for (i = 0; i < N; i++) {
        for (j = 0; j < M; ++j) {
            if (A[i][j] != B[j][i]) {
                return 0;
            }
        }
    }
    return 1;
}

