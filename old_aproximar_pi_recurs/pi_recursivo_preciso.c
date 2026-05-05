#include <gmp.h>
#include <omp.h>
#include <stdio.h>

// Define cuántos BITS de precisión quieres (ej: 3322 bits ≈ 1000 decimales)
#define BITS_PRECISION 10
// Cuántos niveles de la serie calcular
#define NIVELES 10
// Cuántos decimales mostrar en el printf final
#define DECIMALES_MUESTRA 10

void calcular_tramo_leibniz(mpf_t resultado, long a, long b) {
  printf("Numero thread %d\n", omp_get_num_threads());
  if (a == b) {
    mpf_t temp;
    mpf_init(temp);

    // termino = 1 / (2a + 1)
    mpf_set_ui(temp, 1);
    mpf_div_ui(temp, temp, 2 * a + 1);

    // Alternar signo
    if (a % 2 != 0)
      mpf_neg(temp, temp);

    mpf_set(resultado, temp);
    mpf_clear(temp);
  } else {
    long m = (a + b) / 2;
    mpf_t res_derecha;
    mpf_init(res_derecha);

    // Dividimos el rango en dos mitades (recursión binaria)
    // Usamos un umbral para no crear demasiadas tareas
    if (b - a > 1000) {
#pragma omp task shared(resultado)
      calcular_tramo_leibniz(resultado, a, m);
#pragma omp task shared(res_derecha)
      calcular_tramo_leibniz(res_derecha, m + 1, b);
#pragma omp taskwait
    } else {
#pragma omp task shared(resultado)
      calcular_tramo_leibniz(resultado, a, m);
#pragma omp task shared(res_derecha)
      calcular_tramo_leibniz(res_derecha, m + 1, b);
#pragma omp taskwait
    }

    mpf_add(resultado, resultado, res_derecha);
    mpf_clear(res_derecha);
  }
}

int main() {
  // Configurar precisión por defecto
  mpf_set_default_prec(BITS_PRECISION);

  double start_time = omp_get_wtime();

  mpf_t pi;
  mpf_init(pi);

// Cálculo recursivo
#pragma omp parallel num_threads(2)
#pragma omp master // o single o master
  calcular_tramo_leibniz(pi, 0, NIVELES);
  
  // pi = resultado * 4
  mpf_mul_ui(pi, pi, 4);

  // Imprimir resultado final
  gmp_printf("%.*Ff\n", DECIMALES_MUESTRA, pi);
  printf("Numero thread %d\n", omp_get_num_threads());

  double end_time = omp_get_wtime();
  printf("\nTiempo total de cómputo: %f segundos\n", end_time - start_time);

  mpf_clear(pi);
  return 0;
}
