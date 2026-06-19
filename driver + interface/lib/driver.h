#ifndef DRIVER_H
#define DRIVER_H

#include <stdint.h>

int elm_open(void);
void elm_close(void);
void elm_reset(void);
int elm_start(uint8_t *img);
int elm_result(void);
int elm_load(void);
void *elm_mmap(void);

#endif
