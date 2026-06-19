// duvida: tem problema usar o mesmo github do marco 2?
// benchamrk seg fault
// mouse lento
// botao direito nao apaga direito, o ciclo de troca entre esq e dir ta ruim
// rastrejo preto no mouse
// enter nf

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
#include <stdlib.h>

// TODO: separar modos em funcs (MODULARIZACAO)


void enter(void)
{
    int c;

    while ((c = getchar()) != '\n' && c != EOF);

    printf("Pressione Enter para continuar...");
    fflush(stdout);

    getchar();
}

int main(int argc, char *argv[]){ // duvida: o que seriam os parametros aqui...? tipo, tem o path, mas e os "parametros", opmode?
    /*
    ideias:
    1. opmode + path (ou imagem ou de arquivos de teste, ou nada se for draw), mas ai perco a reusabilidade com menu
    2. path img + path teste, ai fica os 3 modos usaveis, mas tem que passar os 2 paths
    3. so path de imagem, ai o benchmark fica hardcoded mesmo do test

    se for pegar path de teste tambem, vai ter que trocar como funciona pra pegar a var e
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
            if (argv[1] == NULL) {
                printf("Path deve ser passado como primeiro argumento, exemplo: %s test/0_0.bin\n", argv[0]);
                printf("Não passou path no CLI, digite o path abaixo: \n");
                getchar();
                fgets(path, 1024, stdin);
                path[strcspn(path, "\n")] = 0;
            } else {
                strcpy(path, argv[1]);
            }

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
            
            elm_reset();

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

            vga_border();
            draw(img);

            printf("Digite o dígito esperado: ");
            scanf("%d", &e);
            
            elm_reset();

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

            eimg = 0;
            einf = 0;
            ok = 0;
            wrng = 0;
            lat = 0;
            var = 0;

            DIR *dir = opendir("test");
            struct dirent *entry;

            if (dir == NULL) {
                printf("Erro abrindo pasta test\n");
                continue;
            }

            FILE *stream = create_csv();

            if (stream == NULL) {
                closedir(dir);
                continue;
            }

            /* Conta imagens válidas */
            int total = 0;

            while ((entry = readdir(dir)) != NULL) {
                if (entry->d_name[0] == '.')
                    continue;

                total++;
            }

            rewinddir(dir);

            if (total == 0) {
                printf("Nenhuma imagem encontrada.\n");

                fclose(stream);
                closedir(dir);
                continue;
            }

            double *lats = malloc(total * sizeof(double));

            if (lats == NULL) {
                printf("Erro alocando memória.\n");

                fclose(stream);
                closedir(dir);
                continue;
            }

            int validas = 0;

            printf("Iniciando benchmark...\n");

            while ((entry = readdir(dir)) != NULL) {

                if (entry->d_name[0] == '.')
                    continue;

                snprintf(path,
                        sizeof(path),
                        "test/%s",
                        entry->d_name);

                e = entry->d_name[0] - '0';

                FILE *f = fopen(path, "rb");

                if (f == NULL) {
                    printf("Erro abrindo %s\n", path);

                    eimg++;
                    continue;
                }

                if (fread(img, 1, 784, f) != 784) {

                    printf("Erro lendo %s\n", path);

                    fclose(f);

                    eimg++;
                    continue;
                }

                fclose(f);

                clock_gettime(CLOCK_MONOTONIC, &t1_lat);


                elm_reset();
                
                

                if (elm_start(img) < 0) {

                    printf("Erro iniciando inferência\n");

                    einf++;
                    continue;
                }


                r = elm_result();

                clock_gettime(CLOCK_MONOTONIC, &t2_lat);

                double tempo =
                    (t2_lat.tv_sec - t1_lat.tv_sec) * 1e9 +
                    (t2_lat.tv_nsec - t1_lat.tv_nsec);

                lats[validas] = tempo;

                lat += tempo;

                if (r == e)
                    ok++;
                else
                    wrng++;

                printf("[%d/%d] %s -> predito: %d | esperado: %d\n",
                    validas + 1,
                    total,
                    entry->d_name,
                    r,
                    e);

                infs_csv(stream,
                        entry->d_name,
                        r,
                        e,
                        tempo,
                        validas);

                validas++;
            }

            if (validas > 0) {

                lat /= validas;

                for (int j = 0; j < validas; j++) {

                    diff = lats[j] - lat;

                    var += diff * diff;
                }

                jitter = sqrt(var / validas);

                s = 0;

                for (int j = 0; j < validas; j++) {
                    s += lats[j];
                }

                s /= 1e9;

                thr = validas / s;
            }
            else {

                lat = 0;
                jitter = 0;
                thr = 0;
            }

            printf("\n");
            printf("------------------------------------------\n");
            printf("MÉTRICAS BENCHMARK\n");
            printf("------------------------------------------\n");

            printf("Total encontrado: %d\n", total);

            printf("Inferências válidas: %d\n", validas);

            printf("Corretas: %d\n", ok);

            printf("Incorretas: %d\n", wrng);

            printf("Erros ao carregar imagem: %d\n", eimg);

            printf("Erros ao iniciar inferência: %d\n", einf);

            if (validas > 0) {

                printf("Acurácia: %.2f%%\n",
                    100.0 * ok / validas);

                printf("Latência média: %.0f ns\n",
                    lat);

                printf("Throughput: %.2f inferências/s\n",
                    thr);

                printf("Desvio padrão: %.0f ns\n",
                    jitter);
            }

            smr_csv(stream,
                    total,
                    ok,
                    wrng,
                    eimg,
                    einf,
                    lat,
                    thr,
                    jitter);

            free(lats);

            fclose(stream);

            closedir(dir);

            enter();
        } else {
            printf("Opção inválida\n");
        }
    } while (op != 4);

    elm_close();
    return 0;
}
