#include <gmp.h>
#include <stdio.h>
#include <time.h>
#include <omp.h>

// Configuración de precisión y niveles
#define BITS_PRECISION 100000 
#define NIVELES 1000000
#define DECIMALES_MUESTRA 50

int main() {
    #pragma omp parallel num_threads(16)
    // Configurar precisión por defecto para los cálculos
    mpf_set_default_prec(BITS_PRECISION);

    mpf_t pi, termino, suma;
    mpf_init(pi);
    mpf_init(termino);
    mpf_init(suma);
    mpf_set_ui(suma, 0);

    //printf("Calculando PI de forma iterativa (Leibniz)...\n");
    
    clock_t start = clock();
    
    // Bucle iterativo de la serie de Leibniz
    long i = 0;
    #pragma omp for //reduction(+:suma)
    for (i = 0; i < NIVELES; i++) {
        // termino = 1 / (2*i + 1)
        mpf_set_ui(termino, 1);
        mpf_div_ui(termino, termino, 2 * i + 1);

        // Alternar signo: si i es impar, restar; si es par, sumar
        if (i % 2 != 0) {
            mpf_sub(suma, suma, termino);
        } else {
            mpf_add(suma, suma, termino);
        }
    }

    // pi = suma * 4
    mpf_mul_ui(pi, suma, 4);

    clock_t end = clock();
    double tiempo = (double)(end - start) / CLOCKS_PER_SEC;

    // Imprimir resultado final
    printf("Resultado:\n");
    gmp_printf("%.*Ff\n", DECIMALES_MUESTRA, pi);
    printf("\nTiempo total de cómputo (CPU): %f segundos\n", tiempo);

    // Limpiar memoria
    mpf_clear(pi);
    mpf_clear(termino);
    mpf_clear(suma);

    return 0;
}
