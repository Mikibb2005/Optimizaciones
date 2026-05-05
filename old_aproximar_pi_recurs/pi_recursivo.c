#include <stdio.h>
#include <time.h>

#define NIVELES 150000000

double suma_leibniz(int n) {
  if (n == 0)
    return 1.0;
  double signo = (n % 2 == 0) ? 1.0 : -1.0;
  return (signo / (2.0 * n + 1.0)) + suma_leibniz(n - 1);
}

int main() {
  struct timespec start, end;
  clock_gettime(CLOCK_MONOTONIC, &start);

  double pi = 4.0 * suma_leibniz(NIVELES);
  printf("%.99f\n", pi);

  clock_gettime(CLOCK_MONOTONIC, &end);
  double time_taken =
      (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
  printf("\nTiempo total de cómputo: %f segundos\n", time_taken);

  return 0;
}
