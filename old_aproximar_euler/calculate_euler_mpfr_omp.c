#include <math.h>
#include <mpfr.h>
#include <omp.h>
#include <stdio.h>
#include <stdlib.h>

/**
 * Para calcular e con millones de decimales de forma eficiente y paralela,
 * usamos el algoritmo de Binary Splitting.
 * Calcula el producto de (1/i) para i de a+1 a b.
 * P(a,b) = Producto de i para i desde a+1 hasta b.
 * T(a,b) = Suma de los terminos desde a+1 hasta b multiplicada por P(a,b).
 */

typedef struct {
  mpz_t P;
  mpz_t T;
} bsplit_t;

void bsplit(unsigned long a, unsigned long b, bsplit_t *res) {
  if (b - a == 1) {
    mpz_set_ui(res->P, b);
    mpz_set_ui(res->T, 1);
    return;
  }

  unsigned long m = a + (b - a) / 2;
  bsplit_t left, right;

  mpz_inits(left.P, left.T, right.P, right.T, NULL);

  // Si el rango es suficientemente grande, paralelizamos
  // Usamos un umbral para evitar el overhead de hilos en rangos pequeños
  if (b - a > 5000 && b - a > (unsigned long)(100000 / omp_get_max_threads())) {
#pragma omp task shared(left)
    bsplit(a, m, &left);

#pragma omp task shared(right)
    bsplit(m, b, &right);

#pragma omp taskwait
  } else {
    bsplit(a, m, &left);
    bsplit(m, b, &right);
  }

  // T(a,b) = T(a,m) * P(m,b) + T(m,b)
  mpz_mul(res->T, left.T, right.P);
  mpz_add(res->T, res->T, right.T);

  // P(a,b) = P(a,m) * P(m,b)
  mpz_mul(res->P, left.P, right.P);

  mpz_clears(left.P, left.T, right.P, right.T, NULL);
}

void calculate_euler_parallel(long digits) {
  // Estimación de bits
  mpfr_prec_t precision = (mpfr_prec_t)(digits * 3.321928) + 128;
  mpfr_set_default_prec(precision);

  // Determinar cuántos términos necesitamos (n! > 10^digits)
  // Para n grande, Stirling dice log10(n!) approx n log10(n/e)
  // Aproximación rápida:
  unsigned long n_terms = 50;
  if (digits > 20) {
    n_terms = (unsigned long)(digits / 0.4); // Aproximación inicial
    // Ajustar n_terms para que sea más preciso si es necesario
  }

  // En un cálculo real serio buscaríamos el n exacto, para e con Stirling
  // basta. Para 1000 decimales, n=450 es suficiente. Para 1 millón, n=200,000.
  // Ponemos un margen de seguridad cómodo.
  n_terms = (unsigned long)(digits * 1.1) + 100;
  if (digits > 10000)
    n_terms = (unsigned long)(digits / (log10(digits / 3.0))) + 1000;

  printf("Calculando e con %ld decimales usando Binary Splitting y OpenMP...\n",
         digits);
  printf("Usando %d hilos. Terminos estimados: %lu\n", omp_get_max_threads(),
         n_terms);

  bsplit_t result;
  mpz_inits(result.P, result.T, NULL);

  double start = omp_get_wtime();

#pragma omp parallel
  {
#pragma omp single
    { bsplit(0, n_terms, &result); }
  }

  // e = 1 + T(0, n_terms) / P(0, n_terms)
  mpfr_t e, t_final, p_final;
  mpfr_inits(e, t_final, p_final, NULL);

  mpfr_set_z(t_final, result.T, MPFR_RNDN);
  mpfr_set_z(p_final, result.P, MPFR_RNDN);

  mpfr_div(e, t_final, p_final, MPFR_RNDN);
  mpfr_add_ui(e, e, 1, MPFR_RNDN);

  double end = omp_get_wtime();
  double total_time = end - start;
  printf("\nCálculo completado en %.4f segundos.\n", total_time);

  // Escribir directamente al archivo para ahorrar RAM (importante para >100M
  // decimales)
  FILE *f = fopen("euler_result.json", "w");
  if (f) {
    fprintf(f, "{\n");
    fprintf(f, "  \"digits\": %ld,\n", digits);
    fprintf(f, "  \"execution_time_seconds\": %.4f,\n", total_time);
    fprintf(f, "  \"value\": \"2.");

    // mpfr_out_str escribe directamente al stream y es mucho más eficiente
    // El formato es base 10, número de dígitos.
    // Como queremos '2.' fuera, imprimimos la mantisa.
    // Pero mpfr_out_str incluye el exponente y punto.
    // Mejor usamos un pequeño buffer intermedio o mpfr_get_str

    // Para máxima eficiencia en e:
    // Restamos 1 para quedarnos solo con la parte decimal
    mpfr_sub_ui(e, e, 2, MPFR_RNDN);
    // Ahora e es 0.71828...
    // Multiplicamos por 10^digits no es eficiente.
    // Usamos mpfr_get_str que devuelve la mantisa pura.

    mpfr_exp_t exp;
    char *digits_str = mpfr_get_str(NULL, &exp, 10, digits, e, MPFR_RNDN);

    if (digits_str) {
      // Escribir los dígitos de la mantisa
      // mpfr_get_str para e=2.718 devuelve mantisa "2718..." y exp 1
      // Como restamos 2, e=0.71828... devuelve mantisa "71828..." y exp 0
      fputs(digits_str, f);

      // Mostrar resumen por consola
      printf("\nPrimeros 20 decimales:\n2.");
      for (int i = 0; i < 20 && digits_str[i] != '\0'; i++) {
        putchar(digits_str[i]);
        if ((i + 1) % 10 == 0)
          printf(" ");
      }
      printf("...\n");

      mpfr_free_str(digits_str);
    }

    fprintf(f, "\"\n}\n");
    fclose(f);
    printf("Resultado guardado en 'euler_result.json'.\n");
  } else {
    printf("Error: No se pudo crear el archivo JSON.\n");
  }

  mpz_clears(result.P, result.T, NULL);
  mpfr_clears(e, t_final, p_final, NULL);
}

int main(int argc, char *argv[]) {
  long digits = 100000000;
  if (argc > 1)
    digits = atol(argv[1]);
  if (digits <= 0) {
    printf("Numero invalido.\n");
    return 1;
  }

  calculate_euler_parallel(digits);
  return 0;
}
