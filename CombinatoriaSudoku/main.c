#include <stdio.h>
#include <string.h>
#include <omp.h>

#define N 9

long long total = 0;

// ================= VALIDACIÓN =================
int es_valido(int t[N][N], int f, int c, int num)
{
    for(int i = 0; i < N; i++)
        if(t[f][i] == num || t[i][c] == num)
            return 0;

    int bf = (f/3)*3, bc = (c/3)*3;

    for(int i = bf; i < bf+3; i++)
        for(int j = bc; j < bc+3; j++)
            if(t[i][j] == num)
                return 0;

    return 1;
}

// ================= SERIALIZAR =================
void serializar(int t[N][N], char *out)
{
    int k = 0;
    for(int i = 0; i < N; i++)
        for(int j = 0; j < N; j++)
            out[k++] = t[i][j] + '0';
    out[81] = '\0';
}

// ================= CANONICALIZAR (solo dígitos) =================
void canonicalizar(int t[N][N], char *out)
{
    int map[10] = {0};
    int next = 1;

    int temp[N][N];

    for(int i = 0; i < N; i++)
    {
        for(int j = 0; j < N; j++)
        {
            int v = t[i][j];
            if(map[v] == 0)
                map[v] = next++;
            temp[i][j] = map[v];
        }
    }

    serializar(temp, out);
}

// ================= BACKTRACK =================
void resolver(int t[N][N], int pos, long long *count)
{
    if(pos == 81)
    {
        char canon[82];
        canonicalizar(t, canon);

        // Aquí deberías usar un hash set global real
        // Para simplificar: contamos todo (placeholder)
        (*count)++;
        return;
    }

    int f = pos / 9;
    int c = pos % 9;

    if(t[f][c] != 0)
    {
        resolver(t, pos + 1, count);
        return;
    }

    for(int num = 1; num <= 9; num++)
    {
        if(es_valido(t, f, c, num))
        {
            t[f][c] = num;
            resolver(t, pos + 1, count);
            t[f][c] = 0;
        }
    }
}

// ================= MAIN =================
int main()
{
    long long res = 0;

    #pragma omp parallel
    {
        #pragma omp single
        {
            for(int num = 1; num <= 9; num++)
            {
                #pragma omp task firstprivate(num)
                {
                    int t[N][N] = {0};
                    long long local = 0;

                    // fijar primera fila
                    for(int i = 0; i < 9; i++)
                        t[0][i] = i+1;

                    // segunda fila (división de trabajo)
                    t[1][0] = num;

                    resolver(t, 0, &local);

                    #pragma omp atomic
                    res += local;
                }
            }
        }
    }

    printf("Total (sin eliminar duplicados reales): %lld\n", res);
    return 0;
}
