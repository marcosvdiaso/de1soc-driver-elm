#ifndef _POSIX_C_SOURCE
#define _POSIX_C_SOURCE 199309L
#endif

#include "driver.h"
#include "vga.h"
#include <stdio.h>
#include <stdint.h>
#include "mouse.h"
#include <string.h>
#include <dirent.h>
#include <time.h>
#include <math.h>

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
    int eimg = 0, einf = 0;
    int ok = 0, wrng = 0;
    struct timespec t1_lat, t2_lat;
    double lat = 0;
    double s, thr, diff, jitter;
    double var = 0;

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
            eimg = 0, einf = 0;
            ok = 0, wrng = 0;
            lat = 0;
            var = 0;

            DIR *dir = opendir("test");
            struct dirent *entry;
            if (dir == NULL) {
                printf("Erro abrindo pasta\n");
                continue;
            }

            int total = 0, i=0;
            while ((entry = readdir(dir)) != NULL) {
                if (entry->d_name[0] == '.') continue;
                total++;
            }
            rewinddir(dir);

            double lats[total];

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

                clock_gettime(CLOCK_MONOTONIC, &t1_lat);
                if (elm_start(img) < 0) {
                    printf("Erro na inferência\n");
                    einf++;
                    continue;
                }
                clock_gettime(CLOCK_MONOTONIC, &t2_lat);
                lats[i] = (t2_lat.tv_sec - t1_lat.tv_sec) * 1e9 + (t2_lat.tv_nsec - t1_lat.tv_nsec);
                lat += lats[i];
                i++;

                r = elm_result();
                (r == e) ? ok++ : wrng++;
                printf("Imagem \"%s\" foi inferida como: %d e era esperado: %d\n", entry->d_name, r, e);
            }

            s = lat / 1e9;
            thr = (total - (einf+eimg)) / s;
            lat /= (total - (einf+eimg));
            
            for (int i =0; i < (total - (einf+eimg)); i++){
                diff = lats[i] - lat;
                var += diff * diff;
                }
            jitter = sqrt(var/(total - (einf+eimg)));

            printf("\n");
            printf("------------------------------------------\n");
            printf("MÉTRICAS BENCHMARK\n");
            printf("------------------------------------------\n");
            printf("Total de imagens: %d\n", total);
            printf("Imagens inferidas corretamente: %d\n", ok);
            printf("Imagens inferidas incorretamente: %d\n", wrng);
            printf("Erros ao carregar imagem: %d\n", eimg);
            printf("Erros ao iniciar inferência: %d\n", einf);
            printf("Acurácia: %.2f%% (%d imagens de %d inferidas)\n", (float)ok / (total - einf) * 100, ok, total - einf);
            printf("Latência: %.0f ns\n", lat);
            printf("Throughput: %.2f inferencias/s\n", thr);
            printf("Desvio padrão: %.0f ns\n", jitter);

            closedir(dir);
        } else {
            printf("Opção inválida\n");
        }
    } while (op != 4);

    elm_close();
    return 0;
}