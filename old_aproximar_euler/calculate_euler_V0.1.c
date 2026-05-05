#include <stdio.h>
#include <string.h>
#include <omp.h>
//#include "/home/miki/Documentos/cosas/aproximar_euler/mpfr-4.2.2/src/mpfr.h"
//e=∑(1/n!)
#define N 10000

long double calculate_euler()
{
    long double e = 1.0;
    //long double n = 1.0;
    //float n = 0;
    int i = 1;

    long double n[N+1];
    n[0] = 1.0;
    for(i = 1; i <= N; i++)
        n[i] = n[i-1]*i;

    #pragma omp parallel for reduction(+:e) num_threads(12)
    for(i = 1; i < N; i++)
    {
        e += 1.0L/n[i];
        //printf("Thread: %d \n", omp_get_thread_num());
    }

    return e;

}


long double main()
{
    long double res = 0.0;
    
    res = calculate_euler();
    printf("Euler: %.15LF \n", res);
    //mpfr_printf("%.1000Rf\n", res);
    
    return 0;
}