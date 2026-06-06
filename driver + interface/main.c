// duvida: tem problema usar o mesmo github do marco 2?

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
#include "csv.h"

void enter() {
    printf("Pressione enter para continuar\n");
    fflush(stdout);
    
    int c;
    while ((c = getchar()) != '\n' && c != EOF) {
        continue;
    }
}

int main(int argc, char *argv[]){ // duvida: o que seriam os parametros aqui...? tipo, tem o path, mas e os "parametros", opmode?
    /*
    ideias:
    1. opmode + path (ou imagem ou de arquivos de teste, ou nada se for draw), mas ai perco a reusabilidade com menu
    2. path img + path teste, ai fica os 3 modos usaveis, mas tem que passar os 2 paths
    3. so path de imagem, ai o benchmark fica hardcoded mesmo do test
    */
    
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
            if (argc < 2) {
                printf("Não passou path no CLI\n", argv[0]);
                continue;
            }
            strcpy(path, argv[1]);

            FILE *f = fopen(path, "rb");
            if (f) {
                if (fread(img, 1, 784, f) == 784) {
                    vga_border();
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

            // https://stackoverflow.com/questions/1271064/how-do-i-loop-through-all-files-in-a-folder-using-c
            DIR *dir = opendir("test");
            struct dirent *entry;
            if (dir == NULL) {
                printf("Erro abrindo pasta\n");
                continue;
            }

            FILE *stream = create_csv();
            if (stream == NULL) {
                closedir(dir);
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

                r = elm_result();
                (r == e) ? ok++ : wrng++;
                printf("Imagem \"%s\" foi inferida como: %d e era esperado: %d\n", entry->d_name, r, e);
                infs_csv(stream, entry->d_name, r, e, lats[i], i);
                i++;
            }

            /*
            duvida: faz sentido eu calcular latencia, throughput e jitter desconsiderando os erros de inferência 
            e de leitura de imagem? tipo, se a imagem não for lida ou a inferência não for feita, 
            não tem como eu calcular a latencia dela mesmo, entao faz sentido eu excluir esses casos do 
            calculo, mas tenho duvidas sobre
            */

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

            smr_csv(stream, total, ok, wrng, eimg, einf, lat, thr, jitter);
            fclose (stream);

            closedir(dir);
        } else {
            printf("Opção inválida\n");
        }
    } while (op != 4);

    elm_close();
    return 0;
}