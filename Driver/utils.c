#include <stdio.h>
#include <string.h>
#include "driver.h"
#include "utils.h"

#include <stdio.h>

/*
FUNÇÃO GENÉRICA PARA CARREGAMENTO DE ARQUIVOS

PARÂMETROS:
  * PATH -> CAMINHO DO ARQUIVO
  * BUFFER -> BUFFER
  * ELEM_SIZE -> TAMANHO DE CADA ELEMENTO EM BYTE
  * SIZE -> QUANTIDADE DE ELEMENTO
*/
int load_file(const char *path,void *buffer,size_t elem_size,size_t size) {
    FILE *f = fopen(path, "rb");

    if (f == NULL) {
        return -1;
    }

    size_t n = fread(buffer, elem_size, size, f);
    fclose(f);

    if (n != size) {
        return -1;
    }

    return 0;
}

/*
FUNÇÃO GENÉRICA PARA LER CAMINHO DE ARQUIVOS

PARÂMETROS:
  * MSG -> MENSAGEM A SER EXIBIDA
  * BUFFER -> BUFFER
  * SIZE -> TAMANHO P/ FGETS
*/
void read_path(const char *msg, char *buffer, int size) {
    printf("%s", msg);

    fgets(buffer, size, stdin);

    buffer[strcspn(buffer, "\n")] = 0;
}

/*
FUNÇÃO PARA TESTE MÚLTIPLO DO DRIVER

PARÂMETROS:
  * DIGIT -> DIGITO PREDITO ESPERADO
*/
void benchmark(int digit) {
    int ok = 0;
    int n = 0;
    int t;

    printf("Quantos testes? ");
    scanf("%d", &t);

    int results[t];

    for (int i = 0; i < t; i++) {
        if (elm_start() < 0) {
            printf("Erro ao iniciar\n");
            return;
        }

        int r = elm_result();
        results[i] = r;
        if (r == digit) {
            ok++;
        } else {
            n++;
        }

        printf("Teste nº %d -> %d\n", i + 1, r);
    }

    float acc = (ok * 100.0f) / t;
    float err = (n * 100.0f) / t;

    printf("Resultados: \n");
    printf("Acertos: %d\n", ok);
    printf("Erros: %d\n", n);
    printf("Precisao: %.2f%%\n", acc);
    printf("Erro: %.2f%%\n", err);
}