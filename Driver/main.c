#include <stdio.h>

#include "driver.h"
#include "utils.h"

int menu(){
    int op;
    printf("1. Carregar imagem\n");
    printf("2. Carregar bias\n");
    printf("3. Carregar weights\n");
    printf("4. Carregar beta\n");
    printf("5. Iniciar inferência\n");
    printf("0. Sair\n");

    do {
        printf("Digite sua opção: ");
        scanf("%d", &op);
        getchar();
    } while(op <= -1 || op >= 6);

    return op;
}

int main() {
    char path_img[1024];
    char path_bias[1024];
    char path_beta[1024];
    char path_weights[1024];

    unsigned char img[784];
    unsigned short bias[128];
    unsigned short beta[1280];
    unsigned short weights[100352];

    int digit;
    int op;

    do {
        op = menu();

        if (op == 1){
            read_path("Caminho da imagem: ", path_img, sizeof(path_img));
            if (load_file(path_img, img, 1, 784) < 0) {
                printf("Erro carregando imagem\n");
            } else {
                printf("Imagem carregada\n");
            }
        } else if (op == 2){
            read_path("CAminho do bias: ", path_bias, sizeof(path_bias));
            if (load_file(path_bias, bias, sizeof(unsigned short), 128) < 0) {
                printf("Erro carregando bias\n");
            } else {
                printf("bias carregada\n");
            }
        } else if (op == 3) {
            read_path("Caminho dos weights: ", path_weights, sizeof(path_weights));
            if (load_file(path_weights, weights, sizeof(unsigned short), 100352) < 0) {
                printf("Erro carregando weights\n");
            }else {
                printf("wheigts carregada\n");
            }
        } else if (op == 4){
            read_path("Caminho do beta: ", path_beta, sizeof(path_beta));
            if (load_file(path_beta, beta, sizeof(unsigned short), 1280) < 0) {
                printf("Erro carregando beta\n");
            } else {
                printf("beta carregada\n");
            }
        } else if (op == 5){
            printf("Digite o digito esperado: ");
            scanf("%d", &digit);
            getchar();
        
            if (elm_open() < 0) {
                    printf("Erro no open\n");
                    return 1;
                }
            elm_reset();

            if (elm_store_img(img) < 0) {
                printf("Erro enviando imagem\n");
                elm_close();
                return 1;
            }

            if (elm_store_bias(bias) < 0) {
                printf("Erro enviando bias\n");
                elm_close();
                return 1;
            }

            if (elm_store_beta(beta) < 0) {
                printf("Erro enviando beta\n");
                elm_close();
                return 1;
            }

            if (elm_store_weights(weights) < 0) {
                printf("Erro enviando weights\n");
                elm_close();
                return 1;
            }

            benchmark(digit);
            elm_close();
        }
    } while (op!=0);
    return 0;
}