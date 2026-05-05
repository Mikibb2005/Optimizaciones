/*
 * Sudoku solver combinado — dos versiones en un único binario.
 *
 * Compilar:
 *   gcc -O2 -fopenmp -o sudoku main.c
 *
 * Uso:
 *   ./sudoku 1   →  Versión OpenMP (tareas paralelas, primera fila fija)
 *   ./sudoku 2   →  Versión serie   (bloque 3×3 superior-izquierdo fijo)
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <omp.h>

#define N 9

/* ================================================================
   VERSIÓN 1 — OpenMP con tareas paralelas
   Estrategia: fija la primera fila (1–9) y divide el trabajo
   creando una tarea OMP por cada valor posible en t[1][0].
   ================================================================ */

static int es_valido_v1(int t[N][N], int f, int c, int num)
{
    for (int i = 0; i < N; i++)
        if (t[f][i] == num || t[i][c] == num)
            return 0;

    int bf = (f / 3) * 3, bc = (c / 3) * 3;
    for (int i = bf; i < bf + 3; i++)
        for (int j = bc; j < bc + 3; j++)
            if (t[i][j] == num)
                return 0;
    return 1;
}

static void resolver_v1(int t[N][N], int pos, long long *count)
{
    if (pos == 81) {
        (*count)++;
        return;
    }

    int f = pos / 9;
    int c = pos % 9;

    /* Celda ya fijada → avanzar */
    if (t[f][c] != 0) {
        resolver_v1(t, pos + 1, count);
        return;
    }

    for (int num = 1; num <= 9; num++) {
        if (es_valido_v1(t, f, c, num)) {
            t[f][c] = num;
            resolver_v1(t, pos + 1, count);
            t[f][c] = 0;
            printf("[v1] tablero[%d][%d] = %d  →  subtotal local = %lld\n",
                   f, c, num, *count);
        }
    }
}

static void run_v1(void)
{
    long long res = 0;

    #pragma omp parallel
    {
        #pragma omp single
        {
            for (int num = 1; num <= 9; num++) {
                #pragma omp task firstprivate(num)
                {
                    int t[N][N] = {0};
                    long long local = 0;

                    /* Primera fila fija: 1 2 3 4 5 6 7 8 9 */
                    for (int i = 0; i < N; i++)
                        t[0][i] = i + 1;

                    /* Anclar t[1][0] para dividir la búsqueda */
                    t[1][0] = num;

                    /* Empezar desde pos=9 (la primera fila ya está completa) */
                    resolver_v1(t, 9, &local);

                    #pragma omp atomic
                    res += local;
                }
            }
        }
    }

    printf("\n[v1] Total (sin eliminar simetrias): %lld\n", res);
}

/* ================================================================
   VERSIÓN 2 — Recursión serie con tablero global
   Estrategia: fija el bloque 3×3 superior-izquierdo (valores 1–9)
   y deja que el backtracking complete el resto.
   ================================================================ */

static int tablero_v2[N][N];
static long long res_v2;

static int es_valido_v2(int fila, int col, int num)
{
    for (int i = 0; i < N; i++)
        if (tablero_v2[fila][i] == num)
            return 0;

    for (int j = 0; j < N; j++)
        if (tablero_v2[j][col] == num)
            return 0;

    int cf = (fila / 3) * 3, cc = (col / 3) * 3;
    
    for (int i = cf; i < cf + 3; i++)
        for (int j = cc; j < cc + 3; j++)
            if (tablero_v2[i][j] == num)
                return 0;
    return 1;
}

static void resolver_rec_v2(int pos)
{
    if (pos == 81) {
        res_v2++;
        return;
    }

    int fila = pos / 9;
    int col  = pos % 9;

    /* Celda ya fijada → avanzar */
    if (tablero_v2[fila][col] != 0) {
        resolver_rec_v2(pos + 1);
        return;
    }

    for (int num = 1; num <= 9; num++) {
        if (es_valido_v2(fila, col, num)) {
            tablero_v2[fila][col] = num;
            resolver_rec_v2(pos + 1);
            tablero_v2[fila][col] = 0;
            printf("[v2] tablero[%d][%d] = %d  →  total acumulado = %lld\n",
                   fila, col, num, res_v2);
        }
    }
}

static void run_v2(void)
{
    memset(tablero_v2, 0, sizeof(tablero_v2));
    res_v2 = 0;

    /*
     * Fijar bloque 3×3 superior-izquierdo:
     *   1 2 3
     *   4 5 6
     *   7 8 9
     */
    int val = 1;
    for (int i = 0; i < 3; i++)
        for (int j = 0; j < 3; j++)
            tablero_v2[i][j] = val++;

    resolver_rec_v2(0);

    printf("\n[v2] Hay %lld sudokus validos 9×9\n", res_v2);
}

/* ================================================================
   MAIN — selección de versión por argumento
   ================================================================ */

int main(int argc, char *argv[])
{
    if (argc < 2) {
        fprintf(stderr,
            "Uso: %s <version>\n"
            "  1  →  OpenMP paralelo (primera fila fija, tareas por t[1][0])\n"
            "  2  →  Serie          (bloque 3×3 fijo, backtrack global)\n",
            argv[0]);
        return EXIT_FAILURE;
    }

    int version = atoi(argv[1]);

    switch (version) {
        case 1:
            printf("=== Ejecutando versión 1 (OpenMP) ===\n\n");
            run_v1();
            break;
        case 2:
            printf("=== Ejecutando versión 2 (serie) ===\n\n");
            run_v2();
            break;
        default:
            fprintf(stderr, "Versión desconocida: %d  (usa 1 o 2)\n", version);
            return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
