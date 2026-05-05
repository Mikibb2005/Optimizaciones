//#include <iostream>
#include <stdio.h>

int tablero[9][9] = {0};
long long res = 0;



int es_valido(int fila, int col, int num)
{
	for(int i = 0; i < 9; i++)
	{
		if(tablero[fila][i] == num)
		{
			return 0;
		}
	}
	
	for(int j = 0; j < 9; j++)
	{
		if(tablero[j][col] == num)
		{
			return 0;
		}
	}
	
	int caja_fila = (fila / 3) * 3;
	int caja_col = (col / 3) * 3;
	
	//#pragma omp parallel for collapse(2)
	for(int i = caja_fila; i < caja_fila + 3; i++)
	{
		for(int j = caja_col; j < caja_col + 3; j++)
		{
			if(tablero[i][j] == num)
			{
				return 0;
			}
		}
	}
	return 1;	
}

void resolver_rec(int pos)
{
	if(pos == 81)
	{
		res++;
		return;
	}
	
	int fila = pos / 9;
	int col = pos % 9;
	
	for(int num = 1; num <= 9; num++)
	{
		if(es_valido(fila, col, num))
		{
			tablero[fila][col] = num;
			resolver_rec(pos + 1);
			tablero[fila][col] = 0;
			printf("puesto: tablero[%d][%d] = %d --> entonces RESULTADO = %lld\n", fila, col, num, res);
		}
	}	
}


int main()
{
	// fijar primera fila
	for(int i = 0; i < 9; i++)
		tablero[0][i] = i + 1;

	// fijar primer bloque
	int val = 1;
	for(int i = 0; i < 3; i++)
		for(int j = 0; j < 3; j++)
		    tablero[i][j] = val++;

	resolver_rec(tablero, 9, &res);
	printf("Hay %lld sudokus validos 9x9", res);
	return 0;
}
