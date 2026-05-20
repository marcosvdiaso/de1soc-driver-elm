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
    printf("6. Abrir MMAP\n");
    printf("0. Sair\n");

    do {
        printf("Digite sua opção: ");
        scanf("%d", &op);
        getchar();
    } while(op <= -1 || op >= 7);

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
    int open = 0;
    int img_ok = 0;
    int w_ok = 0;
    int b_ok = 0;
    int beta_ok = 0;

    do {
        op = menu();

        if (op == 1){
            read_path("Caminho da imagem: ", path_img, sizeof(path_img));
            if (load_file(path_img, img, 1, 784) < 0) {
                printf("Erro carregando imagem\n");
                img_ok = 0;
            } else {
                printf("Imagem carregada\n");
                img_ok = 1;
            }
        } else if (op == 2){
            read_path("CAminho do bias: ", path_bias, sizeof(path_bias));
            if (load_file(path_bias, bias, sizeof(unsigned short), 128) < 0) {
                printf("Erro carregando bias\n");
                b_ok = 0;
            } else {
                printf("bias carregada\n");
                b_ok = 1;
            }
        } else if (op == 3) {
            read_path("Caminho dos weights: ", path_weights, sizeof(path_weights));
            if (load_file(path_weights, weights, sizeof(unsigned short), 100352) < 0) {
                printf("Erro carregando weights\n");
                w_ok = 0;
            }else {
                printf("wheigts carregada\n");
                w_ok = 1;
            }
        } else if (op == 4){
            read_path("Caminho do beta: ", path_beta, sizeof(path_beta));
            if (load_file(path_beta, beta, sizeof(unsigned short), 1280) < 0) {
                printf("Erro carregando beta\n");
                beta_ok = 0;
            } else {
                printf("beta carregada\n");
                beta_ok =1;
            }
        } else if (op == 5){
            if (!b_ok || !w_ok || !img_ok || !beta_ok){
                printf("Verifique se todos os arquivos estão carregados.\n");
                continue;
            } else if (!open){
                printf("mmap não aberto\n");
                continue;
            }
            printf("Digite o digito esperado: ");
            scanf("%d", &digit);
            getchar();
        
            elm_reset();

            if (elm_store_img(img) < 0) {
                printf("Erro enviando imagem\n");
                continue;
            }

            if (elm_store_bias(bias) < 0) {
                printf("Erro enviando bias\n");
                continue;
            }

            if (elm_store_beta(beta) < 0) {
                printf("Erro enviando beta\n");
                continue;
            }

            if (elm_store_weights(weights) < 0) {
                printf("Erro enviando weights\n");
                continue;
            }

            benchmark(digit);
        } else if (op == 6){
            if (!open){
                if (elm_open() < 0) {
                    printf("Erro no open\n");
                } else {
                    printf("Open ok!\n");
                    open = 1;
                }
            } else {
            printf("mmap já aberto.\n");
            }
        }
    } while (op!=0);

    if (open){
        elm_close();
    }
    return 0;
}