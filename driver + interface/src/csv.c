#include <stdio.h>
#include <string.h>
#include <time.h>


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
    // https://stackoverflow.com/questions/14916527/writing-to-a-csv-file-in-c]
    // https://www.w3schools.com/c/c_date_time.php
    FILE *stream;

    time_t lt = time(NULL);
    struct tm *t = localtime(&lt);

    char name[] = "benchmark_00000000_000000.csv";
    snprintf(name, sizeof(name), "benchmark_%04d%02d%02d_%02d%02d%02d.csv", t->tm_year + 1900, t->tm_mon + 1, t->tm_mday, t->tm_hour, t->tm_min, t->tm_sec);

    stream = fopen(name, "w");
    if (stream == NULL) {
        printf("Erro ao criar arquivo CSV\n");
        return NULL;
    }
    fprintf(stream, "Nº,Arquivo,Predito,Esperado,Resultado,Latência (ns)\n");
    return stream;
}