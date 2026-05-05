#include <mpfr.h>
#include <stdio.h>
#include <stdlib.h>

/**
 * Calcula el número e con la precisión deseada usando la librería MPFR.
 * e = 1 + 1/1! + 1/2! + 1/3! + ...
 */
void calculate_euler(long digits) {
  // Estimación de bits necesarios (aprox 3.32 bits por dígito decimal)
  // Añadimos un margen extra para evitar errores de redondeo acumulados.
  mpfr_prec_t precision = (mpfr_prec_t)(digits * 3.321928) + 128;
  mpfr_set_default_prec(precision);

  mpfr_t e, term;
  mpfr_init_set_ui(e, 1, MPFR_RNDN);    // e = 1 (primer término n=0)
  mpfr_init_set_ui(term, 1, MPFR_RNDN); // term = 1/0! = 1

  printf("Calculando e con %ld decimales...\n", digits);

  unsigned long n = 1;
  while (1) {
    // term = term / n  =>  esto va calculando 1/1!, 1/2!, 1/3!...
    mpfr_div_ui(term, term, n, MPFR_RNDN);

    // Si el término es tan pequeño que no afecta a la precisión pedida, paramos
    if (mpfr_zero_p(term) || mpfr_get_exp(term) < -(long)(digits * 3.321928)) {
      break;
    }

    mpfr_add(e, e, term, MPFR_RNDN);
    n++;

    // Progreso opcional para cálculos muy largos
    if (n % 1000 == 0) {
      printf("\rTermino actual: %lu", n);
      fflush(stdout);
    }
  }

  // Imprimir el resultado de forma bonita
  printf("\nResultado (%ld decimales):\n", digits);
  char *s = NULL;
  // mpfr_asprintf genera un string con el formato deseado
  mpfr_asprintf(&s, "%.*Rf", (int)digits, e);

  if (s) {
    printf("2.\n"); // Parte entera
    // Empezamos desde el índice 2 para saltar "2."
    for (long i = 2; s[i] != '\0'; i++) {
      putchar(s[i]);
      if ((i - 1) % 50 == 0)
        printf("  [%4ld]\n", i - 1);
      else if ((i - 1) % 10 == 0)
        printf(" ");
    }
    printf("\n");
    mpfr_free_str(s);
  }

  mpfr_clears(e, term, NULL);
}

int main(int argc, char *argv[]) {
  long digits = 2000; // Valor por defecto

  if (argc > 1) {
    digits = atol(argv[1]);
  }

  if (digits <= 0) {
    printf("Por favor, introduce un número válido de decimales.\n");
    return 1;
  }

  calculate_euler(digits);

  return 0;
}
