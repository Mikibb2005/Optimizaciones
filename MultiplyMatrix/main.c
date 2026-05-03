#include <omp.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>

#define N 1000
#define NUM 200

int matrix1[N][N];
int matrix2[N][N];
int mat_res[N][N];


void initMatrix()
{
	for(int i = 0; i < N; i++)
	{
		for(int j = 0; j < N; j++)
		{
			matrix1[i][j] = 0;
			matrix2[i][j] = 0;
			mat_res[i][j] = 0;
		}
	}

}

void printMatrix(int mat[N][N])
{
	for(int i = 0; i < N; i++)
	{
		for(int j = 0; j < N; j++)
		{
			printf(" [%d] ", mat[i][j]);
		}
		printf("\n");
	}
	printf("\n");
}

void randomizeMatrix(int mat[N][N])
{
	for(int i = 0; i < N; i++)
	{
		for(int j = 0; j < N; j++)
		{
			mat[i][j] = rand() % 999 + 1;
		}
	}
}

void multiply_matrix_seq()
{
	for(int i = 0; i < N; i++)
	{
		for(int j = 0; j < N; j++)
		{
			mat_res[i][j] = matrix1[i][j] * matrix2[i][j];
		}
	}	
}


void multiply_matrix_par()
{
	#pragma omp parallel for collapse(2)
	for(int i = 0; i < N; i++)
	{
		for(int j = 0; j < N; j++)
		{
			mat_res[i][j] = matrix1[i][j] * matrix2[i][j];
		}
	}	
}



int main(int argc, char *argv[])
{
	srand(time(NULL));
	
	int use_omp = 0;
    if(argc > 1)
        use_omp = atoi(argv[1]); // 0 = sec, 1 = omp
	
	double start = omp_get_wtime();
	
	for(int k = 0; k < NUM; k++)
	{
		//printf("Iteracion n%d \n\n", k);
		initMatrix();
		randomizeMatrix(matrix1);
		randomizeMatrix(matrix2);
		
		if(use_omp)
            multiply_matrix_par();
        else
            multiply_matrix_seq();
		
		/*
		printf("Primera matriz: \n");
		printMatrix(matrix1);
		printf("Segunda matriz: \n");
		printMatrix(matrix2);
		
		printf("Matriz Resultante: \n");
		printMatrix(mat_res);
		*/
	}
	
	double end = omp_get_wtime();

    printf("time=%f\n", end - start);

	return 0;
}
