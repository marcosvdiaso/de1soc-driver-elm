#include "driver.h"
#include <stdio.h>
#include <time.h>
#include <math.h>

// https://man7.org/linux/man-pages/man3/clock_gettime.3.html

int main(void) {
    int result, digit, test;
    int ok = 0;
    float rob;
    struct timespec t1_lat, t2_lat;
    long lat = 0;
    double s, thr, diff, jitter;
    double var = 0;

    printf("Digito predito esperado: ");
    scanf("%d", &digit);

    printf("Quantas vezes deseja rodar o teste? ");
    scanf("%d", &test);

    long lats[test];

    if (elm_open() < 0) {
        printf("Erro ao abrir /dev/mem\n");
        return -1;
    }

    elm_reset();

    for (int i = 0; i < test; i++){
      clock_gettime(CLOCK_MONOTONIC, &t1_lat);
      if (elm_start() < 0) {
        printf("Erro na inferencia nº %d\n", i+1);
        continue;
      }
      clock_gettime(CLOCK_MONOTONIC, &t2_lat);
      lats[i] = (t2_lat.tv_sec - t1_lat.tv_sec) * 1e9 + (t2_lat.tv_nsec - t1_lat.tv_nsec);
      lat += lats[i];
      result = elm_result();
      if (result == digit) ok++;
      printf("Digito predito na inferência de nº %d = %d\n", i+1, result);
    }

    
    rob = (ok *100.0f)/ test;
    lat /= test;
    s = lat / 1e9;
    thr = test / s;
    
    for (int i =0; i < test; i++){
      diff = lats[i] - lat;
      var += diff * diff;
    }
    jitter = sqrt(var/test);

    printf("------------------------------------------\n");
    printf("MÉTRICAS DOS TESTES:\n");
    printf("------------------------------------------\n");
    printf("Robustez: %.1f%%\n", rob);
    printf("Latência: %ld ns\n", lat);
    printf("Throughput: %.2f inferencias/s\n", thr);
    printf("Desvio padrão: %.0f ns\n", jitter);

    elm_close();
    return 0;
}