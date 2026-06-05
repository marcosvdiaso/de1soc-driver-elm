#include "driver.h"
#include "vga.h"
#include <stdio.h>
#include <stdint.h>
#include "mouse.h"

int main(){
    int e, r;
    uint8_t img[784];
    uint8_t draw[784];

    for (int i = 0; i < 784; i++){
        draw[i] = 0;
    }

    printf("digito predito esperado: ");
    scanf("%d", &e);

    if (elm_open() < 0) {
        printf("erro abrir\n");
        return -1;
    }

    elm_reset();

    if (elm_load() < 0) {
        printf("erro load\n");
        elm_close();
        return -1;
    }

    vga_start();
    vga_reset();
    vga_drawing(draw);
    init_mouse(draw);

    FILE *f = fopen("archives/images/image.bin", "rb");
    if (f) {
        if (fread(img, 1, 784, f) == 784) {
            vga_draw(img);
            printf("imagem foi\n");
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

    r = elm_result();
    printf("a imagem foi inferida como: %d\n", r);
    printf("era esperado: %d", e);


    elm_close();
    return 0;
}