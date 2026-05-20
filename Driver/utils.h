#ifndef UTILS_H
#define UTILS_H

#include <stddef.h>

int load_file(const char *path, void *buffer, size_t elem_size, size_t size);
void read_path(const char *msg, char *buffer, int size);
void benchmark(int digit);

#endif