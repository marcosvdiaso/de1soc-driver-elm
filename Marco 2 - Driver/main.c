#include "driver.h"
#include <stdio.h>

int main(void) {
    int r, d;

    printf("Digito predito esperado: ");
    scanf("%d", &d);

    if (elm_open() < 0) {
        printf("Erro ao abrir /dev/mem\n");
        return -1;
    }

    elm_reset();

    if (elm_start() < 0) {
        printf("Erro na inferencia\n");
        elm_close();
        return -1;
    }

    r = elm_result();

    printf("Resultado: %d\n", r);
    printf("Esperado: %d\n", d);
    printf("Correto: %s\n", r == d ? "SIM" : "NAO");

    elm_close();
    return 0;
}