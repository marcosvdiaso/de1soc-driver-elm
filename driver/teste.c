#include "driver.h"
#include "vga.h"
#include <stdio.h>
#include <stdint.h>

int main(void)
{
    int esperado, resultado;
    uint8_t img[784];

    printf("Digito predito esperado: ");
    if (scanf("%d", &esperado) != 1) {
        printf("Entrada invalida.\n");
        return -1;
    }

    if (elm_open() < 0) {
        printf("Erro ao abrir /dev/mem\n");
        return -1;
    }

    elm_reset();

    if (elm_load() < 0) {
        printf("Erro ao carregar arquivos.\n");
        elm_close();
        return -1;
    }

    vga_start();

    FILE *f = fopen("archives/images/image.bin", "rb");
    if (f) {
        if (fread(img, 1, 784, f) == 784) {
            vga_draw(img);
            printf("imagem foi");
        } else {
            printf("erro acessar bytes imagem");
        }
        fclose(f);
    } else {
        printf("error abrir imagens");
    }

    if (elm_start() < 0) {
        printf("Erro na inferencia\n");
        elm_close();
        return -1;
    }

    resultado = elm_result();
    printf("------------------------------------------\n");
    printf("A imagem foi inferida como: %d\n", resultado);
    printf("Digito esperado: %d -> %s\n",
           esperado, (resultado == esperado) ? "ACERTOU" : "ERROU");
    printf("------------------------------------------\n");

    elm_close();
    return 0;
}