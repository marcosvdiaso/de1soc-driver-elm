#include "driver.h"
#include "vga.h"
#include <stdio.h>
#include <stdint.h>
#include "mouse.h"
#include <string.h>
#include <dirent.h>

void enter() {
    printf("Pressione enter para continuar\n");
    fflush(stdout);
    
    int c;
    while ((c = getchar()) != '\n' && c != EOF) {
        continue;
    }
}   

int main(){
    uint8_t img[784];
    char path[1024] = "";
    int op, e, r;
    int eimg = 0;
    int ok = 0, wrng = 0;

    if (elm_open() < 0) {
        printf("Erro ao abrir /dev/mem\n");
        return -1;
    }
    elm_reset();

    if (elm_load() < 0) {
        printf("Erro ao carregar métricas\n");
        elm_close();
        return -1;
    }

    vga_start();
    vga_reset();

    do {
        printf("Digite o número correspondente menu: \n");
        printf("1. Inferência enviando imagem\n");
        printf("2. Inferência desenhando a imagem\n");
        printf("3. Benchmark\n");
        printf("4. Sair\n");
        scanf("%d", &op);

        if (op == 1) {
            getchar();
            fgets(path, 1024, stdin);
            path[strcspn(path, "\n")] = 0;

            FILE *f = fopen(path, "rb");
            if (f) {
                if (fread(img, 1, 784, f) == 784) {
                    vga_draw(img);
                    printf("Imagem exibida\n");
                } else {
                    printf("Erro no fread\n");
                }
                fclose(f);
            } else {
                printf("Erro ao abrir imagem\n");
            }

            printf("Digite o dígito esperado: ");
            scanf("%d", &e);

            if (elm_start(img) < 0) {
                printf("Erro na inferência\n");
                elm_close();
                return -1;
            }

            r = elm_result();
            printf("A imagem foi inferida como: %d\n", r);
            printf("Era esperado: %d\n", e);
            r == e ? printf("Inferência correta\n") : printf("Inferência incorreta\n");

            enter();
            vga_reset();

        } else if (op == 2) {
            for (int i = 0; i < 784; i++){
                img[i] = 0;
            }

            draw(img);

            printf("Digite o dígito esperado: ");
            scanf("%d", &e);

            if (elm_start(img) < 0) {
                printf("Erro na inferência\n");
                elm_close();
                return -1;
            }

            r = elm_result();
            printf("A imagem foi inferida como: %d\n", r);
            printf("Era esperado: %d\n", e);
            r == e ? printf("Inferência correta\n") : printf("Inferência incorreta\n");

            enter();
            vga_reset();

        } else if (op == 3) {
            DIR *dir = opendir("test");
            struct dirent *entry;
            if (dir == NULL) {
                printf("Erro abrindo pasta\n");
                continue;
            }

            while ((entry = readdir(dir)) != NULL) {
                strcpy(path, "test/");
                strcat(path, entry->d_name);
                if (entry->d_name[0] == '.') continue;

                e = entry->d_name[0] - '0';

                FILE *f = fopen(path, "rb");
                if (f) {
                    if (fread(img, 1, 784, f) == 784) {
                        vga_draw(img);
                        printf("Imagem exibida\n");
                    } else {
                        printf("Erro no fread\n");
                        eimg++;
                        continue;
                    }
                    fclose(f);
                } else {
                    printf("Erro ao abrir imagem\n");
                    eimg++;
                    continue;
                }

                if (elm_start(img) < 0) {
                    printf("Erro na inferência\n");
                    elm_close();
                    return -1;
                }

                r = elm_result();
                (r == e) ? ok++ : wrng++;
            }

            printf("------------------------------------------\n");
            printf("MÉTRICAS BENCHMARK\n");
            printf("------------------------------------------\n");
            printf("Total de imagens: %d\n", ok + wrng + eimg);
            printf("Imagens inferidas corretamente: %d\n", ok);
            printf("Imagens inferidas incorretamente: %d\n", wrng);
            printf("Erros ao carregar imagem: %d\n", eimg);

            closedir(dir);
        } else {
            printf("Opção inválida\n");
        }
    } while (op != 4);

    elm_close();
    return 0;
}