#include "driver.h"
#include <stdio.h>

int main(void) {
    int result, digit, test;
    int ok = 0;
    float rob;

    printf("Digito predito esperado: ");
    scanf("%d", &digit);

    printf("Quantas vezes deseja rodar o teste? ");
    scanf("%d", &test);

    if (elm_open() < 0) {
        printf("Erro ao abrir /dev/mem\n");
        return -1;
    }

    elm_reset();

    for (int i = 0; i < test; i++){
      if (elm_start() < 0) {
        printf("Erro na inferencia nº %d\n", i+1);
        continue;
      }
      result = elm_result();
      if (result == digit) ok++;
      printf("Digito predito na inferência de nº %d = %d\n", i+1, result);
    }

    rob = (ok *100.0f)/ test;

    printf("------------------------------------------\n");
    printf("MÉTRICAS DOS TESTES:\n");
    printf("------------------------------------------\n");
    printf("Robustez: %.1f%%\n", rob);

    elm_close();
    return 0;
}