#include <stdio.h>


void smr_csv(FILE *stream, int total, int ok, int wrng, int eimg, int einf, double lat, double thr, double jitter) {
    fprintf(stream, "\nMÉTRICAS BENCHMARK\n");
    fprintf(stream, "Total de imagens,Imagens inferidas corretamente,Imagens inferidas incorretamente,Erros ao carregar imagem,Erros ao iniciar inferência,Acurácia,Latência (ns),Throughput (inferencias/s),Desvio padrão (ns)\n");
    fprintf(stream, "%d,%d,%d,%d,%d,%.2f%%,%.0f,%.2f,%.0f\n", total, ok, wrng, eimg, einf, (float)ok / (total - einf) * 100, lat, thr, jitter);
}

void infs_csv(FILE *stream, char *name, int r, int e, double lat, int i) {
    fprintf(stream, "Inferência nº [%d],%s,%d,%d,%s,%.0f\n", i+1, name, r, e, (r == e) ? "Correta" : "Incorreta", lat);
}

FILE *create_csv(){
    // https://www.ibm.com/docs/pt-br/i/7.5.0?topic=functions-fprintf-write-formatted-data-stream
    FILE *stream;
    char name[1024];

    printf("Digite o nome que deseja salvar o arquivo csv: \n");
    getchar();
    fgets(name, sizeof(name), stdin);
    name[strcspn(name, "\n")] = 0;
    strcat(name, ".csv");
    
    stream = fopen(name, "w");
    return stream;
}