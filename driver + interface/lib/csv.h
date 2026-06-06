#ifndef CSV_H
#define CSV_H

#include <stdio.h>

void smr_csv(FILE *stream, int total, int ok, int wrng, int eimg, int einf, double lat, double thr, double jitter);
void infs_csv(FILE *stream, char *name, int r, int e, double lat, int i);
FILE *create_csv();

#endif
