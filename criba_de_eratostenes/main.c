    #include <stdlib.h>
    #include <stdio.h>
    #include <omp.h>


    //#define N 1000

    int es_primo(int n)
    {
        if(n < 2) return 0;

        for(int i = 2; i * i <= n; i++)
        {
            if(n % i == 0)
            {
                return 0;
            }
        }
        return 1;
    }



    int main(int argc, char *argv[])
    {

        if(argc < 2)
        {
            printf("Uso: %s N\n", argv[0]);
            return 1;
        }

        int N = atoi(argv[1]);

        int *primos = malloc(N * sizeof(int));
        int n_primos = 0;
        int i = 0;
        #pragma omp parallel for
        for(i = 0; i <= N; i++)
        {
            if(es_primo(i))
            {
                int idx;
                #pragma omp atomic capture
                idx = n_primos++;
                primos[idx] = i;
            }
        }




        for(int i = 0; i < n_primos; i++)
        {
            printf("  %d  \n", primos[i]);
        }
        printf("Numero de primos: %d\n", n_primos);

        free(primos);
        return 0; 
    }
